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

	/// TODO: 서버에서 방마다에 생성할 아이템을 저장해놓은 ID-아이템 유형 테이블을 읽어서 아이템을 생성해야함
	void SpawnItemBox();

	UFUNCTION(Category = "CandyLandSaga|Game|Item")
	FORCEINLINE int32 GetItemId() const noexcept
	{
		return myItemId;
	}

	friend class ASagaInGameMode;

protected:
	/// TODO: 게임 레벨에서 모든 장애물에 ID 부여해주어야 함
	/* 아이템의 고유 번호

	*  장애물이 월드에 스폰할 당시에는 이 숫자만 가지고 있음
	*  어짜피 모든 클라이언트는 맵이 똑같으니까 배치된 장애물도 순서가 같고 숫자도 같을 것임
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
