#pragma once
#include "SagaGameInfo.h"
#include <Templates/SubclassOf.h>
#include <GameFramework/Actor.h>
#include <Engine/World.h>

#include "SagaWeaponData.h"
#include "SagaItemBox.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Item")
class SAGAGAME_API ASagaItemBox : public AActor
{
	GENERATED_BODY()

public:
	//prevent null var
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Item")
	EItemType ItemType = EItemType::Drink;

	ASagaItemBox();

	UFUNCTION(Category = "CandyLandSaga|Game|Item")
	FORCEINLINE UStaticMeshComponent* GetMesh() const noexcept { return Mesh; }
	
	template<typename T>
	[[nodiscard]]
	static T* CreateItemBox(int32 id, UWorld* world, TSubclassOf<ASagaItemBox> uclass, const FTransform* transform, const FActorSpawnParameters& params = {})
	{
		const auto actor = world->SpawnActor(uclass, transform, params);
		const auto as_box = static_cast<ASagaItemBox*>(actor);

		//as_box->myId = id;

		return Cast<T>(actor);
	}

protected:
	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Item")
	TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Item")
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Item")
	TObjectPtr<class UParticleSystemComponent> Effect;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEffectFinished(class UParticleSystemComponent* ParticleSystem);

	void SetRandomItemType();
};
