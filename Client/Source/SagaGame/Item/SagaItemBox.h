#pragma once
#include "SagaGameInfo.h"
#include <Templates/SubclassOf.h>
#include <GameFramework/Actor.h>
#include <Engine/World.h>
#include <NiagaraFunctionLibrary.h>

#include "SagaItemBox.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Item")
class SAGAGAME_API ASagaItemBox : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Item")
	int32 myItemId{ -1 };

	ASagaItemBox();

	UFUNCTION(Category = "CandyLandSaga|Game|Item")
	void SetItemId(int32 id) noexcept;

	UFUNCTION(Category = "CandyLandSaga|Game|Item")
	FORCEINLINE UStaticMeshComponent* GetMesh() const noexcept
	{
		return Mesh;
	}

	UFUNCTION(Category = "CandyLandSaga|Game|Item")
	FORCEINLINE int32 GetItemId() const noexcept
	{
		return myItemId;
	}

protected:
	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Item")
	TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Item")
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Item")
	TObjectPtr<class UNiagaraSystem> itemGrabEffect;

	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Item")
	bool isGrabbed{};

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* component, class AActor* other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool sweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEffectFinished(class UParticleSystemComponent* ParticleSystem);

	virtual void PostInitializeComponents() override;
};
