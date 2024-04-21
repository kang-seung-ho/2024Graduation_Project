#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/SagaAttackAnimationInterface.h"
#include "Interface/SagaCharacterItemInterface.h"
#include "Interface/SagaCharacterWidgetInterface.h"

#include "Saga/Network/SagaNetworkRpcView.h"
#include "SagaCharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSagaCharacter, Log, All);

DECLARE_DELEGATE_OneParam(FOnTakeItemDelegate, class USagaItemData*);

UENUM(BlueprintType)
enum class ECharacterControlType : uint8
{
	Shoulder,
	Quater
};

USTRUCT(BlueprintType)
struct FTakeItemDelegateWrapper
{
	GENERATED_BODY()

	FTakeItemDelegateWrapper() {};
	FTakeItemDelegateWrapper(const FOnTakeItemDelegate& InItemDelegate) : ItemDelegate(InItemDelegate) {};

	FOnTakeItemDelegate ItemDelegate;
};

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaCharacterBase
	: public ACharacter, public ISagaAttackAnimationInterface, public ISagaCharacterItemInterface, public ISagaCharacterWidgetInterface
{
	GENERATED_BODY()

public:
	ASagaCharacterBase();

	virtual void PostInitializeComponents() override;

protected:
	UFUNCTION(Category = "CandyLandSaga|Game|Character")
	virtual void SetCharacterControlData(const class USagaCharacterControlData* CharacterControlData); //캐릭터컨트롤데이터에셋을 입력으로 받음

	UPROPERTY(EditAnywhere, Category = "CandyLandSaga|Game|Character", Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class USagaCharacterControlData*> CharacterControlManager;

//Animation Mantage 관련
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character")
	TObjectPtr<class UAnimMontage> ComboActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USagaComboActionData> ComboActionData;

	UFUNCTION()
	void ProcessComboCommand();

	//몽타주 시작될때 호출하는 함수
	UFUNCTION(Category = "CandyLandSaga|Game|Character")
	void ComboActionBegin();

	//몽타주 모두 종료시 호출
	UFUNCTION(Category = "CandyLandSaga|Game|Character")
	void ComboActionEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);

	//타이머 발동
	UFUNCTION(Category = "CandyLandSaga|Game|Character")
	void SetComboCheckTimer();

	UFUNCTION(Category = "CandyLandSaga|Game|Character")
	void ComboCheck();

	//현재 콤보가 어디까지 진행되었는지 저장을 위함
	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	int32 CurrentCombo = 0;

	FTimerHandle ComboTimerHandle;
	bool HasNextComboCommand = false;

	//공격 hit 부분
protected:
	virtual void AttackHitCheck() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DeadMontage;

	UFUNCTION(Category = "CandyLandSaga|Game|Character")
	virtual void SetDead();
	UFUNCTION(Category = "CandyLandSaga|Game|Character")
	void PlayDeadAnimation();

	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	float DeadEventDelayTime = 5.0f;

protected:
	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	float WalkSpeed = 500.f;
	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	float SprintSpeed = 1000.0f;

	//아이템관련
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Weapon; //생성자에서도 생성해준다.

	UPROPERTY()
	TArray<FTakeItemDelegateWrapper> TakeItemActions; //함수들이 바인딩 된다.

	UFUNCTION(Category = "CandyLandSaga|Game|Character")
	virtual void TakeItem(class USagaItemData* InItemData) override;
	UFUNCTION(Category = "CandyLandSaga|Game|Character")
	virtual void DrinkEnergyDrink(class USagaItemData* InItemData);
	UFUNCTION(Category = "CandyLandSaga|Game|Character")
	virtual void EquipWeapons(class USagaItemData* InItemData);
	UFUNCTION(Category = "CandyLandSaga|Game|Character")
	virtual void InstallGumball(class USagaItemData* InItemData);

	//스탯부분
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USagaCharacterStatComponent> Stat;

	//UI위젯부분
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> HpBar;

	virtual void SetupCharacterWidget(class USagaUserWidget* InUserWidget) override;
};
