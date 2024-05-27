#pragma once
#include "SagaGameInfo.h"
#include <GameFramework/Actor.h>

#include "SagaDestructibleMapObstacle.generated.h"

UENUM(BlueprintType)
enum class EObstacleTeam : uint8
{
	Red,
	Blue
};

UCLASS()
class SAGAGAME_API ASagaDestructibleMapObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASagaDestructibleMapObstacle();
	float health = 100.0f;

	FTimerHandle ChangeLevelTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle")
	int32 WhichTeam; // 0: Red, 1: Blue

	void OpenEndLevel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TSoftClassPtr<UAnimInstance> ObjectAnimation;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components)
	USkeletalMeshComponent* MeshComponent;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//USkeletalMeshComponent* SKMeshComponent;

	UPROPERTY()
	UAnimSequence* DeathAnimation;
	UAnimSequence* HitAnimation;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


};
