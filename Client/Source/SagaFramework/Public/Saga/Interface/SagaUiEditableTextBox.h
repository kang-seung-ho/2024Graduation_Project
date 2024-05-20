#pragma once
#include "Saga/Interface/SagaUserWidget.h"
#include <Delegates/DelegateCombinations.h>
#include <Components/Widget.h>
#include <Components/TextWidgetTypes.h>
#include <Components/EditableTextBox.h>
#include <Widgets/SWidget.h>
#include <Widgets/Text/ISlateEditableTextWidget.h>
#include <Styling/SlateTypes.h>
#include <Serialization/Archive.h>

#include "SagaUiEditableTextBox.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaOnEditableInputChangedEvent, const FText&, text);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSagaOnEditableInputCommittedEvent, const FText&, text, ETextCommit::Type, method);

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Text Input Box"))
class SAGAFRAMEWORK_API USagaUiEditableTextBox : public USagaUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI|Content")
	TObjectPtr<UEditableTextBox> myEditableTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = SetHintText, BlueprintGetter = GetHintText, Category = "CandyLandSaga|UI|Content")
	FText hintText;

	UPROPERTY()
	FGetText hintTextDelegate;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI|Event")
	FSagaOnEditableInputChangedEvent OnTextChanged;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI|Event")
	FSagaOnEditableInputCommittedEvent OnTextCommitted;

	USagaUiEditableTextBox(const FObjectInitializer& initializer);

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "Set Text (Saga Text Input Box)"))
	void SetText(FText text) noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "Get Text (Saga Text Input Box)"))
	FText GetText() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "SetIsReadOnly (Saga Text Input Box)"))
	void SetReadOnly(bool flag) noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "GetIsReadOnly (Saga Text Input Box)"))
	bool IsReadOnly() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "Set Hint Text (Saga Text Input Box)"))
	void SetHintText(FText text) noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "Get Hint Text (Saga Text Input Box)"))
	FText GetHintText() const noexcept;

	UFUNCTION(BlueprintSetter, Category = "CandyLandSaga|UI|Appearance")
	void SetJustification(ETextJustify::Type justification) noexcept;

	UFUNCTION(BlueprintGetter, BlueprintPure, Category = "CandyLandSaga|UI|Appearance")
	ETextJustify::Type GetJustification() const noexcept;

	UFUNCTION(BlueprintSetter, Category = "CandyLandSaga|UI|Content")
	void SetTextOverflowPolicy(ETextOverflowPolicy policy) noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Content")
	ETextOverflowPolicy GetTextOverflowPolicy() const noexcept;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void HandleOnTextChanged(const FText& text);
	UFUNCTION()
	void HandleOnTextCommitted(const FText& text, ETextCommit::Type method);

	PROPERTY_BINDING_IMPLEMENTATION(FText, hintText);
};
