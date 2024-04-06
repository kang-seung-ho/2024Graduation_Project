// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/SagaItemBox.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Physics/SagaCollision.h"
#include "Interface/SagaCharacterItemInterface.h"

ASagaItemBox::ASagaItemBox()
{
 	
    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));

    RootComponent = Trigger;
    Mesh->SetupAttachment(Trigger);
    Effect->SetupAttachment(Trigger);

    Trigger->SetCollisionProfileName(CPROFILE_SAGATRIGGER);
    Trigger->SetBoxExtent(FVector(40.0f, 42.0f, 30.0f));//크기 설정 (아이템별로 맞게 변경필요)
    Trigger->OnComponentBeginOverlap.AddDynamic(this, &ASagaItemBox::OnOverlapBegin); //블루프린트에서도 편집 가능하기때문에 여기에 들어가는 함수는 UFUNCTION 설정필요.

    static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/Map/Item/Cube.Cube'"));
    if (BoxMeshRef.Object)
    {
        Mesh->SetStaticMesh(BoxMeshRef.Object);
    }
    Mesh->SetRelativeLocation(FVector(0.0f, -3.5f, -30.0f));
    Mesh->SetCollisionProfileName(TEXT("NoCollision"));

    static ConstructorHelpers::FObjectFinder<UParticleSystem> EffectRef(TEXT("/Script/Engine.ParticleSystem'/Game/Effect/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh'"));
    if (EffectRef.Object)
    {
        Effect->SetTemplate(EffectRef.Object);
        Effect->bAutoActivate = false; //바로 발동하지 않도록
    }
}

void ASagaItemBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
    //아이템 획득 했을 때 획득한 Actor에게 TakeItem()함수를 호출하도록 한다.

    if (nullptr == Item) //아이템이 꽝
    {
        Destroy();
        return;
    }

    ISagaCharacterItemInterface* OverlappingPawn = Cast<ISagaCharacterItemInterface>(OtherActor);
    if (OverlappingPawn)
    {
        OverlappingPawn->TakeItem(Item);
    }

    Effect->Activate(true);
    Mesh->SetHiddenInGame(true);
    SetActorEnableCollision(false); //또 충돌하면 안되니까 비활성화
    Effect->OnSystemFinished.AddDynamic(this, &ASagaItemBox::OnEffectFinished); //이펙트 종료후 발동 delegate
}

void ASagaItemBox::OnEffectFinished(UParticleSystemComponent* ParticleSystem)
{
    Destroy();
}
