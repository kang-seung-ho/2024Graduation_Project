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
	bool SendRpc(ESagaRpcProtocol rpc, const int64 arg0 = 0, const int32 arg1 = 0) const;
#pragma endregion

	/* 게임 내 이벤트 (타이머, 입력 반응, 언리얼 이벤트, ...) */
#pragma region =========================
	UFUNCTION()
	void OnLevelReady();
	UFUNCTION()
	void OnGameStarted();
	UFUNCTION()
	void OnUpdateTransform();
	UFUNCTION()
	void CountDown();
#pragma endregion

	/* 패킷 이벤트 (USagaNetworkSubSystem에서 전달) */
#pragma region =========================
	UFUNCTION()
	void OnCreatingCharacter(int32 user_id, EUserTeam team, EPlayerWeapon weapon);
	UFUNCTION()
	void OnRpc(ESagaRpcProtocol cat, int32 id, int64 arg0, int32 arg1);
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

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void BeginAttack(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void EndAttack();

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void BeginRide();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	void EndRide();
#pragma endregion

	/* Getters */
#pragma region =========================
#pragma endregion

public:
	/* Game */
#pragma region =========================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Minutes = 5;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Seconds = 30;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	bool isRiding;
	UPROPERTY()
	FOnRideNPCDelegate OnRideNPC;

	UPROPERTY()
	TSubclassOf<UUserWidget> mTeamScoreBoardClass;
	UPROPERTY()
	class UUserWidget* mTeamScoreBoard;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	TMap<EUserTeam, class ASagaCharacterSpawner*> playerSpawners;
#pragma endregion

protected:
	/* State Machines */
#pragma region =========================
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override;
#pragma endregion

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	FVector walkDirection;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "CandyLandSaga|Game|Character")
	FVector lastCharacterPosition;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "CandyLandSaga|Game|Character")
	FRotator lastCharacterRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "CandyLandSaga|Game|Character", meta = (NotBlueprintThreadSafe))
	FTimerHandle tranformUpdateTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character")
	bool isAttacking;

private:
	// 좌표 쓰기
	UFUNCTION()
	static void SerializePosition(const FVector& vector, int64& arg0, int32& arg1);
	// 좌표 읽기
	UFUNCTION()
	static FVector DeserializePosition(const int64& arg0, const int32& arg1);
};
