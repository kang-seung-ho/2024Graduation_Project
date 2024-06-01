#include "SagaPlayableCharacter.h"
#include "SagaPlayerAnimInstance.h"
#include "../Effect/SagaSwordEffect.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


ASagaPlayableCharacter::ASagaPlayableCharacter()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/PlayerAssets/Player.Player'"));
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}

	GetMesh()->SetRelativeLocation(FVector(0.0, 0.0, -88.0));
	GetMesh()->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimAsset(TEXT("AnimBlueprint'/Game/PlayerAssets/Animation/AB_SagaPlayer.AB_SagaPlayer_C'"));
	if (AnimAsset.Succeeded())
	{
		humanCharacterAnimation = AnimAsset.Class;
	}

	/*mArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	mArm->SetupAttachment(GetMesh());

	mCamera->SetupAttachment(mArm);

	mArm->SetRelativeLocation(FVector(0.0, 0.0, 150.0));
	mArm->SetRelativeRotation(FRotator(-15.0, 90.0, 0.0));

	mArm->TargetArmLength = 150.f;*/

	// ASagaPlayableCharacter 클래스 내 생성자에 추가
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraEffect(TEXT("Niagara.NiagaraSystem'/Game/PartyFX/Niagara/NS_Fireworks_Star.NS_Fireworks_Star'"));
	if (NiagaraEffect.Succeeded())
	{
		HitEffect = NiagaraEffect.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> CascadeEffect(TEXT("ParticleSystem'/Game/Hit_VFX/VFX/Hard_Hit/P_Hit_5.P_Hit_5'"));
	if (CascadeEffect.Succeeded())
	{
		HitCascadeEffect = CascadeEffect.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> GunCascadeEffect(TEXT("ParticleSystem'/Game/Hit_VFX/VFX/Hard_Hit/P_Hit_3.P_Hit_3'"));
	if (GunCascadeEffect.Succeeded())
	{
		GunHitCascadeEffect = GunCascadeEffect.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> HitSoundEffectObject(TEXT("SoundWave'/Game/PlayerAssets/Sounds/Damage.Damage'"));
	if (HitSoundEffectObject.Succeeded())
	{
		HitSoundEffect = HitSoundEffectObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> DeadSoundEffectObject(TEXT("SoundWave'/Game/PlayerAssets/Sounds/Death.Death'"));
	if (DeadSoundEffectObject.Succeeded())
	{
		DeadSoundEffect = DeadSoundEffectObject.Object;
	}

	//Item get Action
	TakeItemAction.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ASagaPlayableCharacter::Acquire_Drink)));
	TakeItemAction.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ASagaPlayableCharacter::Acquire_Gum)));
	TakeItemAction.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ASagaPlayableCharacter::Acquire_smokebomb)));
}

void
ASagaPlayableCharacter::RideNPC()
{
	UE_LOG(LogSagaGame, Warning, TEXT("RideNPC Called"))
		FOutputDeviceNull Ar;

	bool ret = CallFunctionByNameWithArguments(TEXT("RidingFunction"), Ar, nullptr, true);
	if (ret == true)
	{
		UE_LOG(LogSagaGame, Warning, TEXT("RidingFunction Called"))
	}
	else
	{
		UE_LOG(LogSagaGame, Warning, TEXT("RidingFunction Not Found"))
	}
}

void
ASagaPlayableCharacter::BeginPlay()
{
	Super::BeginPlay();
	//myWeaponType = EPlayerWeapon::LightSabor;
	UE_LOG(LogSagaGame, Warning, TEXT("Playable Character BeginPlay"));
}

void
ASagaPlayableCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

float
ASagaPlayableCharacter::ExecuteHurt(const float dmg)
{
	UE_LOG(LogSagaGame, Log, TEXT("[Character] ExecuteHurt (%f)"), dmg);

	Stat->ApplyDamage(dmg);
	myHealth -= dmg;

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	FVector NiagaraSpawnLocation = GetActorLocation();
	FRotator NiagaraSpawnRotation = GetActorRotation();

	FVector EffectSpawnLocation = GetActorLocation();
	FRotator EffectSpawnRotation = GetActorRotation();

	

	if (dmg == 30.f)
	{
		if (HitCascadeEffect)
		{
			UParticleSystemComponent* CascadeComponent = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				HitCascadeEffect,
				EffectSpawnLocation,
				EffectSpawnRotation,
				true
			);

			if (CascadeComponent)
			{
				FTimerHandle TimerHandle;
				GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &ASagaPlayableCharacter::DeactivateCascadeEffect, CascadeComponent), 3.0f, false);
			}
		}
	}
	else if (dmg == 20.f)
	{
		if (GunHitCascadeEffect)
		{
			UParticleSystemComponent* CascadeComponent = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				GunHitCascadeEffect,
				EffectSpawnLocation,
				EffectSpawnRotation,
				true
			);

			if (CascadeComponent)
			{
				FTimerHandle TimerHandle;
				GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &ASagaPlayableCharacter::DeactivateCascadeEffect, CascadeComponent), 3.0f, false);
			}
		}
	}
	else
	{
		if (HitEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, NiagaraSpawnLocation, NiagaraSpawnRotation);
		}
	}
	
	

	if (myHealth <= 0.0f)
	{
		// 사망 애니메이션 실행
		mAnimInst->Death();

		if (DeadSoundEffect)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DeadSoundEffect, GetActorLocation());
		}

		// 사망 처리 (이동 정리, 충돌 해제)
		ExecuteDeath();

		// 리스폰 함수 실행
		// RespawnCharacter 함수 3초 뒤	실행
		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ASagaPlayableCharacter::RespawnCharacter, 3.0f, false);

		// 상대 팀 점수 증가 실행
		system->AddScore(myTeam == EUserTeam::Red ? EUserTeam::Blue : EUserTeam::Red, 1);

		// arg1이 0이면 사람 캐릭터
		system->SendRpcPacket(ESagaRpcProtocol::RPC_DMG_PLYER, myHealth, 0);
	}
	else
	{
		mAnimInst->Hit();

		if (HitSoundEffect)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSoundEffect, GetActorLocation());
		}

		if (not system->IsOfflineMode())
		{
			// arg1이 0이면 사람 캐릭터
			system->SendRpcPacket(ESagaRpcProtocol::RPC_DMG_PLYER, myHealth, 0);
		}
		else
		{

		}
	}

	return dmg;
}

void ASagaPlayableCharacter::DeactivateCascadeEffect(UParticleSystemComponent* ParticleComponent)
{
	if (ParticleComponent)
	{
		ParticleComponent->Deactivate();
	}
}


void
ASagaPlayableCharacter::ExecuteDeath()
{
	UE_LOG(LogSagaGame, Log, TEXT("[Character] ExecuteDeath"));

	Super::ExecuteDeath();
}

float
ASagaPlayableCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void
ASagaPlayableCharacter::RespawnCharacter()
{
	//
	Super::RespawnCharacter();

	// Animate
	mAnimInst->Revive();

	// Retrive collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// re-set collision profile name. NOT just Enable

	//GetCapsuleComponent()->SetCollisionProfileName(TEXT("Red"));
}

void
ASagaPlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASagaPlayableCharacter::Attack()
{
	Super::Attack();

	FHitResult Result;
	FVector Start, End;
	FCollisionQueryParams Params;
	bool Collision = false;

	if (myWeaponType == EPlayerWeapon::LightSabor)
	{
		// Calculate attack start and end based on the camera's rotation
		FRotator CameraRotation = GetControlRotation();
		FVector ForwardVector = CameraRotation.Vector();
		Start = GetActorLocation() + ForwardVector * 50.f;
		End = Start + ForwardVector * 150.f;

		Params.AddIgnoredActor(this);
		Collision = GetWorld()->SweepSingleByChannel(Result, Start, End, FQuat::Identity, ECC_GameTraceChannel4, FCollisionShape::MakeSphere(50.f), Params);
	}
	else if (myWeaponType == EPlayerWeapon::WaterGun)
	{
		Start = GetActorLocation();
		End = GetActorLocation() + GetActorForwardVector() * 1500.f;

		Params.AddIgnoredActor(this);
		Collision = GetWorld()->LineTraceSingleByChannel(Result, Start, End, ECC_GameTraceChannel4, Params);
	}
	else if (myWeaponType == EPlayerWeapon::Hammer)
	{
		Start = GetActorLocation() + GetActorForwardVector() * 50.f;
		End = Start + GetActorForwardVector() * 150.f;

		Params.AddIgnoredActor(this);
		Collision = GetWorld()->SweepSingleByChannel(Result, Start, End, FQuat::Identity, ECC_GameTraceChannel4, FCollisionShape::MakeSphere(50.f), Params);
	}

	if (Collision)
	{
		FVector HitLocation = Result.ImpactPoint;
		FVector HitNormal = Result.Normal;
		float DamageAmount = (myWeaponType == EPlayerWeapon::WaterGun) ? 20.f : 30.f;

		FDamageEvent DamageEvent;
		Result.GetActor()->TakeDamage(DamageAmount, DamageEvent, GetController(), this);

		// Check if hit actor is a GummyBear player
		if (Result.GetActor()->IsA<ASagaGummyBearPlayer>())
		{
			Cast<ASagaGummyBearPlayer>(Result.GetActor())->TryDismemberment(HitLocation, HitNormal);
		}
	}
	else
	{
		UE_LOG(LogSagaGame, Log, TEXT("No Actors were hit"));
	}
}


void ASagaPlayableCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetMesh()->SetAnimInstanceClass(humanCharacterAnimation.LoadSynchronous());

	MyWeapon->SetCollisionProfileName(TEXT("Weapon"));
}

void
ASagaPlayableCharacter::Acquire_Drink(USagaWeaponData* ItemData)
{
	USagaWeaponData* AcquiredItemData = Cast<USagaWeaponData>(ItemData);
	if (AcquiredItemData)
	{
		//Send HP adding code or Item Acquiring code to server
		//And Save to Local Inventory
	}

	mItemType = AcquiredItemData->ItemType;
}

void
ASagaPlayableCharacter::Acquire_Gum(USagaWeaponData* ItemData)
{
	USagaWeaponData* AcquiredItemData = Cast<USagaWeaponData>(ItemData);
	if (AcquiredItemData)
	{
		//Item Acquiring code to server
		//And Save to Local Inventory
	}

	mItemType = AcquiredItemData->ItemType;
}

void
ASagaPlayableCharacter::Acquire_smokebomb(USagaWeaponData* ItemData)
{
	USagaWeaponData* AcquiredItemData = Cast<USagaWeaponData>(ItemData);
	if (AcquiredItemData)
	{
		//Item Acquiring code to server
		//And Save to Local Inventory
	}
	mItemType = AcquiredItemData->ItemType;
}

void
ASagaPlayableCharacter::TakeItem(USagaWeaponData* WeaponData)
{
	if (WeaponData)
	{
		TakeItemAction[(uint8)WeaponData->ItemType].ItemDelegate.ExecuteIfBound(WeaponData);
	}
}
