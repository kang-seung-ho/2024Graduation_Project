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
	void RideNPC();

public:
	//UFUNCTION(BlueprintImplementableEvent, Category = "Blueprint")
	//void RidingFunction();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	constexpr EPlayerWeapon GetWeaponType() const noexcept { return myWeaponType; }

	virtual void Attack();

protected:
	virtual void PostInitializeComponents() override;

protected:
	EItemType mItemType;
	float myClientHP = 100.0f;
private:
	FTimerHandle RespawnTimerHandle;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UAnimInstance> humanCharacterAnimation;

	// For saving Weapon Meshes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TMap<EPlayerWeapon, UStaticMesh*> WeaponMeshes;

protected:
	virtual void Acquire_Drink(class USagaWeaponData* WeaponData);
	virtual void Acquire_Gum(class USagaWeaponData* WeaponData);
	virtual void Acquire_smokebomb(class USagaWeaponData* WeaponData);
	virtual void TakeItem(class USagaWeaponData* WeaponData) override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);


	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	TObjectPtr<class UStaticMeshComponent> MyWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character")
	TArray<FTakeItemDelegateWrapper> TakeItemAction;

public:
	// ������ ó���� ���� �������̵�
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void RespawnCharacter();

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
