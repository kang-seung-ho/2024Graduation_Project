#include "Saga/Network/SagaNetworkSubSystem.h"
#include "Sockets.h"
#include "GameFramework/PlayerController.h"

#include "Saga/Network/SagaNetworkSettings.h"
#include "Saga/Network/SagaNetworkUtility.h"
#include "Character/SagaCharacterPlayer.h"
#include "Character/SagaInGamePlayerController.h"
#include "Player/SagaUserTeam.h"

[[nodiscard]] TSharedRef<FInternetAddr> CreateRemoteEndPoint();

bool
USagaNetworkSubSystem::InitializeNetwork_Implementation()
{
	if (nullptr != clientSocket)
	{
		return true;
	}

	clientSocket = saga::CreateTcpSocket();
	if (nullptr == clientSocket)
	{
		return false;
	}

	// NOTICE: 클라는 바인드 금지
	//auto local_endpoint = saga::MakeEndPoint(FIPv4Address::InternalLoopback, saga::GetLocalPort());
	//if (not clientSocket->Bind(*local_endpoint))
	//{
	//	return false;
	//}

	if (not clientSocket->SetReuseAddr())
	{
		return false;
	}

	if (not clientSocket->SetNoDelay())
	{
		return false;
	}

	return true;
}

ESagaConnectionContract
USagaNetworkSubSystem::ConnectToServer_Implementation()
{
	if (not IsSocketAvailable())
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("The socket is not available."));
		return ESagaConnectionContract::NoSocket;
	}

	// #1 연결 부분
	if constexpr (not saga::IsOfflineMode)
	{
		auto remote_endpoint = CreateRemoteEndPoint();
		if (not remote_endpoint->IsValid())
		{
			auto err_msg = saga::GetLastErrorContents();
			UE_LOG(LogSagaNetwork, Error, TEXT("The endpoint has fault, due to '%s'"), *err_msg);

			return ESagaConnectionContract::WrongAddress;
		}

		if (not clientSocket->Connect(*remote_endpoint))
		{
			// 연결 실패 처리
			auto err_msg = saga::GetLastErrorContents();
			UE_LOG(LogSagaNetwork, Error, TEXT("Cannot connect to the server, due to '%s'"), *err_msg);

			return ESagaConnectionContract::OtherError;
		}
	}

	// #2
	netWorker = MakeUnique<FSagaNetworkWorker>(this);
	if (netWorker == nullptr)
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("Has failed to create the worker thread."));
		return ESagaConnectionContract::CannotCreateWorker;
	}

	if constexpr (not saga::IsOfflineMode)
	{
		// #3
		// 클라는 접속 이후에 닉네임 패킷을 보내야 한다.

		auto sent_r = SendSignInPacket(localUserName);
		if (sent_r <= 0)
		{
			auto err_msg = saga::GetLastErrorContents();
			UE_LOG(LogSagaNetwork, Error, TEXT("First try of sending signin packet has been failed, due to '%s'"), *err_msg);

			return ESagaConnectionContract::SignInFailed;
		}
		else
		{
			UE_LOG(LogSagaNetwork, Log, TEXT("User's nickname is %s."), *localUserName);
		}
	}

	return ESagaConnectionContract::Success;
}

bool
USagaNetworkSubSystem::CloseNetwork_Implementation()
{
	//clientSocket->Shutdown(ESocketShutdownMode::ReadWrite);

	return std::exchange(clientSocket, nullptr)->Close();
}

void
USagaNetworkSubSystem::OnNetworkInitialized_Implementation(bool succeed)
{
	if (succeed)
	{
		UE_LOG(LogSagaNetwork, Log, TEXT("The network subsystem is initialized."));
	}
	else
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("Cannot initialize the network subsystem."));
	}
}

void
USagaNetworkSubSystem::OnConnected_Implementation()
{}

void
USagaNetworkSubSystem::OnFailedToConnect_Implementation(ESagaConnectionContract reason)
{
	auto msg = UEnum::GetValueAsString(reason);
	UE_LOG(LogSagaNetwork, Log, TEXT("Local client can't get an id from server due to %s."), *msg);
}

void
USagaNetworkSubSystem::OnDisconnected_Implementation()
{
	if (IsSocketAvailable())
	{
		(void)CloseNetwork_Implementation();
	}
}

void
USagaNetworkSubSystem::OnRoomCreated_Implementation(int32 id)
{
	UE_LOG(LogSagaNetwork, Log, TEXT("A room %d is created."), id);

	SendRequestMembersPacket();
}

void
USagaNetworkSubSystem::OnJoinedRoom_Implementation(int32 id)
{}

void
USagaNetworkSubSystem::OnLeftRoomBySelf_Implementation()
{
	UE_LOG(LogSagaNetwork, Log, TEXT("Local client has been left from room."));
}

void
USagaNetworkSubSystem::OnLeftRoom_Implementation(int32 id)
{
	UE_LOG(LogSagaNetwork, Log, TEXT("Remote client %d has been left from room."), id);
}

void
USagaNetworkSubSystem::OnRespondVersion_Implementation(const FString& version_string)
{
	UE_LOG(LogSagaNetwork, Log, TEXT("Version: %s"), *version_string);
}

void
USagaNetworkSubSystem::OnUpdateRoomList_Implementation(const TArray<FSagaVirtualRoom>& list)
{}

void
USagaNetworkSubSystem::OnUpdateMembers_Implementation(const TArray<FSagaVirtualUser>& list)
{}

void
USagaNetworkSubSystem::OnTeamChanged_Implementation(int32 user_id, bool is_red_team)
{
	for (auto& user : everyUsers)
	{
		if (user.MyID == user_id)
		{
			user.myTeam = is_red_team ? EUserTeam::Red : EUserTeam::Blue;
		}
	}
}

void
USagaNetworkSubSystem::OnGetPreparedGame_Implementation()
{}

void
USagaNetworkSubSystem::OnFailedToStartGame_Implementation(ESagaGameContract reason)
{
	auto name = UEnum::GetValueAsString(reason);

	UE_LOG(LogSagaNetwork, Error, TEXT("Cannot start the game, due to '%s'."), *name);
}

void
USagaNetworkSubSystem::OnStartGame_Implementation()
{
	const auto player = GEngine->FindFirstLocalPlayerFromControllerId(0);
	if (nullptr == player)
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][OnUpdatePosition] Cannot find a handle of the local player."));
		return;
	}

	const auto world = player->GetWorld();
	if (nullptr == world)
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][RPC] The handle of world is null."));
		return;
	}

	auto controller = player->GetPlayerController(world);
	if (nullptr == controller)
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][OnUpdatePosition] Cannot find the local player's controller."));
		return;
	}

	auto my_pawn = controller->GetPawn();
	if (nullptr == my_pawn)
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][OnUpdatePosition] Cannot find a pawn of the local player."));
		return;
	}

	UE_LOG(LogSagaNetwork, Log, TEXT("[SagaGame][OnUpdatePosition] System found the character of local player."));
	localPlayerCharacter = my_pawn;
}

void
USagaNetworkSubSystem::OnUpdatePosition_Implementation(int32 id, float x, float y, float z)
{
	if (id == GetLocalUserId()) // 로컬 클라이언트
	{
		//auto movement = localPlayerCharacter->GetMovementComponent();
		//localPlayerCharacter->SetActorLocation(FVector{ x, y, z });
	}
	else // 원격 클라이언트
	{
		FSagaVirtualUser user{};

		if (not FindUser(id, user))
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][RPC] Cannot find remote player %d."), id);
			return;
		}

		auto& character = user.remoteCharacter;
		if (not character)
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][RPC] Cannot find a character of remote player %d'."), id);
			return;
		}

		character->SetActorLocation(FVector{ x, y, z });
	}
}

void
USagaNetworkSubSystem::OnUpdateRotation_Implementation(int32 id, float p, float y, float r)
{
	if (id == GetLocalUserId()) // 로컬 클라이언트
	{
	}
	else // 원격 클라이언트
	{
		FSagaVirtualUser user{};

		if (not FindUser(id, user))
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][RPC] Cannot find remote player %d."), id);
			return;
		}

		auto& character = user.remoteCharacter;
		if (not character)
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][RPC] Cannot find a character of remote player %d'."), id);
			return;
		}

		character->SetActorRotation(FRotator{ p, y, r });
	}
}

TSharedRef<FInternetAddr>
CreateRemoteEndPoint()
{
	if constexpr (saga::ConnectionCategory == saga::SagaNetworkConnectionCategory::Local)
	{
		return saga::MakeEndPoint(FIPv4Address::Any, saga::RemotePort);
	}
	else if constexpr (saga::ConnectionCategory == saga::SagaNetworkConnectionCategory::Host)
	{
		return saga::MakeEndPoint(FIPv4Address::InternalLoopback, saga::RemotePort);
	}
	else if constexpr (saga::ConnectionCategory == saga::SagaNetworkConnectionCategory::Remote)
	{
		return saga::MakeEndPointFrom(saga::RemoteAddress, saga::RemotePort);
	}
	else
	{
		throw "error!";
	}
}
