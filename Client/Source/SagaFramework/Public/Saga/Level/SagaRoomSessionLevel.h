#pragma once
#include "Saga/Level/SagaLevelScripter.h"
#include <Containers/Array.h>

#include "Saga/Network/SagaGameContract.h"

#include "SagaRoomSessionLevel.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game|Level", meta = (DisplayName = "Saga Room Session Level C++ Class"))
class SAGAFRAMEWORK_API ASagaRoomSessionLevel final : public ASagaLevelScripter
{
	GENERATED_BODY()

public:
	ASagaRoomSessionLevel() noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Level|Room Session")
	void UpdateRoomSessionUI();

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	UFUNCTION()
	void OnDisconnected();
	UFUNCTION()
	void OnJoinedRoom(int32 user_id);
	UFUNCTION()
	void OnLeftRoomBySelf();
	UFUNCTION()
	void OnLeftRoom(int32 user_id);
	UFUNCTION()
	void OnUpdateUserList(UPARAM(ref) const TArray<struct FSagaVirtualUser>& list);
	UFUNCTION()
	void OnTeamChanged(int32 user_id, bool is_red_team);
	UFUNCTION()
	void OnFailedToStartGame(ESagaGameContract reason);
	UFUNCTION()
	void OnGetPreparedGame();

	UFUNCTION()
	void HandleOnClose();
	UFUNCTION()
	void HandleStartButton();
	UFUNCTION()
	void HandleQuitButton();
	UFUNCTION()
	void HandleRedTeamButton();
	UFUNCTION()
	void HandleBlueTeamButton();
	UFUNCTION()
	void HandlePeriodicUpdate();
	UFUNCTION()
	void HandleDestroy(AActor* actor);

	UFUNCTION()
	void Fallback();
	UFUNCTION()
	void PauseTimer();
	UFUNCTION()
	void UnPauseTimer();
	UFUNCTION()
	void AddMemberToTeamViewer(class UListView* listview, const struct FSagaVirtualUser& user);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class USagaRoomSessionLevelUiWidget> levelUiClass;
	UPROPERTY()
	TObjectPtr<class USagaRoomSessionLevelUiWidget> levelUiInstance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class USagaUiPopup> errorPopupWidgetClass;
	UPROPERTY()
	TObjectPtr<class USagaUiPopup> errorPopupWidgetInstance;
	UPROPERTY()
	TSubclassOf<class USagaUiInstancedUserData> userDataClass;

	UPROPERTY()
	FTimerHandle teamUpdateTimer;
};
