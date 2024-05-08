#pragma once
#include "SagaGame.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"

#include "../SagaGameInfo.h"
#include "SagaInGamePlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRideNPCDelegate);


UCLASS(Blueprintable, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaInGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASagaInGamePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float delta_time) override;

	/* Getters */
#pragma region =========================
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character")
	float GetMoveDir() const noexcept
	{
		return mMoveDir;
	}
#pragma endregion

	UPROPERTY()
	FOnRideNPCDelegate OnRideNPC;

protected:
	TSubclassOf<UUserWidget> mTeamScoreBoardClass;
	class UUserWidget* mTeamScoreBoard;

protected:
	/* State Machines */
#pragma region =========================
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override;
#pragma endregion

	/* Methods */
#pragma region =========================
	UFUNCTION()
	void TriggerRideNPC(const FInputActionValue& Value);
	UFUNCTION()
	void RideNPCCallFunction();
#pragma endregion

	/* Events */
#pragma region =========================
	UFUNCTION()
	void OnAttack(const FInputActionValue& Value);
	UFUNCTION()
	void OnGameStarted();
	UFUNCTION()
	void OnUpdateTransform();
	UFUNCTION()
	void OnRpc(ESagaRpcProtocol cat, int32 id, int64 arg0, int32 arg1);
#pragma endregion

	/* Actions */
#pragma region =========================
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void BeginForwardWalk(const FInputActionValue& Value);
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void EndForwardWalk(const FInputActionValue& Value);

	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void BeginStrafeWalk(const FInputActionValue& Value);
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void EndStrafeWalk(const FInputActionValue& Value);

	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void BeginRun();
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void EndRun();

	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void BeginJump();
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void EndJump();

	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void BeginRotate(const FInputActionValue& input);
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void EndRotate(const FInputActionValue& input);

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void BeginAttack();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void EndAttack();

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void BeginRide();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	void EndRide();
#pragma endregion

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	bool SendRpc(const ESagaRpcProtocol& rpc, const int64 arg0 = 0, const int32 arg1 = 0) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	bool wasMoved;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	bool wasTilted;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	bool isRiding;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	FVector walkDirection;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	FTimerHandle tranformUpdateTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	float mMoveDir;

	/*UPROPERTY(VisibleAnywhere)
	UCameraComponent* mCamera;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* mArm;*/
};
