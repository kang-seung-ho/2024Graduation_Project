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
	// for character
	static inline constexpr float normalWalkSpeed = 400;
	// for character
	static inline constexpr float normalRunSpeed = 600;
	// for guardian
	static inline constexpr float riderWalkSpeed = 250;
	// for guardian
	static inline constexpr float riderRunSpeed = 400;

	bool isForwardWalking;
	bool isStrafeWalking;
	bool isRunning;

	FVector walkDirection;
	FVector preferedDirection;
	float mMoveDir;

	/*UPROPERTY(VisibleAnywhere)
	UCameraComponent* mCamera;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* mArm;*/

public:
	ASagaInGamePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* RPC Methods */

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
	void TerminateAttack() {}

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void BeginRide();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void EndRide();
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void ExecuteRide() {}
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void TerminateRide() {}

	/* Events */

	void OnAttack(const FInputActionValue& Value);

	/* Getters */

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character")
	float GetMoveDir() const noexcept
	{
		return mMoveDir;
	}

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character")
	double GetNormalizedMoveDir() const noexcept;

	/* Flag Methods */

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Game")
	bool IsLocalPlayer() const noexcept;

	/* Unreal Engine Methods */

	virtual void Tick(float delta_time) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void SetupInputComponent();
};
