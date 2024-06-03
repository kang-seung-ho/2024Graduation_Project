#pragma once
#include "SagaGame.h"
#include <UObject/ObjectPtr.h>
#include <GameFramework/GameModeBase.h>
#include <GameFramework/Actor.h>

#include "Player/SagaUserTeam.h"
#include "Player/SagaPlayerWeaponTypes.h"
#include "Character/SagaCharacterSpawner.h"

#include "Saga/Network/SagaRpcProtocol.h"
#include "SagaInGameMode.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|System|Game Mode")
class SAGAGAME_API ASagaInGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<TObjectPtr<class ASagaCharacterSpawner>> playerSpawners;
	UPROPERTY()
	TObjectPtr<class ASagaInGamePlayerController> localPlayerController;

	ASagaInGameMode();

	UFUNCTION()
	class ASagaCharacterSpawner* GetSpawnerBy(EUserTeam team) const;
	UFUNCTION()
	class ASagaCharacterSpawner* GetSpawner(const FString& team_name) const;
	
	virtual void InitGame(const FString& mapname, const FString& optios, FString& err_msg) override;
	virtual void StartPlay() override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* player) override;
	virtual AActor* FindPlayerStart_Implementation(AController* player, const FString& tag) override;

private:
	UFUNCTION()
	void OnLeftRoom(int32 user_id);
	UFUNCTION()
	void OnGameStarted();
	UFUNCTION()
	void OnCreatingCharacter(int32 user_id, EUserTeam team, EPlayerWeapon weapon);
	UFUNCTION()
	void OnRpc(ESagaRpcProtocol cat, int32 id, int64 arg0, int32 arg1);

	UPROPERTY()
	FTimerHandle readyTimerHandle;
};

extern const FString SagaRedTeamName;
extern const FString SagaBluTeamName;
