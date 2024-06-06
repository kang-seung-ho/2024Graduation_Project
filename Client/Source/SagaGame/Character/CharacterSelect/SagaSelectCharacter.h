#pragma once
#include "SagaGame.h"
#include <UObject/ObjectPtr.h>
#include <GameFramework/Actor.h>

#include "Player/SagaPlayerWeaponTypes.h"
#include "SagaSelectCharacter.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Character Choice Level")
class SAGAGAME_API ASagaSelectCharacter final : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Character Choice Level")
	TObjectPtr<class UCapsuleComponent> mBody;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Character Choice Level")
	TObjectPtr<class USkeletalMeshComponent> mMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Character Choice Level")
	EPlayerWeapon mWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Character Choice Level")
	TSoftClassPtr<UAnimInstance> humanCharacterAnimation;

	ASagaSelectCharacter();

	UFUNCTION(BlueprintPure)
	EPlayerWeapon GetWeapon() const noexcept
	{
		return mWeapon;
	}

protected:
	virtual void PostInitializeComponents() override;
};
