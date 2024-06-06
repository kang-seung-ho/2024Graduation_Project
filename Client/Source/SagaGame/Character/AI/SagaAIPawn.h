#pragma once
#include "SagaGame.h"
#include <GameFramework/Pawn.h>
#include <Components/ArrowComponent.h>

#include "SagaAIMovementComponent.h"
#include "SagaAIPawn.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game|AI")
class SAGAGAME_API ASagaAIPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASagaAIPawn();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> mCapsule;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> mMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USagaAIMovementComponent> mMovement;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UArrowComponent> mArrowComponent;
#endif

public:
	UCapsuleComponent* GetCapsuleComponent() const
	{
		return mCapsule;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

};
