#pragma once
#include "SagaGame.h"
#include <GameFramework/PlayerController.h>
#include <InputActionValue.h>

#include "Item/SagaWeaponData.h"
#include "SagaInGamePlayerController.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game|System")
class SAGAGAME_API ASagaInGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASagaInGamePlayerController(const FObjectInitializer& initializer) noexcept;

	virtual void Tick(float delta_time) override;
	virtual void OnPossess(class APawn* pawn) override;

	UFUNCTION()
	void UpdateInputMode();
	//UFUNCTION()
	//void SetInventoryVisibility(bool flag);
	UFUNCTION()
	void AddItemToInventory(ESagaItemTypes ItemType);
	//UFUNCTION()
	//ESlateVisibility GetInventoryVisibility() const;
	//UFUNCTION()
	//bool IsInventoryVisible() const;

	/* Actions */
#pragma region =========================
	UFUNCTION()
	void BeginForwardWalk(const FInputActionValue& input);
	UFUNCTION()
	void MidForwardWalk(const FInputActionValue& input);
	UFUNCTION()
	void EndForwardWalk(const FInputActionValue& input);

	UFUNCTION()
	void BeginStrafeWalk(const FInputActionValue& input);
	UFUNCTION()
	void MidStrafeWalk(const FInputActionValue& input);
	UFUNCTION()
	void EndStrafeWalk(const FInputActionValue& input);

	UFUNCTION()
	void BeginRun();
	UFUNCTION()
	void EndRun();

	UFUNCTION()
	void BeginJump();
	UFUNCTION()
	void EndJump();

	UFUNCTION()
	void BeginRotate(const FInputActionValue& input);

	UFUNCTION()
	void BeginAttack(const FInputActionValue& Value);
	UFUNCTION()
	void EndAttack();

	/* 수호자 탑승 혹은 하차 */
	UFUNCTION()
	void BeginGuardianAction();

	UFUNCTION()
	void OnSkill1(const FInputActionValue& Value);

	UFUNCTION()
	void OnSkill2(const FInputActionValue& Value);

	UFUNCTION()
	void OnSkill3(const FInputActionValue& Value);

	UFUNCTION()
	void OnSkill4(const FInputActionValue& Value);

	UFUNCTION()
	void OnItem1(const FInputActionValue& Value);

	UFUNCTION()
	void OnItem2(const FInputActionValue& Value);

	UFUNCTION()
	void OnItem3(const FInputActionValue& Value);

#pragma endregion

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY()
	FVector walkDirection;
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> InventoryWidgetClass;
	UPROPERTY()
	TObjectPtr<class USagaInventoryWidget> InventoryWidget;
};
