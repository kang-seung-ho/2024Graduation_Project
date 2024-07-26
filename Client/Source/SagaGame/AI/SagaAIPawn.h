#pragma once
#include "SagaGame.h"
#include "SagaAIInfo.h"

#include <GameFramework/Pawn.h>
#include <Components/ArrowComponent.h>

#include "SagaAIMovementComponent.h"
#include "SagaAIPawn.generated.h"

DECLARE_MULTICAST_DELEGATE(FAIDeathDelegate);

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

	TObjectPtr<UAnimInstance> mAnimInst;

	FAIDeathDelegate mDeathDelegate;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UArrowComponent> mArrowComponent;
#endif

public:
	UFUNCTION()
	UCapsuleComponent* GetCapsuleComponent() const noexcept
	{
		return mCapsule;
	}

	USagaAIMovementComponent* GetMovementComponent() const noexcept override
	{
		return mMovement;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:
	template<typename T>
	void AddDeathDelegate(T* Obj, void(T::* Func)())
	{
		mDeathDelegate.AddUObject(Obj, Func);
	}
};
