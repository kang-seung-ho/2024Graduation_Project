#pragma once
#include "CoreMinimal.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SWidget.h"
#include "Components/ContentWidget.h"

#include "Saga/Interface/SagaUserWidget.h"
#include "SagaButtonWidget.generated.h"

class SButton;
class USlateWidgetStyleAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonClickedEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonPressedEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonReleasedEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonHoverEvent);

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|UI")
class SAGAFRAMEWORK_API USagaButtonWidget : public USagaUserWidget
{
	GENERATED_BODY()

public:
	USagaButtonWidget(const FObjectInitializer& ObjectInitializer);

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

	//~ Begin UObject Interface
	virtual void PostLoad() override;
	//~ End UObject Interface

	/** Sets the color multiplier for the button background */
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Button|Appearance")
	void SetStyle(const FButtonStyle& InStyle);

	const FButtonStyle& GetStyle() const noexcept;

	void SetColorAndOpacity(const FLinearColor& InColorAndOpacity);

	/** Sets the color multiplier for the button background */
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Button|Appearance")
	void SetBackgroundColor(const FLinearColor& InBackgroundColor);

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Button")
	void SetClickMethod(EButtonClickMethod::Type InClickMethod);

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Button")
	void SetTouchMethod(EButtonTouchMethod::Type InTouchMethod);

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Button")
	void SetPressMethod(EButtonPressMethod::Type InPressMethod);

	/**
	 * Returns true if the user is actively pressing the button.  Do not use this for detecting 'Clicks', use the OnClicked event instead.
	 *
	 * @return true if the user is actively pressing the button otherwise false.
	 */
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Button")
	bool IsPressed() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Button")
	FLinearColor GetColorAndOpacity() const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Button")
	FLinearColor GetBackgroundColor() const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Button")
	EButtonClickMethod::Type GetClickMethod() const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Button")
	EButtonTouchMethod::Type GetTouchMethod() const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Button")
	EButtonPressMethod::Type GetPressMethod() const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Button")
	bool GetIsFocusable() const noexcept;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

	UE_DEPRECATED(5.2, "Direct access to WidgetStyle is deprecated. Please use the getter and setter.")
		/** The button style used at runtime */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter = "GetStyle", Setter = "SetStyle", BlueprintSetter = "SetStyle", Category = "Appearance", meta = (DisplayName = "Style"))
	FButtonStyle WidgetStyle;

	UE_DEPRECATED(5.2, "Direct access to BackgroundColor is deprecated. Please use the getter and setter.")
		/** The color multiplier for the button background */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, BlueprintSetter = "SetBackgroundColor", Category = "Appearance", meta = (sRGB = "true"))
	FLinearColor BackgroundColor;

	UE_DEPRECATED(5.2, "Direct access to ClickMethod is deprecated. Please use the getter and setter.")
		/** The type of mouse action required by the user to trigger the buttons 'Click' */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, BlueprintSetter = "SetClickMethod", Category = "Interaction", AdvancedDisplay)
	TEnumAsByte<EButtonClickMethod::Type> ClickMethod;

	UE_DEPRECATED(5.2, "Direct access to TouchMethod is deprecated. Please use the getter and setter.")
		/** The type of touch action required by the user to trigger the buttons 'Click' */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, BlueprintSetter = "SetTouchMethod", Category = "Interaction", AdvancedDisplay)
	TEnumAsByte<EButtonTouchMethod::Type> TouchMethod;

	UE_DEPRECATED(5.2, "Direct access to PressMethod is deprecated. Please use the getter and setter.")
		/** The type of keyboard/gamepad button press action required by the user to trigger the buttons 'Click' */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, BlueprintSetter = "SetPressMethod", Category = "Interaction", AdvancedDisplay)
	TEnumAsByte<EButtonPressMethod::Type> PressMethod;

	UE_DEPRECATED(5.2, "Direct access to IsFocusable is deprecated. Please use the getter.")
		/** Sometimes a button should only be mouse-clickable and never keyboard focusable. */
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Getter, Category = "Interaction")
	bool IsFocusable;

	/** Called when the button is clicked */
	UPROPERTY(BlueprintAssignable, Category = "Button|Event")
	FOnButtonClickedEvent OnClicked;

	/** Called when the button is pressed */
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI|Button|Event")
	FOnButtonPressedEvent OnPressed;

	/** Called when the button is released */
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI|Button|Event")
	FOnButtonReleasedEvent OnReleased;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI|Button|Event")
	FOnButtonHoverEvent OnHovered;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI|Button|Event")
	FOnButtonHoverEvent OnUnhovered;

protected:
	/** Handle the actual click event from slate and forward it on */
	FReply SlateHandleClicked();
	UFUNCTION()
	void SlateHandlePressed();
	UFUNCTION()
	void SlateHandleReleased();
	UFUNCTION()
	void SlateHandleHovered();
	UFUNCTION()
	void SlateHandleUnhovered();

	// Initialize IsFocusable in the constructor before the SWidget is constructed.
	void InitIsFocusable(bool InIsFocusable) noexcept;

	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
#if WITH_EDITOR
	virtual TSharedRef<SWidget> RebuildDesignWidget(TSharedRef<SWidget> Content) override
	{
		return Content;
	}
#endif
	//~ End UWidget Interface

#if WITH_ACCESSIBILITY
	virtual TSharedPtr<SWidget> GetAccessibleWidget() const override;
#endif

	TSharedPtr<SButton> MyButton;
};
