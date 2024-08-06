#pragma once
#include "SagaGameInfo.h"
#include <Templates/SubclassOf.h>
#include <GameFramework/Actor.h>

#include "SagaSpawnPoint.generated.h"

UENUM(BlueprintType)
enum class ESpawnType : uint8
{
	Normal,
	Loop
};

UCLASS()
class SAGAGAME_API ASagaSpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASagaSpawnPoint();

protected:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* mRoot;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<class UArrowComponent> mArrowComponent;
#endif

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> mSpawnClass;

	TObjectPtr<AActor> mSpawnActor;

	UPROPERTY(EditAnywhere)
	ESpawnType mSpawnType;

	UPROPERTY(EditAnywhere)
	float mSpawnTime;

	float mSpawnAccTime;

	UPROPERTY(EditAnywhere)
	int32 mCount = -1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void Spawn();
	void DeathCallback();

};
