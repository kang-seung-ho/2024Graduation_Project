#include "Obstacles/SagaDestructibleMapObstacle.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


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

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraEffect(TEXT("/Script/Niagara.NiagaraSystem'/Game/PartyFX/Niagara/NS_Ribbon_Party_2.NS_Ribbon_Party_2'"));
    if (NiagaraEffect.Succeeded())
    {
        HitEffect = NiagaraEffect.Object;
        UE_LOG(LogTemp, Warning, TEXT("Niagara Effect Loaded"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Niagara Effect Not Loaded"));
    }

    // Set up Mesh component as a Root Component
    RootComponent = MeshComponent;

    MeshComponent->SetCollisionProfileName(TEXT("Enemy")); // Collision Enabled for both Red and Blue Teams
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));
}

void ASagaDestructibleMapObstacle::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ASagaDestructibleMapObstacle::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ASagaDestructibleMapObstacle::DestroyObstacle()
{
    Destroy();
}

float ASagaDestructibleMapObstacle::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    UE_LOG(LogTemp, Warning, TEXT("Obstacle took %f damage, current health: %f"), DamageAmount, health);

    health -= DamageApplied;

    if (health <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Obstacle is destroyed, spawning item..."));
        FVector SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, 40.0f);
        FRotator SpawnRotation = FRotator::ZeroRotator;
        FActorSpawnParameters SpawnParameters;

        if (DeathAnimation && MeshComponent)
        {
            MeshComponent->PlayAnimation(DeathAnimation, false);
        }

        FVector NiagaraSpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, 100.0f);
        FRotator NiagaraSpawnRotation = GetActorRotation();
        UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(HitEffect, RootComponent, NAME_None, NiagaraSpawnLocation, NiagaraSpawnRotation, EAttachLocation::KeepWorldPosition, false);
        if (NiagaraComponent)
        {
            UE_LOG(LogTemp, Warning, TEXT("Niagara Component Spawned"));
        }
        else {
            			UE_LOG(LogTemp, Warning, TEXT("Niagara Component Not Spawned"));
        }
        if (NiagaraComponent)
        {
            // 3초 후에 나이아가라 이펙트 정지
            FTimerHandle NiagaraTimerHandle;
            GetWorldTimerManager().SetTimer(NiagaraTimerHandle, NiagaraComponent, &UNiagaraComponent::Deactivate, 3.0f, false);
        }

        // Uncomment or modify this line as necessary to spawn an item
        // GetWorld()->SpawnActor<ASagaItemBox>(ASagaItemBox::StaticClass(), SpawnLocation, SpawnRotation, SpawnParameters);

        // Set a timer to destroy the obstacle after 3 seconds
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASagaDestructibleMapObstacle::DestroyObstacle, 3.0f, false);
    }
    else
    {
        if (HitAnimation && MeshComponent)
        {
            MeshComponent->PlayAnimation(HitAnimation, false);
        }
    }

    return DamageApplied;
}