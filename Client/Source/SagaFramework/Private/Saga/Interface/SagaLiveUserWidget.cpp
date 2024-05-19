#include "Saga/Interface/SagaLiveUserWidget.h"
#include <Kismet/GameplayStatics.h>
#include <Components/CapsuleComponent.h>
#include <Blueprint/WidgetTree.h>
#include <Math/UnrealMathUtility.h>

USagaLiveUserWidget::USagaLiveUserWidget(const FObjectInitializer& initializer)
noexcept
	: USagaUserWidget(initializer)
	, myState(ESagaLiveUserWidgetStates::None)
	, myParent(), myChildren()
	, everyStates()
	, myProgress(), myTickPolicy(ESagaLiveUserWidgetTickPolicy::TickBySelf)
	, isUserInteractable(true)
	, OnProgressEnded()
	, OnJustOpened(), OnOpened(), OnClosed(), OnIdle()
{
	everyStates.Add(ESagaLiveUserWidgetStates::None).SetProgress(0);
	everyStates.Add(ESagaLiveUserWidgetStates::Opening).SetProgress(0);
	everyStates.Add(ESagaLiveUserWidgetStates::Idle).SetProgress(1);
	everyStates.Add(ESagaLiveUserWidgetStates::Closing).SetProgress(0);
	everyStates.Add(ESagaLiveUserWidgetStates::Dead).SetProgress(1);

	//auto mBody = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Body"));
}

void
USagaLiveUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	auto& tree = *WidgetTree;
	tree.ForEachWidget([this](UWidget* const element) -> void
		{
			if (element == this) return;

			if (auto livewidget = Cast<USagaLiveUserWidget>(element))
			{
				livewidget->SetSagaWidgetParent(this);
			}
		}
	);

	const auto my_name = GetName();
	UE_LOG(LogSagaFramework, Log, TEXT("[USagaLiveUserWidget] '%s' found %d children."), *my_name, myChildren.Num());

	OnJustOpened.AddDynamic(this, &USagaLiveUserWidget::HandleJustOpened);
	OnOpened.AddDynamic(this, &USagaLiveUserWidget::HandleOpened);
	OnClosed.AddDynamic(this, &USagaLiveUserWidget::HandleClosed);
}

void
USagaLiveUserWidget::NativeTick(const FGeometry& geometry, float delta_time)
{
	auto my_parent = myParent.Get();
	if (IsParentProgressive() and nullptr != my_parent)
	{
		myProgress = my_parent->GetProgress();

		SetProgress(myProgress);
	}
	else if (IsProgressTickable())
	{
		const auto curr_progress = GetInternalProgress();

		if (curr_progress != 1.0f)
		{
			auto& status = GetStatus();
			const float new_prog = status.Tick(delta_time);

			if (1.0f <= new_prog)
			{
				const auto name = GetName();

				OnProgressEnded.Broadcast(myState);

				switch (myState)
				{
				case ESagaLiveUserWidgetStates::Opening:
				{
					UE_LOG(LogSagaFramework, Display, TEXT("[USagaLiveUserWidget] '%s' is opened."), *name);

					BroadcastOnOpened();
				}
				break;

				case ESagaLiveUserWidgetStates::Idle:
				{
					UE_LOG(LogSagaFramework, Display, TEXT("[USagaLiveUserWidget] '%s' is on idle."), *name);

					BroadcastOnIdle();
				}
				break;

				case ESagaLiveUserWidgetStates::Closing:
				{
					UE_LOG(LogSagaFramework, Display, TEXT("[USagaLiveUserWidget] '%s' is closed."), *name);

					BroadcastOnClosed();
				}
				break;

				case ESagaLiveUserWidgetStates::Dead:
				{
					UE_LOG(LogSagaFramework, Display, TEXT("[USagaLiveUserWidget] '%s' is on dead."), *name);
				}
				break;

				default:
				{}
				break;
				}
			} // IF (1.0f <= new_prog)

		} // IF (curr_progress != 1.0f)

		myProgress = curr_progress;
	} // IF (IsProgressTickable)

	Super::NativeTick(geometry, delta_time);
}

void
USagaLiveUserWidget::SetTransitionPeriod(ESagaLiveUserWidgetStates state, const float period)
noexcept
{
	everyStates[state].SetPeriod(period);
}

void
USagaLiveUserWidget::SetProgress(const float value)
noexcept
{
	everyStates[myState].SetProgress(value);
}

void
USagaLiveUserWidget::HandleJustOpened()
{
	SetVisibility(ESlateVisibility::Visible);
}

void
USagaLiveUserWidget::HandleOpened()
{
	myState = ESagaLiveUserWidgetStates::Idle;
}

void
USagaLiveUserWidget::HandleClosed()
{
	SetVisibility(ESlateVisibility::Hidden);

	myState = ESagaLiveUserWidgetStates::Dead;
}

float
USagaLiveUserWidget::GetInternalProgress()
const noexcept
{
	return everyStates[myState].GetProgress();
}

float
USagaLiveUserWidget::EaseProgress_Implementation(const float& progress)
const
{
	return progress;
}

FSagaLiveUserWidgetStatus&
USagaLiveUserWidget::GetStatus()
noexcept
{
	return everyStates[myState];
}

FSagaLiveUserWidgetStatus&
USagaLiveUserWidget::GetSpecificStatus(const ESagaLiveUserWidgetStates state)
noexcept
{
	return everyStates[state];
}

void
USagaLiveUserWidget::SetSagaWidgetParent(USagaLiveUserWidget* const ptr)
{
	if (auto old_parent = myParent.Get())
	{
		old_parent->myChildren.Remove(this);
	}

	if (ptr)
	{
		ptr->myChildren.Add(this);
		myParent = ptr;
	}
	else
	{
		myParent = nullptr;
	}
}

USagaLiveUserWidget*
USagaLiveUserWidget::GetSagaWidgetParent()
const
{
	return myParent.Get();
}

bool
USagaLiveUserWidget::IsBoundedSagaWidget()
const
{
	return myParent != nullptr;
}

void
USagaLiveUserWidget::SetParentProgressive(const bool flag)
noexcept
{
	if (flag)
	{
		myTickPolicy |= ESagaLiveUserWidgetTickPolicy::FromParent;
	}
	else
	{
		myTickPolicy &= ~ESagaLiveUserWidgetTickPolicy::FromParent;
	}
}

bool
USagaLiveUserWidget::IsParentProgressive()
const noexcept
{
	return myTickPolicy != ESagaLiveUserWidgetTickPolicy::TickBySelf;
}

void
USagaLiveUserWidget::SetProgressTickable(const bool flag)
noexcept
{
	if (flag)
	{
		myTickPolicy |= ESagaLiveUserWidgetTickPolicy::TickBySelf;
	}
	else
	{
		myTickPolicy &= ~ESagaLiveUserWidgetTickPolicy::TickBySelf;
	}
}

bool
USagaLiveUserWidget::IsProgressTickable()
const noexcept
{
	return ESagaLiveUserWidgetTickPolicy::None != (myTickPolicy & ESagaLiveUserWidgetTickPolicy::TickBySelf);
}

void
USagaLiveUserWidget::SetUserInteractable(const bool flag)
noexcept
{
	isUserInteractable = flag;
}

bool
USagaLiveUserWidget::IsUserInteractable()
const noexcept
{
	return isUserInteractable;
}

void
USagaLiveUserWidget::Open(const float begin_progress)
{
	const auto name = GetName();
	UE_LOG(LogSagaFramework, Display, TEXT("[USagaLiveUserWidget] Opening '%s'..."), *name);

	everyStates[ESagaLiveUserWidgetStates::Opening].SetProgress(begin_progress);

	myState = ESagaLiveUserWidgetStates::Opening;

	if (0 < myChildren.Num())
	{
		for (auto& child : myChildren)
		{
			child->Open(begin_progress);
		}
	}

	BroadcastOnJustOpened();
}

void
USagaLiveUserWidget::Close(const float begin_progress)
{
	const auto name = GetName();
	UE_LOG(LogSagaFramework, Display, TEXT("[USagaLiveUserWidget] Closing '%s'..."), *name);

	everyStates[ESagaLiveUserWidgetStates::Closing].SetProgress(begin_progress);

	myState = ESagaLiveUserWidgetStates::Closing;

	if (0 < myChildren.Num())
	{
		for (auto& child : myChildren)
		{
			child->Close(begin_progress);
		}
	}
}

void
USagaLiveUserWidget::OpenNow()
{
	everyStates[ESagaLiveUserWidgetStates::Idle].SetProgress(1);

	myState = ESagaLiveUserWidgetStates::Idle;

	if (0 < myChildren.Num())
	{
		for (auto& child : myChildren)
		{
			child->OpenNow();
		}
	}

	if (OnOpened.IsBound())
	{
		OnOpened.Broadcast();
	}
}

void
USagaLiveUserWidget::CloseNow()
{
	everyStates[ESagaLiveUserWidgetStates::Dead].SetProgress(1);

	myState = ESagaLiveUserWidgetStates::Dead;

	if (0 < myChildren.Num())
	{
		for (auto& child : myChildren)
		{
			child->CloseNow();
		}
	}

	if (OnClosed.IsBound())
	{
		OnClosed.Broadcast();
	}
}

float
USagaLiveUserWidget::GetProgress()
const noexcept
{
	return myProgress;
}

float
USagaLiveUserWidget::GetGraphicalProgress()
const noexcept
{
	switch (myState)
	{
	case ESagaLiveUserWidgetStates::Closing:
	case ESagaLiveUserWidgetStates::Dead:
	{
		return EaseProgress(1.00000000f - GetProgress());
	}
	break;

	default:
	{
		return EaseProgress(GetProgress());
	}
	break;
	}
}

void
USagaLiveUserWidget::BroadcastOnJustOpened()
{
	if (OnJustOpened.IsBound())
	{
		OnJustOpened.Broadcast();
	}

	for (auto& child : myChildren)
	{
		child->BroadcastOnJustOpened();
	}
}

void
USagaLiveUserWidget::BroadcastOnOpened()
{
	if (OnOpened.IsBound())
	{
		OnOpened.Broadcast();
	}

	for (auto& child : myChildren)
	{
		child->BroadcastOnOpened();
	}
}

void
USagaLiveUserWidget::BroadcastOnClosed()
{
	if (OnClosed.IsBound())
	{
		OnClosed.Broadcast();
	}

	for (auto& child : myChildren)
	{
		child->BroadcastOnClosed();
	}
}

void
USagaLiveUserWidget::BroadcastOnIdle()
{
	if (OnIdle.IsBound())
	{
		OnIdle.Broadcast();
	}

	for (auto& child : myChildren)
	{
		child->BroadcastOnIdle();
	}
}
