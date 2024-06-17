#pragma once
#include "SagaGame.h"
#include "SagaGameInfo.h"
#include <UObject/ObjectPtr.h>
#include <GameFramework/PlayerController.h>
#include <InputActionValue.h>

#include "SagaInGamePlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRideNPCDelegate);

UCLASS(BlueprintType, Category = "CandyLandSaga|Game|System")
class SAGAGAME_API ASagaInGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FOnRideNPCDelegate OnRideNPC;

	ASagaInGamePlayerController(const FObjectInitializer& initializer) noexcept;

	virtual void Tick(float delta_time) override;

	UFUNCTION()
	void TriggerRideNPC(const FInputActionValue& Value);
	UFUNCTION()
	void RideNPCCallFunction();

	/* Actions */
#pragma region =========================
	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void BeginForwardWalk(const FInputActionValue& input);
	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void MidForwardWalk(const FInputActionValue& input);
	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void EndForwardWalk(const FInputActionValue& input);

	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void BeginStrafeWalk(const FInputActionValue& input);
	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void MidStrafeWalk(const FInputActionValue& input);
	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void EndStrafeWalk(const FInputActionValue& input);

	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void BeginRun();
	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void EndRun();

	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void BeginJump();
	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void EndJump();

	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void BeginRotate(const FInputActionValue& input);

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void BeginAttack(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void EndAttack();

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void BeginRide();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void EndRide();
#pragma endregion

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY()
	FVector walkDirection;
	UPROPERTY()
	bool isAttacking;

public:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	UUserWidget* InventoryWidget;

	void ToggleInventory();

	bool bIsInventoryVisible = false;

	void UpdateInputMode();
	void SetInventoryVisibility(bool bVisible);
};
