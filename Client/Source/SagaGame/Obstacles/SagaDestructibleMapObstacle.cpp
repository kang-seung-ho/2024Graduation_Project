#include "Obstacles/SagaDestructibleMapObstacle.h"
#include <Components/SkeletalMeshComponent.h>
#include <NiagaraFunctionLibrary.h>
#include <NiagaraComponent.h>

#include "SagaGameSubsystem.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaDestructibleMapObstacle::ASagaDestructibleMapObstacle()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize components
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));


	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ObstacleMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Terrains/Modelings/Pinata/Pinata.Pinata'"));
	if (ObstacleMeshRef.Succeeded())
	{
		MeshComponent->SetSkeletalMesh(ObstacleMeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> DeathAnimFinder(TEXT("/Script/Engine.AnimSequence'/Game/Terrains/Modelings/Pinata/Pinata_Anim_Bomb.Pinata_Anim_Bomb'"));
	if (DeathAnimFinder.Succeeded())
	{
		DeathAnimation = DeathAnimFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> HitAnimRef(TEXT("/Script/Engine.AnimSequence'/Game/Terrains/Modelings/Pinata/Pinata_Anim_Hit_01.Pinata_Anim_Hit_01'"));
	if (HitAnimRef.Succeeded())
	{
		HitAnimation = HitAnimRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraEffect(TEXT("/Script/Niagara.NiagaraSystem'/Game/VFX/PartyFX/Niagara/NS_Ribbon_Party_2.NS_Ribbon_Party_2'"));
	if (NiagaraEffect.Succeeded())
	{
		HitEffect = NiagaraEffect.Object;
		UE_LOG(LogTemp, Warning, TEXT("Niagara Effect Loaded"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Niagara Effect Not Loaded"));
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> PinataNiagaraEffect(TEXT("/Script/Niagara.NiagaraSystem'/Game/VFX/VFX_Hit/NS_Hit.NS_Hit'"));
	if (PinataNiagaraEffect.Succeeded())
	{
		PinataHitEffect = PinataNiagaraEffect.Object;
		UE_LOG(LogTemp, Warning, TEXT("Pinata Hit Effect Loaded"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Pinata Hit Effect Not Loaded"));
	}

	// Set up Mesh component as a Root Component
	RootComponent = MeshComponent;

	MeshComponent->SetCollisionProfileName(TEXT("Enemy")); // Collision Enabled for both Red and Blue Teams
	SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));
}

float
ASagaDestructibleMapObstacle::TakeDamage(float dmg, FDamageEvent const& event, AController* instigator, AActor* causer)
{
	float DamageApplied = Super::TakeDamage(dmg, event, instigator, causer);

	UE_LOG(LogTemp, Warning, TEXT("Obstacle took %f damage, current health: %f"), dmg, myHealth);

	myHealth -= DamageApplied;

	const auto world = GetWorld();
	auto& timer = GetWorldTimerManager();
	const auto sys = USagaGameSubsystem::GetSubSystem(world);
	const auto net = USagaNetworkSubSystem::GetSubSystem(world);

	const auto rot = GetActorRotation();
	const auto loc = GetActorLocation();

	if (myHealth <= 0)
	{
		FVector SpawnLocation = loc + FVector(0.0f, 0.0f, 40.0f);
		FRotator SpawnRotation = FRotator::ZeroRotator;

		FActorSpawnParameters SpawnParameters{};
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (DeathAnimation && MeshComponent)
		{
			MeshComponent->PlayAnimation(DeathAnimation, false);
		}

		FVector NiagaraSpawnLocation = loc + FVector(0.0f, 0.0f, 100.0f);
		FRotator NiagaraSpawnRotation = rot;

		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(HitEffect, RootComponent, NAME_None, NiagaraSpawnLocation, NiagaraSpawnRotation, EAttachLocation::KeepWorldPosition, false);

		if (NiagaraComponent)
		{
			// 3초 후에 나이아가라 이펙트 정지
			FTimerHandle temp_timer{};

			timer.SetTimer(temp_timer, NiagaraComponent, &UNiagaraComponent::Deactivate, 3.0f, false);
		}

		// Uncomment or modify this line as necessary to spawn an item
		// world->SpawnActor<ASagaItemBox>(ASagaItemBox::StaticClass(), SpawnLocation, SpawnRotation, SpawnParameters);

		// Set a timer to destroy the obstacle after 3 seconds
		timer.SetTimer(TimerHandle, this, &ASagaDestructibleMapObstacle::DestroyObstacle, 4.0f, false);

		timer.SetTimer(LevelChangeTimerHandle, this, &ASagaDestructibleMapObstacle::ChangeLevel, 5.0f, false);

		if (net->IsOfflineMode())
		{
			if (TeamPinataColor == 0)
			{
				// blue win
				sys->SetWhoWonByPinata(0);
			}
			else
			{
				// red win
				sys->SetWhoWonByPinata(1);
			}
		}
		else
		{
			// NOTICE: RPC_DESTROY_CORE
			// arg0: nothing
			// arg1: winning team's id
			// (0 - nothing, 1 - red, 2 - blue)
			if (TeamPinataColor == 0)
			{
				// blue team won
				net->SendRpcPacket(ESagaRpcProtocol::RPC_DESTROY_CORE, 0, 2);
			}
			else if (TeamPinataColor == 1)
			{
				// red team won
				net->SendRpcPacket(ESagaRpcProtocol::RPC_DESTROY_CORE, 0, 1);
			}
			else
			{
#if WITH_EDITOR

				const auto name = GetName();
				UE_LOG(LogSagaGame, Error, TEXT("[ASagaDestructibleMapObstacle][TakeDamage] '%s' has an invalid team id '%d'"), *name, TeamPinataColor);
#endif
			}
		}
	}
	else
	{
		if (HitAnimation && MeshComponent)
		{
			MeshComponent->PlayAnimation(HitAnimation, false);
			//Play Hit Niagara Effect here
			FVector SpawnLocation = GetActorLocation();
			FRotator SpawnRotation = GetActorRotation();
			UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PinataHitEffect, SpawnLocation, SpawnRotation);
			if (NiagaraComponent)
			{
				NiagaraComponent->SetAutoDestroy(true);
			}

		}
	}

	return DamageApplied;
}

void
ASagaDestructibleMapObstacle::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void
ASagaDestructibleMapObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void
ASagaDestructibleMapObstacle::DestroyObstacle()
{
	Destroy();
}

void
ASagaDestructibleMapObstacle::ChangeLevel()
{
	UGameplayStatics::OpenLevel(this, FName("GameEndLevel"));
}
