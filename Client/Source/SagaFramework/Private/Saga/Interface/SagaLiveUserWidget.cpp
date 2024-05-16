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
	, OnJustOpened(), OnOpended(), OnClosed(), OnIdle()
{
	everyStates.Add(ESagaLiveUserWidgetStates::None).SetProgress(0);
	everyStates.Add(ESagaLiveUserWidgetStates::Opening).SetProgress(0);
	everyStates.Add(ESagaLiveUserWidgetStates::Idle).SetProgress(0);
	everyStates.Add(ESagaLiveUserWidgetStates::Closing).SetProgress(0);

	//auto mBody = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Body"));
}

void
USagaLiveUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	OnJustOpened.AddDynamic(this, &USagaLiveUserWidget::ShowOnOpened);
	OnClosed.AddDynamic(this, &USagaLiveUserWidget::HideOnClosed);
}

void
USagaLiveUserWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

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

	if (0 < myChildren.Num())
	{
		// Just let them be opened
		for (auto& child : myChildren)
		{
			child->OpenNow();
		}
	}
	const auto my_name = GetName();
	UE_LOG(LogSagaFramework, Log, TEXT("[USagaLiveUserWidget] '%s' found %d children."), *my_name, myChildren.Num());
}

void
USagaLiveUserWidget::NativeTick(const FGeometry& geometry, float delta_time)
{
	auto my_parent = myParent.Get();
	if (IsParentProgressive() and nullptr != my_parent)
	{
		SetProgress(my_parent->GetProgress());
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
				OnProgressEnded.Broadcast(myState);

				switch (myState)
				{
				case ESagaLiveUserWidgetStates::Opening:
				{
					everyStates[ESagaLiveUserWidgetStates::Idle].SetProgress(1);

					myState = ESagaLiveUserWidgetStates::Idle;

					BroadcastOnOpened();
				}
				break;

				case ESagaLiveUserWidgetStates::Idle:
				{
					BroadcastOnIdle();
				}
				break;

				case ESagaLiveUserWidgetStates::Closing:
				{
					everyStates[ESagaLiveUserWidgetStates::Idle].SetProgress(0);

					myState = ESagaLiveUserWidgetStates::Idle;

					BroadcastOnClosed();
				}
				break;
				}
			}
		}
	}

	Super::NativeTick(geometry, delta_time);
}

void
USagaLiveUserWidget::NativeDestruct()
{
	Super::NativeDestruct();

	OnProgressEnded.Clear();
	OnJustOpened.Clear();
	OnOpended.Clear();
	OnClosed.Clear();
	OnIdle.Clear();
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
USagaLiveUserWidget::BroadcastOnJustOpened()
{
	OnJustOpened.Broadcast();

	for (auto& child : myChildren)
	{
		child->BroadcastOnJustOpened();
	}
}

void
USagaLiveUserWidget::BroadcastOnOpened()
{
	OnOpended.Broadcast();

	for (auto& child : myChildren)
	{
		child->BroadcastOnOpened();
	}
}

void
USagaLiveUserWidget::BroadcastOnClosed()
{
	OnClosed.Broadcast();

	for (auto& child : myChildren)
	{
		child->BroadcastOnClosed();
	}
}

void
USagaLiveUserWidget::BroadcastOnIdle()
{
	OnIdle.Broadcast();

	for (auto& child : myChildren)
	{
		child->BroadcastOnIdle();
	}
}

void
USagaLiveUserWidget::ShowOnOpened()
{
	SetVisibility(ESlateVisibility::Visible);
}

void
USagaLiveUserWidget::HideOnClosed()
{
	SetVisibility(ESlateVisibility::Hidden);
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
		myTickPolicy = ESagaLiveUserWidgetTickPolicy::FromParent;
	}
	else
	{
		myTickPolicy = ESagaLiveUserWidgetTickPolicy::FromParent;
	}
}

bool
USagaLiveUserWidget::IsParentProgressive()
const noexcept
{
	return myTickPolicy != ESagaLiveUserWidgetTickPolicy::FromParent;
}

void
USagaLiveUserWidget::SetProgressTickable(const bool flag)
noexcept
{
	if (flag)
	{
		myTickPolicy = ESagaLiveUserWidgetTickPolicy::TickBySelf;
	}
	else
	{
		myTickPolicy = ESagaLiveUserWidgetTickPolicy::TickBySelf;
	}
}

bool
USagaLiveUserWidget::IsProgressTickable()
const noexcept
{
	return myTickPolicy != ESagaLiveUserWidgetTickPolicy::TickBySelf;
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
	everyStates[ESagaLiveUserWidgetStates::Opening].SetProgress(begin_progress);
	everyStates[ESagaLiveUserWidgetStates::Idle].SetProgress(1);

	myState = ESagaLiveUserWidgetStates::Opening;

	BroadcastOnJustOpened();
}

void
USagaLiveUserWidget::Close(const float begin_progress)
{
	everyStates[ESagaLiveUserWidgetStates::Closing].SetProgress(begin_progress);
	everyStates[ESagaLiveUserWidgetStates::Idle].SetProgress(0);

	myState = ESagaLiveUserWidgetStates::Closing;
}

void
USagaLiveUserWidget::OpenNow()
{
	Open(1);
	BroadcastOnJustOpened();
	BroadcastOnOpened();
}

void
USagaLiveUserWidget::CloseNow()
{
	Close(1);
	BroadcastOnClosed();
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
	return EaseProgress(GetProgress());
}
