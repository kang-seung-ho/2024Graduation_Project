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

	/* Methods */
#pragma region =========================
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game", meta = (NotBlueprintThreadSafe, UnsafeDuringActorConstruction))
	AActor* CreatePlayableCharacter(UClass* type, const FTransform& transform) const;
	UFUNCTION()
	void TriggerRideNPC(const FInputActionValue& Value);
	UFUNCTION()
	void RideNPCCallFunction();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	bool SendRpc(const ESagaRpcProtocol& rpc, const int64 arg0 = 0, const int32 arg1 = 0) const;
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
	UFUNCTION()
	void OnCreatingCharacter(int32 user_id, EUserTeam team, UClass* character_class);
#pragma endregion

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
	UFUNCTION(Category = "CandyLandSaga|Game|Character", meta = (CallableWithoutWorldContext, NotBlueprintThreadSafe))
	void EndRotate(const FInputActionValue& input);

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void BeginAttack();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void EndAttack();

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void BeginRide();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void EndRide();
#pragma endregion

	/* Getters */
#pragma region =========================
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character")
	float GetMoveDir() const noexcept
	{
		return mMoveDir;
	}
#pragma endregion

	/*UPROPERTY(VisibleAnywhere)
	UCameraComponent* mCamera;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* mArm;*/

	UPROPERTY()
	FOnRideNPCDelegate OnRideNPC;
	UPROPERTY()
	TSubclassOf<UUserWidget> mTeamScoreBoardClass;
	UPROPERTY()
	class UUserWidget* mTeamScoreBoard;

protected:
	/* State Machines */
#pragma region =========================
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override;
#pragma endregion

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	bool wasMoved;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	bool wasTilted;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	bool isRiding;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	FVector walkDirection;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	float mMoveDir;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	FTimerHandle tranformUpdateTimer;

};
