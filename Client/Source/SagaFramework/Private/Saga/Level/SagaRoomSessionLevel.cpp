#include "Saga/Level/SagaRoomSessionLevel.h"
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
		UE_LOG(LogSagaFramework, Error, TEXT("[ASagaRoomSessionLevel][Ctor] Could not find the class of user interface for room session."));
	}

	static ConstructorHelpers::FClassFinder<USagaUiPopup> popup_class(TEXT("/Game/UI/Components/SagaUiPopup.SagaUiPopup_C"));

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
	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not system->IsOfflineMode())
	{
		const auto& list = system->GetUserList();

		UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel][UpdateRoomSessionUI] %d users would be listed."), list.Num());

		const auto& red_list = levelUiInstance->myRedTeamMemberListView->myListView;
		const auto& blu_list = levelUiInstance->myBlueTeamMemberListView->myListView;

		red_list->ClearListItems();
		blu_list->ClearListItems();

		for (auto& user : list)
		{
			switch (user.myTeam)
			{
			case EUserTeam::Unknown:
			{
				UE_LOG(LogSagaFramework, Error, TEXT("[ASagaRoomSessionLevel][UpdateRoomSessionUI] user %d has unkown team."), user.MyID);
			}
			break;
			
			case EUserTeam::Red:
			{
				auto& team_list = *red_list;

				AddMemberToTeamViewer(&team_list, user);
			}
			break;
			
			case EUserTeam::Blue:
			{
				auto& team_list = *blu_list;

				AddMemberToTeamViewer(&team_list, user);
			}
			break;

			default:
			{
				UE_LOG(LogSagaFramework, Fatal, TEXT("[ASagaRoomSessionLevel][UpdateRoomSessionUI] user %d has invalid team '%d'."), user.MyID, static_cast<int32>(user.myTeam));
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

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	levelUiInstance = CreateWidget<USagaRoomSessionLevelUiWidget>(GetWorld(), levelUiClass);
	if (IsValid(levelUiInstance))
	{
		levelUiInstance->AddToViewport(1);
		levelUiInstance->OpenNow();

		if (not system->IsConnected() or system->IsOfflineMode())
		{
			levelUiInstance->OnClosed.AddUniqueDynamic(this, &ASagaRoomSessionLevel::HandleOnClose);
		}
		else
		{
			levelUiInstance->myStartButton->OnClicked.AddUniqueDynamic(this, &ASagaRoomSessionLevel::HandleStartButton);

			levelUiInstance->myRedTeamMemberListView->myJoinButton->OnClicked.AddUniqueDynamic(this, &ASagaRoomSessionLevel::HandleRedTeamButton);
			levelUiInstance->myBlueTeamMemberListView->myJoinButton->OnClicked.AddUniqueDynamic(this, &ASagaRoomSessionLevel::HandleBlueTeamButton);
		}

		levelUiInstance->myQuitButton->OnClicked.AddUniqueDynamic(this, &ASagaRoomSessionLevel::HandleQuitButton);
	}
	else
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[ASagaRoomSessionLevel][BeginPlay] Could not create user interface for lobby."));
	}

	system->OnDisconnected.AddDynamic(this, &ASagaRoomSessionLevel::OnDisconnected);
	system->OnJoinedRoom.AddDynamic(this, &ASagaRoomSessionLevel::OnJoinedRoom);
	system->OnLeftRoomBySelf.AddDynamic(this, &ASagaRoomSessionLevel::OnLeftRoomBySelf);
	system->OnLeftRoom.AddDynamic(this, &ASagaRoomSessionLevel::OnLeftRoom);
	system->OnUpdateMembers.AddDynamic(this, &ASagaRoomSessionLevel::OnUpdateUserList);
	system->OnTeamChanged.AddDynamic(this, &ASagaRoomSessionLevel::OnTeamChanged);
	system->OnFailedToStartGame.AddDynamic(this, &ASagaRoomSessionLevel::OnFailedToStartGame);
	system->OnGetPreparedGame.AddDynamic(this, &ASagaRoomSessionLevel::OnGetPreparedGame);
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
	UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel][OnLeftRoomBySelf] Local user has been left from the room."));

	GotoPrevLevel();
}

void
ASagaRoomSessionLevel::OnLeftRoom(int32 user_id)
{
	PauseTimer();
	UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel][OnLeftRoom] Remote user %d has been left from the room."), user_id);

	UpdateRoomSessionUI();

	UnPauseTimer();
}

void
ASagaRoomSessionLevel::OnUpdateUserList(const TArray<struct FSagaVirtualUser>& list)
{
	PauseTimer();
	UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel][OnUpdateUserList] %d users are listed."), list.Num());

	UpdateRoomSessionUI();

	UnPauseTimer();
}

void
ASagaRoomSessionLevel::OnTeamChanged(int32 user_id, bool is_red_team)
{
	PauseTimer();

	const TCHAR* team_name = is_red_team ? TEXT("Red") : TEXT("Blue");
	UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel][OnLeftRoom] User %d changed team to %s team."), user_id, team_name);

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
	UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel][OnGetPreparedGame] Lets prepare for game!"));

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

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not system->IsOfflineMode())
	{
		if (system->IsConnected())
		{
			UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] HandleStartButton"));

			system->SendGameStartPacket();
		}
		else
		{
			UE_LOG(LogSagaFramework, Error, TEXT("[ASagaRoomSessionLevel][HandleStartButton] Network subsystem is not ready."));

			Fallback();
		}
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] HandleStartButton (Offline Mode)"));
	}

	UnPauseTimer();
}

void
ASagaRoomSessionLevel::HandleQuitButton()
{
	PauseTimer();

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not system->IsOfflineMode())
	{
		if (system->IsConnected())
		{
			UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] HandleQuitButton"));

			system->SendLeaveRoomPacket();

			levelUiInstance->Close();
		}
		else
		{
			UE_LOG(LogSagaFramework, Warning, TEXT("[ASagaRoomSessionLevel][HandleQuitButton] Network subsystem is not ready."));

			Fallback();
		}
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] HandleQuitButton (Offline Mode)"));

		levelUiInstance->Close();
	}
}

void
ASagaRoomSessionLevel::HandleRedTeamButton()
{
	PauseTimer();

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not system->IsOfflineMode())
	{
		if (system->IsConnected())
		{
			UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] Changing team to red..."));

			system->SendChangeTeamPacket(true);
		}
		else
		{
			UE_LOG(LogSagaFramework, Error, TEXT("[ASagaRoomSessionLevel][HandleRedTeamButton] Network subsystem is not ready."));

			Fallback();
		}
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] HandleRedTeamButton (Offline Mode)"));

		system->SetTeam(0, EUserTeam::Red);
	}

	UnPauseTimer();
}

void
ASagaRoomSessionLevel::HandleBlueTeamButton()
{
	PauseTimer();

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not system->IsOfflineMode())
	{
		if (system->IsConnected())
		{
			UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] Changing team to blue..."));

			system->SendChangeTeamPacket(false);
		}
		else
		{
			UE_LOG(LogSagaFramework, Error, TEXT("[ASagaRoomSessionLevel][HandleBlueTeamButton] Network subsystem is not ready."));

			Fallback();
		}
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] HandleBlueTeamButton (Offline Mode)"));

		system->SetTeam(0, EUserTeam::Blue);
	}

	UnPauseTimer();
}

void
ASagaRoomSessionLevel::HandlePeriodicUpdate()
{
	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not system->IsOfflineMode())
	{
		if (system->IsConnected())
		{
			UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] HandlePeriodicUpdate"));

			system->SendRequestMembersPacket();
		}
		else
		{
			UE_LOG(LogSagaFramework, Error, TEXT("[ASagaRoomSessionLevel][HandlePeriodicUpdate] Network subsystem is not ready."));

			// NOTICE: HandlePeriodicUpdate - Fallback
			//Fallback();
		}
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[ASagaRoomSessionLevel] HandlePeriodicUpdate (Offline Mode)"));
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
	SetPrevLevelName(TEXT("MainMenuLevel"));
	levelUiInstance->Close();
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
