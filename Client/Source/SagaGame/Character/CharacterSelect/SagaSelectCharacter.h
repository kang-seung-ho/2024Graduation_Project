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

protected:
	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* mBody;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* mMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlayerWeapon mWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UAnimInstance> humanCharacterAnimation;

public:
	EPlayerWeapon GetWeapon() const
	{
		return mWeapon; 
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
