#pragma once
#include "Saga/Interface/SagaLiveUserWidget.h"
#include <Components/Widget.h>
#include <Components/TextBlock.h>
#include <Components/Button.h>

#include "SagaUiButton.generated.h"

UCLASS(BlueprintType, Blueprintable, Abstract, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Button C++ Class"))
class SAGAFRAMEWORK_API USagaUiButton : public USagaLiveUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI|Button")
	TObjectPtr<UButton> myButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI|Button")
	TObjectPtr<UTextBlock> myLabel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = SetText, BlueprintGetter = GetText, Category = "CandyLandSaga|UI|Content")
	FText buttonCaption;

	UPROPERTY()
	FGetText buttonCaptionDelegate;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI|Button|Event")
	FOnButtonClickedEvent OnClicked;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI|Button|Event")
	FOnButtonPressedEvent OnPressed;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI|Button|Event")
	FOnButtonReleasedEvent OnReleased;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI|Button|Event")
	FOnButtonHoverEvent OnHovered;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI|Button|Event")
	FOnButtonHoverEvent OnUnhovered;

	USagaUiButton(const FObjectInitializer& initializer);

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "SetText (Saga Button)"))
	void SetText(FText text) noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "GetText (Saga Button)"))
	FText GetText() const noexcept;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleClicked();
	UFUNCTION()
	void HandlePressed();
	UFUNCTION()
	void HandleReleased();
	UFUNCTION()
	void HandleHovered();
	UFUNCTION()
	void HandleUnhovered();

	PROPERTY_BINDING_IMPLEMENTATION(FText, buttonCaption);
};
