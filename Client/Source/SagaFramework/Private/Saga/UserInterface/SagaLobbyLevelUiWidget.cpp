#include "Saga/Interface/SagaLobbyLevelUiWidget.h"
#include <UObject/Object.h>
#include <UObject/NameTypes.h>
#include <Internationalization/Text.h>
#include <Templates/Casts.h>
#include <Components/Widget.h>
#include <Components/ListView.h>
#include <Blueprint/WidgetTree.h>

#include "Saga/Interface/SagaUiButton.h"
#include "Saga/Interface/Data/SagaUiInstancedRoomData.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

USagaLobbyLevelUiWidget::USagaLobbyLevelUiWidget(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, myListView()
	, selectedRoom()
	, myCreateButton(), myJoinButton(), myRefreshButton(), myQuitButton()
{}

void
USagaLobbyLevelUiWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
			if (element == this) return;

			if (auto listview = Cast<UListView>(element))
			{
				// myListView in editor
			}
			else if (auto btn = Cast<USagaUiButton>(element))
			{
				const auto name = btn->GetName();

				if (name == TEXT("CreateRoomButton"))
				{
					// myCreateButton in editor
				}
				else if (name == TEXT("JoinRoomButton"))
				{
					// myJoinButton in editor
					btn->SetUserInteractable(IsJoinButtonEnabled());
				}
				else if (name == TEXT("RefreshRoomsButton"))
				{
					// myRefreshButton in editor
					btn->SetUserInteractable(IsRoomButtonEnabled());
				}
				else if (name == TEXT("QuitButton"))
				{
					// myQuitButton in editor
					btn->SetText(GetQuitButtonLabelText());
				}
			}
		}
	);
}

void
USagaLobbyLevelUiWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
			if (element == this) return;

			if (auto listview = Cast<UListView>(element))
			{
				myListView = listview;
			}
			else if (auto btn = Cast<USagaUiButton>(element))
			{
				const auto name = btn->GetName();

				if (name == TEXT("CreateRoomButton"))
				{
					myCreateButton = btn;
				}
				else if (name == TEXT("JoinRoomButton"))
				{
					myJoinButton = btn;
				}
				else if (name == TEXT("RefreshRoomsButton"))
				{
					myRefreshButton = btn;
				}
				else if (name == TEXT("QuitButton"))
				{
					myQuitButton = btn;
				}
			}
		}
	);

	const auto my_name = GetName();
	if (nullptr == myListView)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaLobbyLevelUiWidget] '%s' found no slate list view in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaLobbyLevelUiWidget] '%s' found slate list view in children."), *my_name);
	}

	if (nullptr == myCreateButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaLobbyLevelUiWidget] '%s' could not found the creating a room button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaLobbyLevelUiWidget] '%s' found the creating a room button in children."), *my_name);
	}

	if (nullptr == myJoinButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaLobbyLevelUiWidget] '%s' could not found the joining to room button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaLobbyLevelUiWidget] '%s' found the joining to room button in children."), *my_name);
	}

	if (nullptr == myRefreshButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaLobbyLevelUiWidget] '%s' could not found the refresh button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaLobbyLevelUiWidget] '%s' found the refresh button in children."), *my_name);
	}

	if (nullptr == myQuitButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaLobbyLevelUiWidget] '%s' could not found the quit button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaLobbyLevelUiWidget] '%s' found button in children."), *my_name);
	}

	myCreateButton->OnClicked.AddDynamic(this, &USagaLobbyLevelUiWidget::HandleCreatingRoomClick);

	myJoinButton->OnClicked.AddDynamic(this, &USagaLobbyLevelUiWidget::HandleTryJoinRoom);

	myRefreshButton->OnClicked.AddDynamic(this, &USagaLobbyLevelUiWidget::HandleRefreshRoomList);

	myQuitButton->OnClicked.AddDynamic(this, &USagaLobbyLevelUiWidget::HandleQuitButtonClick);
}

void
USagaLobbyLevelUiWidget::NativeTick(const FGeometry& geometry, float delta_time)
{
	Super::NativeTick(geometry, delta_time);

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not net->IsOfflineMode())
	{
		const bool connection = net->IsConnected();

		if (nullptr != myCreateButton)
		{
			myCreateButton->SetUserInteractable(connection);
		}

		if (nullptr != myJoinButton)
		{
			myJoinButton->SetUserInteractable(connection and selectedRoom != nullptr);
		}

		if (nullptr != myRefreshButton)
		{
			myRefreshButton->SetUserInteractable(connection);
		}
	}
}

void
USagaLobbyLevelUiWidget::HandleCreatingRoomClick()
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not net->IsOfflineMode())
	{
		if (net->IsConnected())
		{
			UE_LOG(LogSagaFramework, Log, TEXT("[USagaLobbyLevelUiWidget][HandleCreatingRoomClick] The client is trying to create a room."));

			//HandleCreateRoom();
		}
		else
		{
			UE_LOG(LogSagaFramework, Error, TEXT("[USagaLobbyLevelUiWidget][HandleCreatingRoomClick] The client is not connected."));
		}
	}
}

void
USagaLobbyLevelUiWidget::HandleTryJoinRoom()
{
	if (IsValid(selectedRoom))
	{
		const auto& room_data = selectedRoom->virtualRoom;

		const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		if (not net->IsOfflineMode())
		{
			if (net->IsConnected())
			{
				UE_LOG(LogSagaFramework, Log, TEXT("[USagaLobbyLevelUiWidget][HandleTryJoinRoom] The client is trying to join to room '%d'."), room_data.myID);

				net->SendJoinRoomPacket(room_data.myID);
			}
			else
			{
				UE_LOG(LogSagaFramework, Error, TEXT("[USagaLobbyLevelUiWidget][HandleTryJoinRoom] The client is not connected."));
			}
		}
	}
}

void
USagaLobbyLevelUiWidget::HandleRefreshRoomList()
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not net->IsOfflineMode())
	{
		if (net->IsConnected())
		{
			net->SendRequestRoomsPacket();
		}
		else
		{
			UE_LOG(LogSagaFramework, Error, TEXT("[USagaLobbyLevelUiWidget][HandleRefreshRoomList] The client is not connected."));
		}
	}
}

void
USagaLobbyLevelUiWidget::HandleCreateRoom(FText title)
{
	if (title.IsEmpty())
	{
		UE_LOG(LogSagaFramework, Warning, TEXT("[USagaLobbyLevelUiWidget][HandleCreateRoom] The title is empty."));

		return;
	}

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (net->IsOfflineMode())
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaLobbyLevelUiWidget][HandleCreateRoom] Creating a room... (Offline Mode)"));

		net->SetCurrentRoomId(22);
		net->SetCurrentRoomTitle(title);
	}
	else
	{
		if (net->IsConnected())
		{
			UE_LOG(LogSagaFramework, Log, TEXT("[USagaLobbyLevelUiWidget][HandleCreateRoom] Sending a room creation request..."));

			if (0 < net->SendCreateRoomPacket(title))
			{
				net->SetCurrentRoomTitle(title);
			}
		}
		else
		{
			UE_LOG(LogSagaFramework, Error, TEXT("[USagaLobbyLevelUiWidget][HandleCreateRoom] The client is not connected."));
		}
	}
}

void
USagaLobbyLevelUiWidget::HandleQuitButtonClick()
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());
	Close();

	if (not net->IsOfflineMode())
	{
		net->Close();
	}
}

FText
USagaLobbyLevelUiWidget::GetQuitButtonLabelText()
const
{
	return AcquireQuitButtonLabel();
}

FText
USagaLobbyLevelUiWidget::GetUserNicknameLabelText()
const
{
	return AcquireUserNickname();
}

FText
USagaLobbyLevelUiWidget::GetUserIdLabelText()
const
{
	return FText::AsNumber(AcquireUserId());
}

bool
USagaLobbyLevelUiWidget::IsRoomButtonEnabled()
const
{
	return AcquireButtonsInteractableFlag();
}

bool
USagaLobbyLevelUiWidget::IsJoinButtonEnabled()
const
{
	return AcquireButtonsInteractableFlag() and IsValid(selectedRoom);
}

FText
USagaLobbyLevelUiWidget::GetNotConnectedAlertLabelText()
const
{
	return AcquireNotConnectedAlertLabelText();
}

ESlateVisibility
USagaLobbyLevelUiWidget::IsNotConnectedAlertLabelVisibility()
const
{
	return AcquireNotConnectedAlertLabelVisibility();
}

FText
USagaLobbyLevelUiWidget::AcquireUserNickname()
const
{
	static FText unknown = FText::FromString(TEXT("Unknown"));

	const auto world = GetWorld();
	if (nullptr == world)
	{
		return unknown;
	}

	const auto singleton = world->GetGameInstance();
	if (nullptr == singleton)
	{
		return unknown;
	}

	const auto net = singleton->GetSubsystem<USagaNetworkSubSystem>();
	if (nullptr == net)
	{
		return unknown;
	}
	else
	{
		return net->GetLocalUserName();
	}
}

int32
USagaLobbyLevelUiWidget::AcquireUserId()
const
{
	const auto world = GetWorld();
	if (nullptr == world)
	{
		return -1;
	}

	const auto singleton = world->GetGameInstance();
	if (nullptr == singleton)
	{
		return -1;
	}

	const auto net = singleton->GetSubsystem<USagaNetworkSubSystem>();
	if (nullptr == net)
	{
		return -1;
	}
	else
	{
		return net->GetLocalUserId();
	}
}

bool
USagaLobbyLevelUiWidget::AcquireButtonsInteractableFlag()
const
{
	const auto world = GetWorld();
	if (nullptr == world)
	{
		return false;
	}

	const auto singleton = world->GetGameInstance();
	if (nullptr == singleton)
	{
		return false;
	}

	const auto net = singleton->GetSubsystem<USagaNetworkSubSystem>();
	if (nullptr == net)
	{
		return false;
	}
	else
	{
		return not net->IsOfflineMode() and net->IsConnected();
	}
}

FText
USagaLobbyLevelUiWidget::AcquireQuitButtonLabel()
const
{
	static auto back_text = FText::FromString(TEXT("Back"));
	static auto quit_text = FText::FromString(TEXT("Quit"));

	const auto world = GetWorld();
	if (nullptr == world)
	{
		return back_text;
	}

	const auto singleton = world->GetGameInstance();
	if (nullptr == singleton)
	{
		return back_text;
	}

	const auto net = singleton->GetSubsystem<USagaNetworkSubSystem>();
	if (nullptr == net)
	{
		return back_text;
	}
	else if (net->IsOfflineMode())
	{
		return back_text;
	}
	else if (net->IsConnected())
	{
		return quit_text;
	}
	else
	{
		return back_text;
	}
}

FText
USagaLobbyLevelUiWidget::AcquireNotConnectedAlertLabelText()
const
{
	static auto err_text = FText::FromString(TEXT("Error"));

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
	else if (net->IsOfflineMode())
	{
		return FText::FromString(TEXT("오프라인 모드"));
	}
	else if (net->IsConnected())
	{
		return FText::GetEmpty();
	}
	else
	{
		return FText::FromString(TEXT("서버에 연결되지 않았습니다"));
	}
}

ESlateVisibility
USagaLobbyLevelUiWidget::AcquireNotConnectedAlertLabelVisibility()
const
{
	const auto world = GetWorld();
	if (nullptr == world)
	{
		return ESlateVisibility::Collapsed;
	}

	const auto singleton = world->GetGameInstance();
	if (nullptr == singleton)
	{
		return ESlateVisibility::Collapsed;
	}

	const auto net = singleton->GetSubsystem<USagaNetworkSubSystem>();
	if (nullptr == net)
	{
		return ESlateVisibility::Collapsed;
	}
	else if (net->IsOfflineMode())
	{
		return ESlateVisibility::Visible;
	}
	else if (net->IsConnected())
	{
		return ESlateVisibility::Collapsed;
	}
	else
	{
		return ESlateVisibility::Visible;
	}
}
