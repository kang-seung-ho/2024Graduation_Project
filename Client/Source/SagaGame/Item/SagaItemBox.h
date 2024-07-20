#pragma once
#include "SagaGameInfo.h"
#include <Templates/SubclassOf.h>
#include <GameFramework/Actor.h>
#include <Engine/World.h>

#include "SagaItemBox.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Item")
class SAGAGAME_API ASagaItemBox : public AActor
{
	GENERATED_BODY()

public:
	ASagaItemBox();

	UFUNCTION(Category = "CandyLandSaga|Game|Item")
	void SetItemId(int32 id) noexcept;

	UFUNCTION(Category = "CandyLandSaga|Game|Item")
	FORCEINLINE UStaticMeshComponent* GetMesh() const noexcept
	{
		return Mesh;
	}

protected:
	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Item")
	int32 myItemId;

	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Item")
	TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Item")
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Item")
	TObjectPtr<class UParticleSystemComponent> Effect;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* component, class AActor* other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool sweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEffectFinished(class UParticleSystemComponent* ParticleSystem);
};
