#pragma once
#include "SagaGame.h"
#include "Misc/OutputDeviceNull.h"
#include "SagaCharacterPlayer.h"

#include "../Item/SagaWeaponData.h"
#include "../Interface/SagaCharacterItemInterface.h"
#include "Player/SagaPlayerWeaponTypes.h"
#include "CharacterSelect/SagaSelectCharacter.h"
#include "SagaPlayableCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FOnTakeItemDelegate, class USagaWeaponData*);

USTRUCT(BlueprintType)
struct FTakeItemDelegateWrapper
{
	GENERATED_BODY()

public:
	FTakeItemDelegateWrapper()
		: ItemDelegate()
	{}

	FTakeItemDelegateWrapper(const FOnTakeItemDelegate& InItemDelegate)
		: ItemDelegate(InItemDelegate)
	{}

	FOnTakeItemDelegate ItemDelegate;
};

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaPlayableCharacter : public ASagaCharacterPlayer, public ISagaCharacterItemInterface
{
	GENERATED_BODY()
	
public:
	ASagaPlayableCharacter();

	void RespawnCharacter();

	virtual void Attack();
	void RideNPC();
	//UFUNCTION(BlueprintImplementableEvent, Category = "Blueprint")
	//void RidingFunction();

	//virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual float ExecuteHurt(const float dmg) override;
	virtual void ExecuteDeath() override;

	constexpr EPlayerWeapon GetWeaponType() const noexcept { return myWeaponType; }

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	virtual void Acquire_Drink(class USagaWeaponData* WeaponData);
	virtual void Acquire_Gum(class USagaWeaponData* WeaponData);
	virtual void Acquire_smokebomb(class USagaWeaponData* WeaponData);
	virtual void TakeItem(class USagaWeaponData* WeaponData) override;

	EItemType mItemType;

	FTimerHandle RespawnTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UAnimInstance> humanCharacterAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character")
	TArray<FTakeItemDelegateWrapper> TakeItemAction;
};
