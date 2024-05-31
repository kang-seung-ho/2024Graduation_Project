#pragma once
#include "Saga/Interface/SagaLiveUserWidget.h"

#include "SagaRoomMemberViewerUiWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Room Member Viewer UI Widget C++ Class"))
class SAGAFRAMEWORK_API USagaRoomMemberViewerUiWidget : public USagaLiveUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<class UListView> myListView;

	UPROPERTY()
	TObjectPtr<class USagaUiButton> myJoinButton;

	USagaRoomMemberViewerUiWidget(const FObjectInitializer& initializer) noexcept;

protected:
	virtual void NativeConstruct() override;
};
