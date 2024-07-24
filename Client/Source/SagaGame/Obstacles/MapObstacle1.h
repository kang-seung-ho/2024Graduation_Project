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

	/* 아이템의 고유 번호

	*  장애물이 월드에 스폰할 당시에는 이 숫자만 가지고 있음
	*  어짜피 모든 클라이언트는 맵이 똑같으니까 배치된 장애물도 순서가 같고 숫자도 같을 것임
	*  서버, 클라 모두 같게 동기화됨
	*/
	UPROPERTY()
	int32 myItemId{ -1 };

	AMapObstacle1();

	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float dmg, struct FDamageEvent const& event, AController* instigator, AActor* causer) override;

	UFUNCTION(Category = "CandyLandSaga|Game|Item")
	void OnDestroy();

	UFUNCTION(Category = "CandyLandSaga|Game|Item")
	ASagaItemBox* SpawnItemBox() const;

	UFUNCTION(Category = "CandyLandSaga|Game|Item")
	FORCEINLINE void SetID(int32 id) noexcept
	{
		myItemId = id;
	}
	
	UFUNCTION(Category = "CandyLandSaga|Game|Item")
	FORCEINLINE int32 GetID() const noexcept
	{
		return myItemId;
	}

protected:
	virtual void BeginPlay() override;
};
