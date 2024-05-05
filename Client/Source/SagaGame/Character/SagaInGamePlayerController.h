#pragma once
#include "SagaGame.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"

#include "../SagaGameInfo.h"
#include "SagaInGamePlayerController.generated.h"

UCLASS(Blueprintable, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaInGamePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	bool isForwardWalking;
	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	bool isStrafeWalking;
	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	bool isRunning;
	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	bool isRiding;
	UPROPERTY()
	bool wasMoved; // 이동했는지 여부
	UPROPERTY()
	bool wasTilted; // 회전했는지 여부

	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	FVector walkDirection;
	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	FVector preferedDirection;
	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character")
	float mMoveDir;
	UPROPERTY(VisibleAnywhere, AdvancedDisplay, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	FTimerHandle tranformUpdateTimer;

	/*UPROPERTY(VisibleAnywhere)
	UCameraComponent* mCamera;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* mArm;*/

public:
	ASagaInGamePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnAttack(const FInputActionValue& Value);
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnGameStarted();
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnUpdateTransform();

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void BeginForwardWalk(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void EndForwardWalk(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void BeginStrafeWalk(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void EndStrafeWalk(const FInputActionValue& Value);

	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void UpdateMovement(const float& delta_time);
	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void ExecuteWalk(const float& delta_time);
	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void TerminateWalk(const float& delta_time);
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void BeginRun();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void EndRun();
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void ExecuteRun();
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void TerminateRun();

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void BeginJump();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void EndJump();
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void ExecuteJump();
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void TerminateJump();

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void BeginRotation();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void EndRotation();
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void ExecuteRotation(const FInputActionValue& Value);
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void TerminateRotation();

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void BeginAttack();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void EndAttack();
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void ExecuteAttack() {}
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void TerminateAttack() {};

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void BeginRide();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void EndRide();
	UFUNCTION(BlueprintNativeEvent, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void ExecuteRide();
	UFUNCTION(BlueprintNativeEvent, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void TerminateRide();

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character")
	float GetMoveDir() const noexcept
	{
		return mMoveDir;
	}
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character")
	double GetNormalizedMoveDir() const noexcept;

	virtual void Tick(float delta_time) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void SetupInputComponent();
};
