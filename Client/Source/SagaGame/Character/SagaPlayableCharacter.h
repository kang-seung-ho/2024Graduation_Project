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
	FTakeWeaponDelegateWrapper()
		: WeaponDelegate()
	{}

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

	constexpr EItemType GetWeaponType() const noexcept { return mWeaponType; }

	virtual void SwordAttack();

protected:
	virtual void EquipHammer(class USagaWeaponData* WeaponData);
	virtual void EquipLightSaber(class USagaWeaponData* WeaponData);
	virtual void EquipWaterGun(class USagaWeaponData* WeaponData);
	virtual void TakeItem(class USagaWeaponData* WeaponData) override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character", Meta = (AllowPrivateAccess = "true"))
	EItemType mWeaponType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	TObjectPtr<class UStaticMeshComponent> Weapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character")
	TArray<FTakeWeaponDelegateWrapper> TakeWeaponAction;
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
