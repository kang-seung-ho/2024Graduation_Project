#include "SagaItemBox.h"
#include <Math/UnrealMathUtility.h>
#include <Templates/Casts.h>
#include <Kismet/GameplayStatics.h>
#include <Components/BoxComponent.h>
#include <NiagaraSystem.h>
#include <NiagaraComponent.h>

#include "Item/SagaWeaponData.h"
#include "Interface/SagaCharacterItemInterface.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaItemBox::ASagaItemBox()
	: Super()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RootComponent = Trigger;

	Mesh->SetupAttachment(Trigger);

	Trigger->SetBoxExtent(FVector(30.f, 30.f, 30.f));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ASagaItemBox::OnOverlapBegin);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/Item/ItemBox/Box_Prop.Box_Prop'"));
	if (BoxMeshRef.Object)
	{
		Mesh->SetStaticMesh(BoxMeshRef.Object);
	}

	Mesh->SetRelativeLocation(FVector(0.f, -3.5f, -30.0f));
	Mesh->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraEffect(TEXT("/Script/Niagara.NiagaraSystem'/Game/VFX/VFX_Get/NS_Get.NS_Get'"));
	if (NiagaraEffect.Succeeded())
	{
		itemGrabEffect = NiagaraEffect.Object;
		UE_LOG(LogTemp, Warning, TEXT("Item Niagara Effect Loaded"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Item Niagara Effect Not Loaded"));
	}

	Trigger->SetCollisionProfileName(TEXT("Item"));

	// Make Mesh Collision Profile to NoCollision because the TriggerBox has the Item Collision Profile
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));
}

void
ASagaItemBox::SetItemId(int32 id)
noexcept
{
	myItemId = id;
}

void
ASagaItemBox::BeginPlay()
{
	Super::BeginPlay();
}

void
ASagaItemBox::OnOverlapBegin(UPrimitiveComponent* component, AActor* other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool sweep, const FHitResult& SweepResult)
{
	ISagaCharacterItemInterface* pawn = Cast<ISagaCharacterItemInterface>(other);

	if (pawn)
	{
		if (not isGrabbed)
		{
			const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

			if (net->IsOfflineMode())
			{
				UE_LOG(LogSagaGame, Log, TEXT("[AMapObstacle1] Item spawner %d is destroyed (Offline Mode)."), myItemId);

				pawn->TakeItem(static_cast<ESagaItemTypes>(FMath::RandRange(0, 2)));
			}
			else
			{
				UE_LOG(LogSagaGame, Log, TEXT("[AMapObstacle1] Item spawner %d is destroyed."), myItemId);

				net->SendRpcPacket(ESagaRpcProtocol::RPC_GRAB_ITEM, 0, myItemId);
			}

			if (itemGrabEffect)
			{
				FVector SpawnLocation = GetActorLocation();
				FRotator SpawnRotation = GetActorRotation();

				UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, itemGrabEffect, SpawnLocation, SpawnRotation);

				if (NiagaraComponent)
				{
					FTimerHandle NiagaraTimerHandle;
					GetWorldTimerManager().SetTimer(NiagaraTimerHandle, NiagaraComponent, &UNiagaraComponent::Deactivate, 3.0f, false);
				}
			}

			Mesh->SetHiddenInGame(true);
			SetActorEnableCollision(false);

			isGrabbed = true;
		}
	}
	else
	{
		//Destroy();
	}
}

void
ASagaItemBox::OnEffectFinished(UParticleSystemComponent* ParticleSystem)
{
	//Destroy();
}
