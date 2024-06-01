#pragma once
#include "SagaGame.h"
#include <GameFramework/GameModeBase.h>
#include <GameFramework/Actor.h>
#include <Containers/Map.h>

#include "Player/SagaUserTeam.h"
#include "Player/SagaPlayerWeaponTypes.h"
#include "Character/SagaCharacterSpawner.h"
#include "SagaInGameMode.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|System|Game Mode")
class SAGAGAME_API ASagaInGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASagaInGameMode();

	UFUNCTION()
	class ASagaCharacterSpawner* GetSpawnerBy(EUserTeam team) const;
	UFUNCTION()
	class ASagaCharacterSpawner* GetSpawner(const FString& team_name) const;
	
	virtual void InitGame(const FString& mapname, const FString& optios, FString& err_msg) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* player) override;
	virtual AActor* FindPlayerStart_Implementation(AController* player, const FString& tag) override;

private:
	UFUNCTION()
	void OnCreatingCharacter(int32 user_id, EUserTeam team, EPlayerWeapon weapon);
};

extern const FString SagaRedTeamName;
extern const FString SagaBluTeamName;
