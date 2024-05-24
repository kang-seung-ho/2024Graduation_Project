#include "Saga/Level/SagaLobbyLevel.h"
#include <UObject/ConstructorHelpers.h>
#include <Delegates/Delegate.h>
#include <Containers/Array.h>
#include <Components/ListView.h>

#include "Saga/Interface/SagaLobbyLevelUiWidget.h"
#include "Saga/Interface/Data/SagaUiInstancedRoomData.h"

#include "Saga/Network/SagaVirtualRoom.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaLobbyLevel::ASagaLobbyLevel()
noexcept
	: Super()
	, levelUiClass(), levelUiInstance()
	, roomDataClass()
{
	SetNextLevelName(TEXT("RoomSessionLevel"));
	SetPrevLevelName(TEXT("MainMenuLevel"));

	static ConstructorHelpers::FClassFinder<USagaLobbyLevelUiWidget> widget_class{ TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Level/SagaLobbyLevelUI.SagaLobbyLevelUI_C'") };

	if (widget_class.Succeeded())
	{
		levelUiClass = widget_class.Class;
	}
	else
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[ASagaLobbyLevel][Ctor] Could not find the class of user interface for lobby."));
	}

	static ConstructorHelpers::FClassFinder<USagaUiInstancedRoomData> roomdata_class{ TEXT("/Game/UI/Level/LobbyLevel/SagaUiInstancedRoomData.SagaUiInstancedRoomData_C") };

	if (roomdata_class.Succeeded())
	{
		roomDataClass = roomdata_class.Class;
	}
	else
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[ASagaLobbyLevel][Ctor] Could not find the class of room data for lobby."));
	}
}

void
ASagaLobbyLevel::BeginPlay()
{
	Super::BeginPlay();

	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	levelUiInstance = CreateWidget<USagaLobbyLevelUiWidget>(GetWorld(), levelUiClass);
	if (IsValid(levelUiInstance))
	{
		levelUiInstance->AddToViewport(1);
		levelUiInstance->OpenNow();

		if (not system->IsConnected() or system->IsOfflineMode())
		{
			levelUiInstance->OnClosed.AddUniqueDynamic(this, &ASagaLobbyLevel::HandleOnClose);
		}
	}
	else
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[ASagaLobbyLevel][BeginPlay] Could not create user interface for lobby."));
	}

	system->OnDisconnected.AddDynamic(this, &ASagaLobbyLevel::OnDisconnected);
	system->OnRoomCreated.AddDynamic(this, &ASagaLobbyLevel::OnRoomCreated);
	system->OnJoinedRoom.AddDynamic(this, &ASagaLobbyLevel::OnJoinedRoom);
	system->OnUpdateRoomList.AddDynamic(this, &ASagaLobbyLevel::OnUpdateRoomList);
}

void
ASagaLobbyLevel::HandleOnClose()
{
	GotoPrevLevel();
}

void
ASagaLobbyLevel::OnDisconnected()
{
	GotoPrevLevel();
}

void
ASagaLobbyLevel::OnRoomCreated(int32 room_id)
{
	UE_LOG(LogSagaFramework, Log, TEXT("[ASagaLobbyLevel][OnRoomCreated] Room ID: %d"), room_id);

	GotoNextLevel();
}

void
ASagaLobbyLevel::OnJoinedRoom(int32 room_id)
{
	UE_LOG(LogSagaFramework, Log, TEXT("[ASagaLobbyLevel][OnJoinedRoom] Room ID: %d"), room_id);

	GotoNextLevel();
}

void
ASagaLobbyLevel::OnUpdateRoomList(const TArray<struct FSagaVirtualRoom>& list)
{
	UE_LOG(LogSagaFramework, Log, TEXT("[ASagaLobbyLevel][OnJoinedRoom] %d rooms would be generated,"), list.Num());

	auto& listview = levelUiInstance->myListView;

	listview->ClearListItems();

	for (auto& room_place : list)
	{
		auto data_representation = NewObject<USagaUiInstancedRoomData>(GetWorld(), roomDataClass);

		data_representation->virtualRoom = room_place;

		listview->AddItem(data_representation);
	}

	listview->ScrollToTop();
}
