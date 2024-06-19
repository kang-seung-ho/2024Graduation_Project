// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Gumball.h"
#include "../Character/SagaCharacterBase.h"

// Sets default values
AGumball::AGumball()
{
    PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> GumballMesh(TEXT("/Script/Engine.StaticMesh'/Game/Terrains/Modelings/Candy/Lollipop_01_prop.Lollipop_01_prop'"));
    if (GumballMesh.Succeeded())
    {
        MeshComponent->SetStaticMesh(GumballMesh.Object);
    }
    MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereComponent->SetupAttachment(RootComponent);
    SphereComponent->SetSphereRadius(100.0f);

    // Change Collision Profile to OverlapAllDynamic
    SphereComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AGumball::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AGumball::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGumball::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ASagaCharacterBase* PlayerCharacter = Cast<ASagaCharacterBase>(OtherActor);
    if (PlayerCharacter)
    {
        // Reduce the player's speed by 50%
        OverlappingPlayer = PlayerCharacter;
        OverlappingPlayer->GetCharacterMovement()->MaxWalkSpeed *= 0.5f;

        // Set timer to reset the player's speed after 3 seconds
        GetWorld()->GetTimerManager().SetTimer(SpeedResetTimerHandle, this, &AGumball::ResetPlayerSpeed, 3.0f, false);

        // Set timer to destroy the Gumball after 3 seconds
        GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AGumball::DestroyGumball, 3.0f, false);
    }
}

void AGumball::DestroyGumball()
{
    Destroy();
}

void AGumball::ResetPlayerSpeed()
{
    if (OverlappingPlayer)
    {
        OverlappingPlayer->GetCharacterMovement()->MaxWalkSpeed /= 0.5f;

        // Log reset speed for debugging
        UE_LOG(LogTemp, Warning, TEXT("Player's speed reset to: %f"), OverlappingPlayer->GetCharacterMovement()->MaxWalkSpeed);

        OverlappingPlayer = nullptr;
    }
}

// Called every frame
void AGumball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

