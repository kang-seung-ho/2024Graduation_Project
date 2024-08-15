#pragma once
#include "SagaGame.h"
#include <Subsystems/GameInstanceSubsystem.h>

#include "SagaGameSubsystem.generated.h"

UCLASS(Category = "CandyLandSaga|Game")
class SAGAGAME_API USagaGameSubsystem final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USagaGameSubsystem();

	/* State Machines */
#pragma region =========================
	virtual void Initialize(class FSubsystemCollectionBase& collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(class UObject* outer) const override;
#pragma endregion

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game", meta = (UnsafeDuringActorConstruction))
	static USagaGameSubsystem* GetSubSystem(const class UWorld* world);

	UFUNCTION()
	void AssignLocalPlayerSpawner(class AActor* spawner) noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game")
	void SetScore(ESagaPlayerTeam team, int32 score) noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game")
	void AddScore(ESagaPlayerTeam team, int32 score) noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game")
	void SetWinner(const int32 winner_team_id) noexcept;
	
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game")
	bool TrySetWinner(const int32 winner_team_id) noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game")
	void SetWhoWonByPinata(int32 TeamPinataColor) noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game")
	void ClearWinner() noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Game")
	int32 GetWinnerId() const noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Game")
	const AActor* GetLocalPlayerSpawner() const noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Game")
	int32 GetRedTeamScore() const noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Game")
	int32 GetBlueTeamScore() const noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Game")
	FString GetWhoWon();

private:
	UPROPERTY()
	TObjectPtr<class AActor> localPlayerSpawner;

	UPROPERTY()
	int32 gameWinnerId;
	UPROPERTY()
	int32 RedTeamScore;
	UPROPERTY()
	int32 BluTeamScore;
	UPROPERTY()
	bool RedTeamPinataBroken;
	UPROPERTY()
	bool BluTeamPinataBroken;
	UPROPERTY()
	ESagaPlayerTeam lastLocalPlayerTeam;
};
