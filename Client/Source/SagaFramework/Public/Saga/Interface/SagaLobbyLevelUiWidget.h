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

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class USagaUiInstancedRoomData> selectedRoom;

	USagaLobbyLevelUiWidget(const FObjectInitializer& initializer) noexcept;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& geometry, float delta_time) override;

private:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Lobby Level")
	FText GetQuitButtonLabelText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Lobby Level")
	FText GetUserNicknameLabelText() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Lobby Level")
	FText GetUserIdLabelText() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Lobby Level")
	bool IsRoomButtonEnabled() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Lobby Level")
	bool IsJoinButtonEnabled() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Lobby Level")
	FText GetNotConnectedAlertLabelText() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Lobby Level")
	ESlateVisibility IsNotConnectedAlertLabelVisibility() const;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Lobby Level")
	void HandleQuitButtonClick();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Lobby Level")
	void HandleCreatingRoomClick();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Lobby Level")
	void HandleTryJoinRoom();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Lobby Level")
	void HandleRefreshRoomList();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Lobby Level")
	void HandleCreateRoom(FText title);

	UFUNCTION()
	FText AcquireUserNickname() const;
	UFUNCTION()
	int32 AcquireUserId() const;
	UFUNCTION()
	bool AcquireButtonsInteractableFlag() const;
	UFUNCTION()
	FText AcquireQuitButtonLabel() const;
	UFUNCTION()
	FText AcquireNotConnectedAlertLabelText() const;
	UFUNCTION()
	ESlateVisibility AcquireNotConnectedAlertLabelVisibility() const;

	UPROPERTY()
	TObjectPtr<class USagaUiButton> myCreateButton;
	UPROPERTY()
	TObjectPtr<class USagaUiButton> myJoinButton;
	UPROPERTY()
	TObjectPtr<class USagaUiButton> myRefreshButton;
	UPROPERTY()
	TObjectPtr<class USagaUiButton> myQuitButton;
};
