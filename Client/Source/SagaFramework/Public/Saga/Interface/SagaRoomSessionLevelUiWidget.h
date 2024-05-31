#pragma once
#include "Saga/Interface/SagaLiveUserWidget.h"

#include "SagaRoomSessionLevelUiWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Room Session Level UI Widget C++ Class"))
class SAGAFRAMEWORK_API USagaRoomSessionLevelUiWidget : public USagaLiveUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<class USagaRoomMemberViewerUiWidget> myRedTeamMemberListView;

	UPROPERTY()
	TObjectPtr<class USagaRoomMemberViewerUiWidget> myBlueTeamMemberListView;

	UPROPERTY()
	TObjectPtr<class USagaUiButton> myStartButton;

	UPROPERTY()
	TObjectPtr<class USagaUiButton> myQuitButton;

	USagaRoomSessionLevelUiWidget(const FObjectInitializer& initializer) noexcept;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void OnUpdateMembers(const TArray<struct FSagaVirtualUser>& list);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Room Session Level")
	FText GetRoomTitleLabelText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Room Session Level")
	FText GetRoomMemberCountLabelText() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Room Session Level")
	FText GetRedTeamMemberCountLabelText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Room Session Level")
	FText GetBluTeamMemberCountLabelText() const;

	UFUNCTION()
	FText AcquireRoomTitle() const;
	UFUNCTION()
	int32 AcquireRoomMemberCount() const;
	UFUNCTION()
	int32 AcquireRedTeamMemberCount() const;
	UFUNCTION()
	int32 AcquireBluTeamMemberCount() const;

	UPROPERTY()
	int32 redTeamCount;

	UPROPERTY()
	int32 bluTeamCount;
};
