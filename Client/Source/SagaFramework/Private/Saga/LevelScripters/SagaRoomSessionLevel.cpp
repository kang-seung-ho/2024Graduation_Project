#include "Saga/LevelScripters/SagaRoomSessionLevel.h"
#include <Logging/LogMacros.h>
#include <Internationalization/Text.h>
#include <UObject/ConstructorHelpers.h>
#include <Delegates/Delegate.h>
#include <Components/ListView.h>

#include "Saga/Interface/SagaUiButton.h"
#include "Saga/Interface/SagaUiPopup.h"
#include "Saga/Interface/SagaRoomSessionLevelUiWidget.h"
#include "Saga/Interface/SagaRoomMemberViewerUiWidget.h"
#include "Saga/Interface/Data/SagaUiInstancedUserData.h"

#include "Saga/Network/SagaVirtualUser.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaRoomSessionLevel::ASagaRoomSessionLevel()
noexcept
	: Super()
	, levelUiClass(), levelUiInstance()
	, errorPopupWidgetClass(), errorPopupWidgetInstance()
	, userDataClass()
	, teamUpdateTimer()
{
	SetNextLevelName(TEXT("CharacterSelectLevel"));
	SetPrevLevelName(TEXT("LobbyLevel"));

	static ConstructorHelpers::FClassFinder<USagaRoomSessionLevelUiWidget> widget_class{ TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Level/SagaRoomSessionLevelUI.SagaRoomSessionLevelUI_C'") };

	if (widget_class.Succeeded())
	{
		levelUiClass = widget_class.Class;
	}
	else
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[ASagaRoomSessionLevel][Ctor] Could not find the class of user interface for room session."));
	}

	static ConstructorHelpers::FClassFinder<USagaUiPopup> popup_class(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Components/SagaUiPopup.SagaUiPopup_C'"));

	if (popup_class.Succeeded())
	{
		errorPopupWidgetClass = popup_class.Class;
	}
	else
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[ASagaRoomSessionLevel][Ctor] Could not find the class of error message popup for room session."));
	}

	static ConstructorHelpers::FClassFinder<USagaUiInstancedUserData> userdata_class{ TEXT("/Game/UI/Level/RoomSessionLevel/SagaUiInstancedUserData.SagaUiInstancedUserData_C") };

	if (userdata_class.Succeeded())
	{
		userDataClass = userdata_class.Class;
	}
	else
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[ASagaRoomSessionLevel][ctor] Could not find the class of user data for room session."));
	}
}

void
ASagaRoomSessionLevel::UpdateRoomSessionUI()
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not net->IsOfflineMode())
	{
		const auto& list = net->GetUserList();

#if WITH_EDITOR

		UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel][UpdateRoomSessionUI] %d users would be listed."), list.Num());
#endif

		const auto& red_list = levelUiInstance->myRedTeamMemberListView->myListView;
		const auto& blu_list = levelUiInstance->myBlueTeamMemberListView->myListView;

		red_list->ClearListItems();
		blu_list->ClearListItems();

		for (auto& user : list)
		{
			const auto& user_id = user.myID;

			switch (user.myTeam)
			{
			case ESagaPlayerTeam::Unknown:
			{
#if WITH_EDITOR

				UE_LOG(LogSagaFramework, Error, TEXT("[ASagaRoomSessionLevel][UpdateRoomSessionUI] user %d has unkown team."), user_id);
#endif
			}
			break;

			case ESagaPlayerTeam::Red:
			{
				auto& team_list = *red_list;

#if WITH_EDITOR

				const auto name = user.myName.ToString();
				UE_LOG(LogSagaFramework, Log, TEXT("Member(%d) - '%s' | team: %d"), user_id, *name, static_cast<int>(user.myTeam));
#endif

				AddMemberToTeamViewer(&team_list, user);
			}
			break;

			case ESagaPlayerTeam::Blue:
			{
				auto& team_list = *blu_list;

#if WITH_EDITOR

				const auto name = user.myName.ToString();
				UE_LOG(LogSagaFramework, Log, TEXT("Member(%d) - '%s' | team: %d"), user_id, *name, static_cast<int>(user.myTeam));
#endif

				AddMemberToTeamViewer(&team_list, user);
			}
			break;

			default:
			{
				UE_LOG(LogSagaFramework, Fatal, TEXT("[ASagaRoomSessionLevel][UpdateRoomSessionUI] user %d has invalid team '%d'."), user_id, static_cast<int32>(user.myTeam));
			}
			break;
			}
		}

		red_list->ScrollToTop();
		blu_list->ScrollToTop();

		red_list->RequestRefresh();
		blu_list->RequestRefresh();
	}
}

void
ASagaRoomSessionLevel::BeginPlay()
{
	Super::BeginPlay();

	OnDestroyed.AddDynamic(this, &ASagaRoomSessionLevel::HandleDestroy);

	GetWorldTimerManager().SetTimer(teamUpdateTimer, this, &ASagaRoomSessionLevel::HandlePeriodicUpdate, 5.0f, true, 0.1f);

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	levelUiInstance = CreateWidget<USagaRoomSessionLevelUiWidget>(GetWorld(), levelUiClass);

	if (IsValid(levelUiInstance))
	{
#if WITH_EDITOR

		UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel][BeginPlay] Assigning ui callbacks..."));
#endif

		levelUiInstance->AddToViewport(1);
		levelUiInstance->OpenNow();

		levelUiInstance->OnClosed.AddDynamic(this, &ASagaRoomSessionLevel::HandleOnClose);

		levelUiInstance->myStartButton->OnClicked.AddDynamic(this, &ASagaRoomSessionLevel::HandleStartButton);

		auto& red_join_btn = levelUiInstance->myRedTeamMemberListView->myJoinButton;
		auto& blu_join_btn = levelUiInstance->myBlueTeamMemberListView->myJoinButton;

		//red_join_btn->OpenNow();
		//blu_join_btn->OpenNow();

#if WITH_EDITOR

		UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel][BeginPlay] Red joining button is '%s'."), *red_join_btn->GetName());
		UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel][BeginPlay] Blu joining button is '%s'."), *blu_join_btn->GetName());
#endif

		red_join_btn->OnClicked.AddDynamic(this, &ASagaRoomSessionLevel::HandleRedTeamButton);
		blu_join_btn->OnClicked.AddDynamic(this, &ASagaRoomSessionLevel::HandleBlueTeamButton);

		levelUiInstance->myQuitButton->OnClicked.AddDynamic(this, &ASagaRoomSessionLevel::HandleQuitButton);
	}
	else
	{
#if WITH_EDITOR

		UE_LOG(LogSagaFramework, Fatal, TEXT("[ASagaRoomSessionLevel][BeginPlay] Could not create user interface for lobby."));
#endif
	}

	net->OnDisconnected.AddDynamic(this, &ASagaRoomSessionLevel::OnDisconnected);
	net->OnJoinedRoom.AddDynamic(this, &ASagaRoomSessionLevel::OnJoinedRoom);
	net->OnLeftRoomBySelf.AddDynamic(this, &ASagaRoomSessionLevel::OnLeftRoomBySelf);
	net->OnLeftRoom.AddDynamic(this, &ASagaRoomSessionLevel::OnLeftRoom);
	net->OnUpdateMembers.AddDynamic(this, &ASagaRoomSessionLevel::OnUpdateUserList);
	net->OnTeamChanged.AddDynamic(this, &ASagaRoomSessionLevel::OnTeamChanged);
	net->OnFailedToStartGame.AddDynamic(this, &ASagaRoomSessionLevel::OnFailedToStartGame);
	net->OnGetPreparedGame.AddDynamic(this, &ASagaRoomSessionLevel::OnGetPreparedGame);
}

void
ASagaRoomSessionLevel::BeginDestroy()
{
	/*
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());
	net->OnDisconnected.RemoveDynamic(this, &ASagaRoomSessionLevel::OnDisconnected);
	net->OnJoinedRoom.RemoveDynamic(this, &ASagaRoomSessionLevel::OnJoinedRoom);
	net->OnLeftRoomBySelf.RemoveDynamic(this, &ASagaRoomSessionLevel::OnLeftRoomBySelf);
	net->OnLeftRoom.RemoveDynamic(this, &ASagaRoomSessionLevel::OnLeftRoom);
	net->OnUpdateMembers.RemoveDynamic(this, &ASagaRoomSessionLevel::OnUpdateUserList);
	net->OnTeamChanged.RemoveDynamic(this, &ASagaRoomSessionLevel::OnTeamChanged);
	net->OnFailedToStartGame.RemoveDynamic(this, &ASagaRoomSessionLevel::OnFailedToStartGame);
	net->OnGetPreparedGame.RemoveDynamic(this, &ASagaRoomSessionLevel::OnGetPreparedGame);
	*/

	Super::BeginDestroy();
}

void
ASagaRoomSessionLevel::OnDisconnected()
{
	Fallback();
}

void
ASagaRoomSessionLevel::OnJoinedRoom(int32 user_id)
{
	PauseTimer();

	UpdateRoomSessionUI();

	UnPauseTimer();
}

void
ASagaRoomSessionLevel::OnLeftRoomBySelf()
{
#if WITH_EDITOR

	UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel][OnLeftRoomBySelf] Local user has been left from the room."));
#endif

	GotoPrevLevel();
}

void
ASagaRoomSessionLevel::OnLeftRoom(int32 user_id)
{
	PauseTimer();

#if WITH_EDITOR

	UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel][OnLeftRoom] Remote user %d has been left from the room."), user_id);
#endif

	UpdateRoomSessionUI();

	UnPauseTimer();
}

void
ASagaRoomSessionLevel::OnUpdateUserList(const TArray<struct FSagaVirtualUser>& list)
{
	PauseTimer();

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	for (auto& user : list)
	{
		net->SetTeam(user.myID, user.myTeam);
	}

	UpdateRoomSessionUI();

	UnPauseTimer();
}

void
ASagaRoomSessionLevel::OnTeamChanged(int32 user_id, bool is_red_team)
{
	PauseTimer();

#if WITH_EDITOR

	const TCHAR* team_name = is_red_team ? TEXT("red") : TEXT("blue");

	UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel][OnTeamChanged] User %d changed team to %s team."), user_id, team_name);
#endif

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (is_red_team)
	{
		net->SetTeam(user_id, ESagaPlayerTeam::Red);
	}
	else
	{
		net->SetTeam(user_id, ESagaPlayerTeam::Blue);
	}

	UpdateRoomSessionUI();

	UnPauseTimer();
}

void
ASagaRoomSessionLevel::OnFailedToStartGame(ESagaGameContract reason)
{
	PauseTimer();

	errorPopupWidgetInstance = CreateWidget<USagaUiPopup>(GetWorld(), errorPopupWidgetClass);

	UnPauseTimer();
}

void
ASagaRoomSessionLevel::OnGetPreparedGame()
{
	PauseTimer();

#if WITH_EDITOR

	UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel][OnGetPreparedGame] Lets prepare for game!"));
#endif

	GotoNextLevel();

	UnPauseTimer();
}

void
ASagaRoomSessionLevel::HandleOnClose()
{
	GotoPrevLevel();
}

void
ASagaRoomSessionLevel::HandleStartButton()
{
	PauseTimer();

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not net->IsOfflineMode())
	{
		if (net->IsConnected())
		{
#if WITH_EDITOR

			UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] HandleStartButton"));
#endif

			net->SendGameStartPacket();
		}
		else
		{
#if WITH_EDITOR

			UE_LOG(LogSagaFramework, Error, TEXT("[ASagaRoomSessionLevel][HandleStartButton] Network subsystem is not ready."));
#endif

			Fallback();
		}
	}
	else
	{
#if WITH_EDITOR

		UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] HandleStartButton (Offline Mode)"));
#endif

		GotoNextLevel();
	}

	UnPauseTimer();
}

void
ASagaRoomSessionLevel::HandleQuitButton()
{
	PauseTimer();

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not net->IsOfflineMode())
	{
		if (net->IsConnected())
		{
#if WITH_EDITOR

			UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] HandleQuitButton"));
#endif

			net->SendLeaveRoomPacket();

			levelUiInstance->Close();
		}
		else
		{
#if WITH_EDITOR

			UE_LOG(LogSagaFramework, Warning, TEXT("[ASagaRoomSessionLevel][HandleQuitButton] Network subsystem is not ready."));
#endif

			Fallback();
		}
	}
	else
	{
#if WITH_EDITOR

		UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] HandleQuitButton (Offline Mode)"));
#endif

		levelUiInstance->Close();
	}
}

void
ASagaRoomSessionLevel::HandleRedTeamButton()
{
	PauseTimer();

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (net->IsOfflineMode())
	{
#if WITH_EDITOR

		UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] HandleRedTeamButton (Offline Mode)"));
#endif

		net->SetTeam(net->GetLocalUserId(), ESagaPlayerTeam::Red);
	}
	else
	{
		if (net->IsConnected())
		{
#if WITH_EDITOR

			UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] Changing team to red..."));
#endif

			net->SendChangeTeamPacket(true);
		}
		else
		{
#if WITH_EDITOR

			UE_LOG(LogSagaFramework, Error, TEXT("[ASagaRoomSessionLevel][HandleRedTeamButton] Network subsystem is not ready."));
#endif

			Fallback();
		}
	}

	UnPauseTimer();
}

void
ASagaRoomSessionLevel::HandleBlueTeamButton()
{
	PauseTimer();

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (net->IsOfflineMode())
	{
#if WITH_EDITOR

		UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] HandleBlueTeamButton (Offline Mode)"));
#endif

		net->SetTeam(net->GetLocalUserId(), ESagaPlayerTeam::Blue);
	}
	else
	{
		if (net->IsConnected())
		{
#if WITH_EDITOR

			UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] Changing team to blue..."));
#endif

			net->SendChangeTeamPacket(false);
		}
		else
		{
#if WITH_EDITOR

			UE_LOG(LogSagaFramework, Error, TEXT("[ASagaRoomSessionLevel][HandleBlueTeamButton] Network subsystem is not ready."));
#endif

			Fallback();
		}
	}

	UnPauseTimer();
}

void
ASagaRoomSessionLevel::HandlePeriodicUpdate()
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not net->IsOfflineMode())
	{
		if (net->IsConnected())
		{
#if WITH_EDITOR

			UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] HandlePeriodicUpdate"));
#endif

			net->SendRequestMembersPacket();
		}
		else
		{
#if WITH_EDITOR

			UE_LOG(LogSagaFramework, Error, TEXT("[ASagaRoomSessionLevel][HandlePeriodicUpdate] Network subsystem is not ready."));
#endif
		}
	}
	else
	{
#if WITH_EDITOR

		UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] HandlePeriodicUpdate (Offline Mode)"));
#endif
	}
}

void
ASagaRoomSessionLevel::HandleDestroy(AActor* actor)
{
	GetWorldTimerManager().ClearAllTimersForObject(actor);

	teamUpdateTimer.Invalidate();
}

void
ASagaRoomSessionLevel::Fallback()
{
	if (IsValid(levelUiInstance))
	{
		levelUiInstance->Close();
	}
	else
	{
		SetPrevLevelName(TEXT("MainMenuLevel"));
		GotoPrevLevel();
	}
}

void
ASagaRoomSessionLevel::PauseTimer()
{
	GetWorldTimerManager().PauseTimer(teamUpdateTimer);
}

void

ASagaRoomSessionLevel::UnPauseTimer()
{
	GetWorldTimerManager().UnPauseTimer(teamUpdateTimer);
}

void
ASagaRoomSessionLevel::AddMemberToTeamViewer(UListView* listview, const FSagaVirtualUser& user)
{
	auto data_representation = NewObject<USagaUiInstancedUserData>(GetWorld(), userDataClass);

	data_representation->virtualMember = user;

	listview->AddItem(data_representation);
}
