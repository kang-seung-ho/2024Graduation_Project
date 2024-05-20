#pragma once
#include "Saga/Interface/SagaLiveUserWidget.h"

#include "SagaLobbyLevelUiWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Lobby Level UI Widget C++ Class"))
class SAGAFRAMEWORK_API USagaLobbyLevelUiWidget : public USagaLiveUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<class UListView> myListView;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|UI|Content")
	TObjectPtr<class USagaUiInstancedRoomData> selectedRoom;

	USagaLobbyLevelUiWidget(const FObjectInitializer& initializer) noexcept;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& geometry, float delta_time) override;

	UFUNCTION()
	void HandleQuitButtonClick();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	void HandleCreatingRoomClick();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	void HandleTryJoinRoom();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	void HandleRefreshRoomList();

	UPROPERTY()
	TObjectPtr<class USagaUiButton> myCreateButton;
	UPROPERTY()
	TObjectPtr<class USagaUiButton> myJoinButton;
	UPROPERTY()
	TObjectPtr<class USagaUiButton> myRefreshButton;
	UPROPERTY()
	TObjectPtr<class USagaUiButton> myQuitButton;
};
