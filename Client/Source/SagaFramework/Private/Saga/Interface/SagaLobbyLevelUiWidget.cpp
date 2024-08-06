#include "Saga/Interface/SagaLobbyLevelUiWidget.h"
#include <Components/Widget.h>
#include <Components/ListView.h>
#include <Templates/Casts.h>
#include <Blueprint/WidgetTree.h>

#include "Saga/Interface/SagaUiButton.h"
#include "Saga/Interface/Data/SagaUiInstancedRoomData.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

USagaLobbyLevelUiWidget::USagaLobbyLevelUiWidget(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, myListView()
	, myCreateButton(), myJoinButton(), myRefreshButton(), myQuitButton()
	, selectedRoom()
{}

void USagaLobbyLevelUiWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

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

		//myListView->Native
	}

	if (nullptr == myCreateButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaLobbyLevelUiWidget] '%s' could not found the creating a room button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaLobbyLevelUiWidget] '%s' found the creating a room button in children."), *my_name);

		myCreateButton->OnClicked.AddUniqueDynamic(this, &USagaLobbyLevelUiWidget::HandleCreatingRoomClick);
	}

	if (nullptr == myJoinButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaLobbyLevelUiWidget] '%s' could not found the joining to room button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaLobbyLevelUiWidget] '%s' found the joining to room button in children."), *my_name);

		myJoinButton->OnClicked.AddUniqueDynamic(this, &USagaLobbyLevelUiWidget::HandleTryJoinRoom);
	}

	if (nullptr == myRefreshButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaLobbyLevelUiWidget] '%s' could not found the refresh button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaLobbyLevelUiWidget] '%s' found the refresh button in children."), *my_name);

		myRefreshButton->OnClicked.AddUniqueDynamic(this, &USagaLobbyLevelUiWidget::HandleRefreshRoomList);
	}

	if (nullptr == myQuitButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaLobbyLevelUiWidget] '%s' could not found the quit button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaLobbyLevelUiWidget] '%s' found button in children."), *my_name);

		myQuitButton->OnClicked.AddUniqueDynamic(this, &USagaLobbyLevelUiWidget::HandleQuitButtonClick);
	}
}

void
USagaLobbyLevelUiWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void
USagaLobbyLevelUiWidget::NativeTick(const FGeometry& geometry, float delta_time)
{
	Super::NativeTick(geometry, delta_time);

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not system->IsOfflineMode())
	{
		const bool connection = system->IsConnected();

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
	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not system->IsOfflineMode())
	{
		if (system->IsConnected())
		{
			UE_LOG(LogSagaFramework, Log, TEXT("[USagaLobbyLevelUiWidget][HandleCreatingRoomClick] The client is trying to create a room."));

			//system->SendCreateRoomPacket();
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
	if (nullptr != selectedRoom)
	{
		const auto& room_data = selectedRoom->virtualRoom;

		auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		if (not system->IsOfflineMode())
		{
			if (system->IsConnected())
			{
				UE_LOG(LogSagaFramework, Log, TEXT("[USagaLobbyLevelUiWidget][HandleTryJoinRoom] The client is trying to join to room '%d'."), room_data.MyID);

				system->SendJoinRoomPacket(room_data.MyID);
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
	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not system->IsOfflineMode())
	{
		if (system->IsConnected())
		{
			system->SendRequestRoomsPacket();
		}
		else
		{
			UE_LOG(LogSagaFramework, Error, TEXT("[USagaLobbyLevelUiWidget][HandleRefreshRoomList] The client is not connected."));
		}
	}
}

void
USagaLobbyLevelUiWidget::HandleQuitButtonClick()
{
	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());
	Close();

	if (not system->IsOfflineMode())
	{
		system->Close();
	}
}
