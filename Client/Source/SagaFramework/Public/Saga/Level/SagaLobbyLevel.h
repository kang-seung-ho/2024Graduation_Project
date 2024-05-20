#pragma once
#include "Saga/Level/SagaLevelScripter.h"

#include "SagaLobbyLevel.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game|Level")
class SAGAFRAMEWORK_API ASagaLobbyLevel final : public ASagaLevelScripter
{
	GENERATED_BODY()

public:
	ASagaLobbyLevel() noexcept;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleOnClose();

	UFUNCTION()
	void OnDisconnected();
	UFUNCTION()
	void OnRoomCreated(int32 room_id);
	UFUNCTION()
	void OnJoinedRoom(int32 room_id);
	UFUNCTION()
	void OnUpdateRoomList(UPARAM(ref) const TArray<struct FSagaVirtualRoom>& list);

	UPROPERTY()
	TSubclassOf<class USagaLobbyLevelUiWidget> levelUiClass;
	UPROPERTY()
	TObjectPtr<class USagaLobbyLevelUiWidget> levelUiInstance;
	UPROPERTY()
	TSubclassOf<class USagaUiInstancedRoomData> roomDataClass;
};
