#pragma once
#include "SagaGame.h"
#include "SagaCharacterPlayer.h"

#include "../Item/SagaWeaponData.h"
#include "../Interface/SagaCharacterItemInterface.h"
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

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	constexpr EPlayerWeapon GetWeaponType() const noexcept { return mWeaponType; }

	virtual void Attack();

protected:
	EItemType mItemType;

protected:
	virtual void Acquire_Drink(class USagaWeaponData* WeaponData);
	virtual void Acquire_Gum(class USagaWeaponData* WeaponData);
	virtual void Acquire_smokebomb(class USagaWeaponData* WeaponData);
	virtual void TakeItem(class USagaWeaponData* WeaponData) override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	TObjectPtr<class UStaticMeshComponent> Weapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character")
	TArray<FTakeItemDelegateWrapper> TakeItemAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character");
	bool isRiding = false;

//public:
//	UFUNCTION(BlueprintNativeEvent, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
//	virtual void ExecuteRide() override;
//	UFUNCTION(BlueprintNativeEvent, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
//	virtual void TerminateRide() override;
//
//	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
//	virtual void ExecuteAttack() override;
//	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
//	virtual void TerminateAttack() override;
//
//	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
//	virtual void ExecuteRotation() override;
//	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
//	virtual void TerminateRotation() override;
//
//	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
//	virtual void ExecuteJump() override;
//	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
//	virtual void TerminateJump() override;
//
//	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
//	virtual void ExecuteRun() override;
//	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
//	virtual void TerminateRun() override;
//
//	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
//	virtual void ExecuteWalk(const float& delta_time) override;
//	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
//	virtual void TerminateWalk(const float& delta_time) override;
};
