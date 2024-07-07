#pragma once
#include "SagaGame.h"
#include <GameFramework/Actor.h>

#include "ObjectComponents/ObstacleHPComponent.h"
#include "MapObstacle1.generated.h"

UCLASS()
class SAGAGAME_API AMapObstacle1 : public AActor
{
	GENERATED_BODY()

public:
	// Static Mesh Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// HP Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UObstacleHPComponent* HPComponent;

	UPROPERTY(EditAnywhere, Category = "Obstacle")
	float myHealth;

	AMapObstacle1();

	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float dmg, struct FDamageEvent const& event, AController* instigator, AActor* causer) override;

	void SpawnItemBox();

	UFUNCTION(Category = "CandyLandSaga|Game|Item")
	FORCEINLINE int32 GetItemId() const noexcept
	{
		return myItemId;
	}

	friend class ASagaInGameMode;

protected:
	/* 아이템의 고유 번호

	*  서버, 클라 모두 같게 동기화됨
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Item", meta = (ClampMin = "0"))
	int32 myItemId;

	virtual void BeginPlay() override;

	UFUNCTION(Category = "CandyLandSaga|Game|Item")
	FORCEINLINE void SetItemId(int32 id) noexcept
	{
		myItemId = id;
	}
};
