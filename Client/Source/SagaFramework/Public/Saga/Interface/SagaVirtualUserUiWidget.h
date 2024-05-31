#pragma once
#include "Saga/Interface/SagaLiveUserWidget.h"
#include <Blueprint/IUserObjectListEntry.h>

#include "SagaVirtualUserUiWidget.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Instanced User Data UI Representer C++ Class"))
class SAGAFRAMEWORK_API USagaVirtualUserUiWidget : public USagaLiveUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|UI")
	TObjectPtr<class USagaUiInstancedUserData> myDataHolder;

protected:
	virtual void NativeOnListItemObjectSet(UObject* data) override;
	virtual void NativeOnItemSelectionChanged(bool is_selected) override;
	virtual void NativeOnEntryReleased() override;

private:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI")
	FText GetNicknameLabelText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI")
	FText GetUserIdLabelText() const;
};
