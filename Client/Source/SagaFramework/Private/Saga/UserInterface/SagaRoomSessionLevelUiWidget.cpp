#include "Saga/Interface/SagaRoomSessionLevelUiWidget.h"
#include <Internationalization/Text.h>
#include <Templates/Casts.h>
#include <Components/Widget.h>
#include <Components/ListView.h>
#include <Blueprint/WidgetTree.h>

#include "Saga/Interface/SagaUiButton.h"
#include "Saga/Interface/SagaRoomMemberViewerUiWidget.h"
#include "Saga/Interface/Data/SagaUiInstancedUserData.h"

#include "SagaGame/Player/SagaPlayerTeam.h"

#include "Saga/Network/SagaNetworkSubSystem.h"
#include "Saga/Network/SagaVirtualRoom.h"

USagaRoomSessionLevelUiWidget::USagaRoomSessionLevelUiWidget(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, myRedTeamMemberListView(), myBlueTeamMemberListView()
	, myStartButton(), myQuitButton()
	, redTeamCount(), bluTeamCount()
{}

void
USagaRoomSessionLevelUiWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void
USagaRoomSessionLevelUiWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
			if (element == this) return;

			if (auto viwer = Cast<USagaRoomMemberViewerUiWidget>(element))
			{
				const auto name = viwer->GetName();

				if (name == TEXT("RedTeamListViewerPanel"))
				{
					myRedTeamMemberListView = viwer;
				}
				else if (name == TEXT("BluTeamListViewerPanel"))
				{
					myBlueTeamMemberListView = viwer;
				}
			}
			else if (auto btn = Cast<USagaUiButton>(element))
			{
				const auto name = btn->GetName();

				if (name == TEXT("StartButton"))
				{
					myStartButton = btn;
				}
				else if (name == TEXT("QuitButton"))
				{
					myQuitButton = btn;
				}
			}
		}
	);

	const auto my_name = GetName();
	if (nullptr == myRedTeamMemberListView)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaRoomSessionLevelUiWidget] '%s' could not found the red team panel in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaRoomSessionLevelUiWidget] '%s' found the red team panel in children."), *my_name);
	}

	if (nullptr == myBlueTeamMemberListView)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaRoomSessionLevelUiWidget] '%s' could not found the red team panel in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaRoomSessionLevelUiWidget] '%s' found the red team panel in children."), *my_name);
	}

	if (nullptr == myStartButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaRoomSessionLevelUiWidget] '%s' could not found the starting game button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaRoomSessionLevelUiWidget] '%s' found the starting game button in children."), *my_name);
	}

	if (nullptr == myQuitButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaRoomSessionLevelUiWidget] '%s' could not found the quit button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaRoomSessionLevelUiWidget] '%s' found the quit button in children."), *my_name);
	}

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	net->OnUpdateMembers.AddDynamic(this, &USagaRoomSessionLevelUiWidget::OnUpdateMembers);
}

void
USagaRoomSessionLevelUiWidget::NativeDestruct()
{
	Super::NativeDestruct();

	/*
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	net->OnUpdateMembers.RemoveDynamic(this, &USagaRoomSessionLevelUiWidget::OnUpdateMembers);
	*/
}

void
USagaRoomSessionLevelUiWidget::OnUpdateMembers(const TArray<struct FSagaVirtualUser>& list)
{
	redTeamCount = 0;
	bluTeamCount = 0;

	for (auto& member : list)
	{
		const auto& member_name = member.myName;
		const auto& member_name_str = member_name.ToString();

		if (member.myTeam == ESagaPlayerTeam::Red)
		{
			UE_LOG(LogSagaFramework, Log, TEXT("[USagaRoomSessionLevelUiWidget][OnUpdateMembers] '%s' is in red team."), *member_name_str);

			++redTeamCount;
		}
		else if (member.myTeam == ESagaPlayerTeam::Blue)
		{
			UE_LOG(LogSagaFramework, Log, TEXT("[USagaRoomSessionLevelUiWidget][OnUpdateMembers] '%s' is in blue team."), *member_name_str);

			++bluTeamCount;
		}
		else
		{
			UE_LOG(LogSagaFramework, Log, TEXT("[USagaRoomSessionLevelUiWidget][OnUpdateMembers] '%s' is in unknown team '%d'."), *member_name_str, static_cast<int32>(member.myTeam));
		}
	}
}

FText
USagaRoomSessionLevelUiWidget::GetRoomTitleLabelText()
const
{
	const FText fmt = FText::FromString(TEXT("-{0}-"));
	const auto title = AcquireRoomTitle();

	if (title.IsEmpty())
	{
		return FText::FromString(TEXT("-Empty Room-"));
	}
	{
		return FText::Format(fmt, title);
	}
}

FText
USagaRoomSessionLevelUiWidget::GetRoomMemberCountLabelText()
const
{
	const auto cnt = AcquireRoomMemberCount();
	static const auto fmt = FString::Printf(TEXT("Members: (%d)"), cnt);

	return FText::FromString(fmt);
}

FText
USagaRoomSessionLevelUiWidget::GetRedTeamMemberCountLabelText()
const
{
	const auto cnt = AcquireRedTeamMemberCount();
	static const auto fmt = FString::Printf(TEXT("Red Team: (%d)"), cnt);

	return FText::FromString(fmt);
}

FText
USagaRoomSessionLevelUiWidget::GetBluTeamMemberCountLabelText()
const
{
	const auto cnt = AcquireBluTeamMemberCount();
	static const auto fmt = FString::Printf(TEXT("Blue Team: (%d)"), cnt);

	return FText::FromString(fmt);
}

FText
USagaRoomSessionLevelUiWidget::AcquireRoomTitle()
const
{
	static auto err_text = FText::FromString(TEXT("-"));

	const auto world = GetWorld();
	if (nullptr == world)
	{
		return err_text;
	}

	const auto singleton = world->GetGameInstance();
	if (nullptr == singleton)
	{
		return err_text;
	}

	const auto net = singleton->GetSubsystem<USagaNetworkSubSystem>();
	if (nullptr == net)
	{
		return err_text;
	}
	else
	{
		return net->GetCurrentRoomTitle();
	}
}

int32
USagaRoomSessionLevelUiWidget::AcquireRoomMemberCount()
const
{
	const auto world = GetWorld();
	if (nullptr == world)
	{
		return 0;
	}

	const auto singleton = world->GetGameInstance();
	if (nullptr == singleton)
	{
		return 0;
	}

	const auto net = singleton->GetSubsystem<USagaNetworkSubSystem>();
	if (nullptr == net)
	{
		return 0;
	}
	else
	{
		return net->GetCurrentRoomMemberCount();
	}
}

int32
USagaRoomSessionLevelUiWidget::AcquireRedTeamMemberCount()
const
{
	const auto world = GetWorld();
	if (nullptr == world)
	{
		return 0;
	}

	const auto singleton = world->GetGameInstance();
	if (nullptr == singleton)
	{
		return 0;
	}

	const auto net = singleton->GetSubsystem<USagaNetworkSubSystem>();
	if (nullptr == net)
	{
		return 0;
	}
	else if (net->IsOfflineMode())
	{
		const ESagaPlayerTeam team = net->GetLocalUserTeam();

		return (team == ESagaPlayerTeam::Unknown) ? -1 : ((team == ESagaPlayerTeam::Red) ? 1 : 0);
	}
	else
	{
		return redTeamCount;
	}
}

int32
USagaRoomSessionLevelUiWidget::AcquireBluTeamMemberCount()
const
{
	const auto world = GetWorld();
	if (nullptr == world)
	{
		return 0;
	}

	const auto singleton = world->GetGameInstance();
	if (nullptr == singleton)
	{
		return 0;
	}

	const auto net = singleton->GetSubsystem<USagaNetworkSubSystem>();
	if (nullptr == net)
	{
		return 0;
	}
	else if (net->IsOfflineMode())
	{
		const ESagaPlayerTeam team = net->GetLocalUserTeam();

		return (team == ESagaPlayerTeam::Unknown) ? -1 : ((team == ESagaPlayerTeam::Blue) ? 1 : 0);
	}
	else
	{
		return bluTeamCount;
	}
}
