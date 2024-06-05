#pragma once
#include "SagaGame.h"
#include <UObject/ObjectPtr.h>
#include <GameFramework/Actor.h>

/* Framework */
#include "Saga/GameModes/SagaGameModeBase.h"

/* Game */
#include "Player/SagaPlayerTeam.h"
#include "Player/SagaPlayerWeaponTypes.h"
#include "Character/SagaCharacterSpawner.h"

/* Network */
#include "Saga/Network/SagaRpcProtocol.h"
#include "SagaInGameMode.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|System|Game Mode")
class SAGAGAME_API ASagaInGameMode : public ASagaGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<TObjectPtr<class ASagaCharacterSpawner>> playerSpawners;
	UPROPERTY()
	TObjectPtr<class ASagaInGamePlayerController> localPlayerController;

	ASagaInGameMode(const FObjectInitializer& initializer);

	UFUNCTION()
	class AActor* GetSpawnerBy(ESagaPlayerTeam team) const;
	UFUNCTION()
	class AActor* GetSpawner(const FString& team_name) const;
	
	virtual void InitGame(const FString& mapname, const FString& optios, FString& err_msg) override;
	virtual void StartPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type reason) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* player) override;
	virtual AActor* FindPlayerStart_Implementation(AController* player, const FString& tag) override;

private:
	UFUNCTION()
	void OnLeftRoom(int32 user_id);
	UFUNCTION()
	void OnGameStarted();
	UFUNCTION()
	void OnCreatingCharacter(int32 user_id, ESagaPlayerTeam team, EPlayerWeapon weapon);
	UFUNCTION()
	void OnRpc(ESagaRpcProtocol cat, int32 id, int64 arg0, int32 arg1);

	UFUNCTION()
	void HandleUpdateTransform();

	UPROPERTY()
	FTimerHandle readyTimerHandle;

	UPROPERTY()
	FTimerHandle transformUpdateTimer;
	UPROPERTY()
	FVector lastCharacterPosition;
	UPROPERTY()
	FRotator lastCharacterRotation;
};

extern const FString SagaRedTeamName;
extern const FString SagaBluTeamName;

void SerializePosition(const FVector& vector, int64& arg0, int32& arg1);
FVector DeserializePosition(const int64& arg0, const int32& arg1);
