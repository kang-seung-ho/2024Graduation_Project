#pragma once
#include "SagaGame.h"
#include "SagaCharacterPlayer.h"

#include "../Item/SagaWeaponData.h"
#include "../Interface/SagaCharacterItemInterface.h"
#include "SagaPlayableCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FOnTakeWeaponDelegate, class USagaWeaponData*);

USTRUCT(BlueprintType)
struct FTakeWeaponDelegateWrapper
{
	GENERATED_BODY()

public:
	FTakeWeaponDelegateWrapper(const FOnTakeWeaponDelegate& InWeaponDelegate)
		: WeaponDelegate(InWeaponDelegate)
	{}

	FOnTakeWeaponDelegate WeaponDelegate;
};

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaPlayableCharacter : public ASagaCharacterPlayer, public ISagaCharacterItemInterface
{
	GENERATED_BODY()
	
public:
	ASagaPlayableCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	constexpr EWeaponType GetWeaponType() const noexcept { return mWeaponType; }

	virtual void SwordAttack();

protected:
	virtual void EquipHammer(class USagaWeaponData* WeaponData);
	virtual void EquipLightSaber(class USagaWeaponData* WeaponData);
	virtual void EquipWaterGun(class USagaWeaponData* WeaponData);
	virtual void TakeItem(class USagaWeaponData* WeaponData) override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character", Meta = (AllowPrivateAccess = "true"))
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	EWeaponType mWeaponType;
	TObjectPtr<class UStaticMeshComponent> Weapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character")
	TArray<FTakeWeaponDelegateWrapper> TakeWeaponAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character");
	bool isRiding = false;
};
