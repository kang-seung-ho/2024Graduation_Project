#pragma once
#include "Saga/Level/SagaLevelScripter.h"

#include "SagaGameLevel.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game|Level", meta = (DisplayName = "Saga Game Level C++ Class"))
class SAGAFRAMEWORK_API ASagaGameLevel final : public ASagaLevelScripter
{
	GENERATED_BODY()

public:
	ASagaGameLevel() noexcept;

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

private:
	UFUNCTION()
	void OnDisconnected();
	UFUNCTION()
	void OnLeftRoomBySelf();
	UFUNCTION()
	void OnRpc(ESagaRpcProtocol cat, int32 id, int64 arg0, int32 arg1);

	UPROPERTY()
	TSubclassOf<class UUserWidget> mTeamScoreBoardClass;
	UPROPERTY()
	class UUserWidget* mTeamScoreBoard;
};
