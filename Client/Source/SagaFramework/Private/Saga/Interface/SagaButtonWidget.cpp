#include "Saga/Interface/SagaButtonWidget.h"

#include "Binding/States/WidgetStateRegistration.h"
#include "Styling/DefaultStyleCache.h"
#include "Styling/UMGCoreStyle.h"

#define LOCTEXT_NAMESPACE "UMG"

USagaButtonWidget::USagaButtonWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS;
	WidgetStyle = UE::Slate::Private::FDefaultStyleCache::GetRuntime().GetButtonStyle();
	PRAGMA_ENABLE_DEPRECATION_WARNINGS;

#if WITH_EDITOR 
	if (IsEditorWidget())
	{
		PRAGMA_DISABLE_DEPRECATION_WARNINGS;
		WidgetStyle = UE::Slate::Private::FDefaultStyleCache::GetEditor().GetButtonStyle();
		PRAGMA_ENABLE_DEPRECATION_WARNINGS;

		// The CDO isn't an editor widget and thus won't use the editor style, call post edit change to mark difference from CDO
		PostEditChange();
	}
#endif // WITH_EDITOR

	PRAGMA_DISABLE_DEPRECATION_WARNINGS;
	ColorAndOpacity = FLinearColor::White;
	BackgroundColor = FLinearColor::White;

	ClickMethod = EButtonClickMethod::DownAndUp;
	TouchMethod = EButtonTouchMethod::DownAndUp;

	IsFocusable = true;
	PRAGMA_ENABLE_DEPRECATION_WARNINGS;

#if WITH_EDITORONLY_DATA
	AccessibleBehavior = ESlateAccessibleBehavior::Summary;
	bCanChildrenBeAccessible = false;
#endif
}

void
USagaButtonWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (!MyButton.IsValid())
	{
		return;
	}

	PRAGMA_DISABLE_DEPRECATION_WARNINGS;
	MyButton->SetButtonStyle(&WidgetStyle);
	MyButton->SetColorAndOpacity(ColorAndOpacity);
	MyButton->SetBorderBackgroundColor(BackgroundColor);
	MyButton->SetClickMethod(ClickMethod);
	MyButton->SetTouchMethod(TouchMethod);
	MyButton->SetPressMethod(PressMethod);
	PRAGMA_ENABLE_DEPRECATION_WARNINGS;
}

TSharedRef<SWidget>
USagaButtonWidget::RebuildWidget()
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS;
	MyButton = SNew(SButton)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClicked))
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressed))
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleased))
		.OnHovered_UObject(this, &ThisClass::SlateHandleHovered)
		.OnUnhovered_UObject(this, &ThisClass::SlateHandleUnhovered)
		.ButtonStyle(&WidgetStyle)
		.ClickMethod(ClickMethod)
		.TouchMethod(TouchMethod)
		.PressMethod(PressMethod)
		.IsFocusable(IsFocusable)
		;
	PRAGMA_ENABLE_DEPRECATION_WARNINGS;

	return MyButton.ToSharedRef();
}

void
USagaButtonWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyButton.Reset();
}

void
USagaButtonWidget::PostLoad()
{
	Super::PostLoad();
}

FReply
USagaButtonWidget::SlateHandleClicked()
{
	OnClicked.Broadcast();

	return FReply::Handled();
}

void
USagaButtonWidget::SlateHandlePressed()
{
	OnPressed.Broadcast();
	BroadcastBinaryPostStateChange(UWidgetPressedStateRegistration::Bit, true);
}

void
USagaButtonWidget::SlateHandleReleased()
{
	OnReleased.Broadcast();
	BroadcastBinaryPostStateChange(UWidgetPressedStateRegistration::Bit, false);
}

void
USagaButtonWidget::SlateHandleHovered()
{
	OnHovered.Broadcast();
	BroadcastBinaryPostStateChange(UWidgetHoveredStateRegistration::Bit, true);
}

void
USagaButtonWidget::SlateHandleUnhovered()
{
	OnUnhovered.Broadcast();
	BroadcastBinaryPostStateChange(UWidgetHoveredStateRegistration::Bit, false);
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS
void
USagaButtonWidget::InitIsFocusable(bool InIsFocusable)
noexcept
{
	IsFocusable = InIsFocusable;
}

void
USagaButtonWidget::SetStyle(const FButtonStyle& InStyle)
{
	WidgetStyle = InStyle;
	if (MyButton.IsValid())
	{
		MyButton->SetButtonStyle(&WidgetStyle);
	}
}

void
USagaButtonWidget::SetColorAndOpacity(const FLinearColor& InColorAndOpacity)
{
	ColorAndOpacity = InColorAndOpacity;

	if (MyButton.IsValid())
	{
		MyButton->SetColorAndOpacity(InColorAndOpacity);
	}
}

void
USagaButtonWidget::SetBackgroundColor(const FLinearColor& InBackgroundColor)
{
	BackgroundColor = InBackgroundColor;

	if (MyButton.IsValid())
	{
		MyButton->SetBorderBackgroundColor(InBackgroundColor);
	}
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS
void
USagaButtonWidget::SetClickMethod(EButtonClickMethod::Type InClickMethod)
{
	ClickMethod = InClickMethod;

	if (MyButton.IsValid())
	{
		MyButton->SetClickMethod(ClickMethod);
	}
}

void
USagaButtonWidget::SetTouchMethod(EButtonTouchMethod::Type InTouchMethod)
{
	TouchMethod = InTouchMethod;

	if (MyButton.IsValid())
	{
		MyButton->SetTouchMethod(TouchMethod);
	}
}

void
USagaButtonWidget::SetPressMethod(EButtonPressMethod::Type InPressMethod)
{
	PressMethod = InPressMethod;
	if (MyButton.IsValid())
	{
		MyButton->SetPressMethod(PressMethod);
	}
}

const FButtonStyle&
USagaButtonWidget::GetStyle()
const noexcept
{
	return WidgetStyle;
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS

FLinearColor
USagaButtonWidget::GetColorAndOpacity()
const noexcept
{
	return ColorAndOpacity;
}

FLinearColor
USagaButtonWidget::GetBackgroundColor()
const noexcept
{
	return BackgroundColor;
}

EButtonClickMethod::Type
USagaButtonWidget::GetClickMethod()
const noexcept
{
	return ClickMethod;
}

EButtonTouchMethod::Type
USagaButtonWidget::GetTouchMethod()
const noexcept
{
	return TouchMethod;
}

EButtonPressMethod::Type
USagaButtonWidget::GetPressMethod()
const noexcept
{
	return PressMethod;
}

bool
USagaButtonWidget::GetIsFocusable()
const noexcept
{
	return IsFocusable;
}

bool
USagaButtonWidget::IsPressed()
const noexcept
{
	if (MyButton.IsValid())
	{
		return MyButton->IsPressed();
	}

	return false;
}

#if WITH_ACCESSIBILITY
TSharedPtr<SWidget>
USagaButtonWidget::GetAccessibleWidget()
const
{
	return MyButton;
}
#endif

#if WITH_EDITOR
const FText
USagaButtonWidget::GetPaletteCategory()
{
	return LOCTEXT("Common", "Common");
}
#endif
