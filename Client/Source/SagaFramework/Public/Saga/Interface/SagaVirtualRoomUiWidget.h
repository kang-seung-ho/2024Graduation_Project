#pragma once
#include "Saga/Interface/SagaLiveUserWidget.h"
#include <Blueprint/IUserObjectListEntry.h>

#include "SagaVirtualRoomUiWidget.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Instanced Room Data UI Representer C++ Class"))
class SAGAFRAMEWORK_API USagaVirtualRoomUiWidget final : public USagaLiveUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|UI")
	TObjectPtr<class USagaUiInstancedRoomData> myDataHolder;

	USagaVirtualRoomUiWidget(const FObjectInitializer& initializer) noexcept;

protected:
	virtual void NativeOnListItemObjectSet(UObject* data) override;
	virtual void NativeOnItemSelectionChanged(bool is_selected) override;
	virtual void NativeOnEntryReleased() override;

private:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI")
	FText GetTitleLabelText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI")
	FText GetMemberCountLabelText() const;
};
