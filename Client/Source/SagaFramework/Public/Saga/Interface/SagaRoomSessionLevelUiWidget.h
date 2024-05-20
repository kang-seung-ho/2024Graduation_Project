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
	virtual void NativeOnInitialized() override;
};
