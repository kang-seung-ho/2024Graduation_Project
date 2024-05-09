#pragma once
#include "../../SagaGameInfo.h"
#include "GameFramework/Actor.h"

#include "Player/SagaPlayerWeaponTypes.h"
#include "SagaSelectCharacter.generated.h"

UCLASS()
class SAGAGAME_API ASagaSelectCharacter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASagaSelectCharacter();

protected:
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* mBody;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* mMesh;

	UPROPERTY(EditAnywhere)
	EPlayerWeapon mWeapon = EPlayerWeapon::LightSabor;

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
