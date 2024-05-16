#pragma once
#include "SagaGame.h"
#include <GameFramework/Actor.h>

#include "Player/SagaPlayerWeaponTypes.h"
#include "SagaSelectCharacter.generated.h"

UCLASS()
class SAGAGAME_API ASagaSelectCharacter : public AActor
{
	GENERATED_BODY()
	
public:	
	ASagaSelectCharacter();

	UFUNCTION()
	EPlayerWeapon GetWeapon() const noexcept
	{
		return mWeapon; 
	}

protected:
	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* mBody;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* mMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlayerWeapon mWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UAnimInstance> humanCharacterAnimation;
};
