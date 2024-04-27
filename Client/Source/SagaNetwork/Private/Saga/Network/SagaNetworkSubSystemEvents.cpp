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
	UE_LOG(LogSagaNetwork, Log, TEXT("Local client can't get an id from server due to %s"), *msg);
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
	UE_LOG(LogSagaNetwork, Log, TEXT("A room %d is created"), id);

	SendRequestMembersPacket();
}

void
USagaNetworkSubSystem::OnJoinedRoom_Implementation(int32 id)
{}

void
USagaNetworkSubSystem::OnLeftRoomBySelf_Implementation()
{
	UE_LOG(LogSagaNetwork, Log, TEXT("Local client has been left from room"));
}

void
USagaNetworkSubSystem::OnLeftRoom_Implementation(int32 id)
{
	UE_LOG(LogSagaNetwork, Log, TEXT("Remote client %d has been left from room"), id);
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

	UE_LOG(LogSagaNetwork, Error, TEXT("Cannot start the game, due to '%s'"), *name);
}

void
USagaNetworkSubSystem::OnStartGame_Implementation()
{
	// TODO: OnStartGame_Implementation
}

void
USagaNetworkSubSystem::OnUpdatePosition_Implementation(int32 id, float x, float y, float z)
{
	// TODO: OnUpdatePosition_Implementation
}

void
USagaNetworkSubSystem::OnUpdateRotation_Implementation(int32 id, float r, float y, float p)
{
	// TODO: OnUpdateRotation_Implementation
}

void
USagaNetworkSubSystem::OnRpc_Implementation(ESagaRpcProtocol cat, int32 id, int64 arg0, int32 arg1)
{
	FSagaVirtualUser user{};

	if (not FindUser(id, user))
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][RPC] Cannot find user %d"), id);
		return;
	}

	bool is_local = false;
	if (id == GetLocalUserId()) // 로컬 클라이언트
	{
		is_local = true;
		UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][RPC] This is my rpc message"));

		auto& controller = user.localController;
		auto pawn = controller->GetPawn();
		auto character = Cast<ASagaCharacterPlayer>(pawn);
		if (not character)
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][RPC] Cannot find local user %d's character"), id);
			return;
		}

		switch (cat)
		{
		case ESagaRpcProtocol::RPC_UNKNOWN:
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][RPC] Cannot run rpc script by user %d"), id);
		}
		break;

		case ESagaRpcProtocol::RPC_BEG_WALK:
		{
			/*if (controller)
			{
				controller->ExecuteWalk();
			}*/
		}
		break;

		case ESagaRpcProtocol::RPC_END_WALK:
		{
			/*if (controller)
			{
				controller->TerminateWalk();
			}*/
		}
		break;

		case ESagaRpcProtocol::RPC_BEG_RUN:
		{
			if (controller)
			{
				//controller->ExecuteRun();
			}
		}
		break;

		case ESagaRpcProtocol::RPC_END_RUN:
		{
			if (controller)
			{
				//controller->TerminateRun();
			}
		}
		break;

		case ESagaRpcProtocol::RPC_BEG_JUMP:
		{
			if (controller)
			{
				//controller->ExecuteJump();
			}
		}
		break;

		// 수호자 탑승
		case ESagaRpcProtocol::RPC_BEG_RIDE:
		{
			if (controller)
			{
				//controller->ExecuteRide();
			}
		}
		break;

		// 수호자 하차
		case ESagaRpcProtocol::RPC_END_RIDE:
		{
			if (controller)
			{
				//controller->TerminateRide();
			}
		}
		break;

		case ESagaRpcProtocol::RPC_BEG_ATTACK_0:
		{
			if (controller)
			{
				//controller->ExecuteAttack();
			}
		}
		break;

		case ESagaRpcProtocol::RPC_END_ATTACK_0:
		{
			if (controller)
			{
				//controller->TerminateAttack();
			}
		}
		break;

		case ESagaRpcProtocol::RPC_BEG_ATTACK_1:
		{
			if (controller)
			{
				//controller->ExecuteAttack();
			}
		}
		break;

		case ESagaRpcProtocol::RPC_END_ATTACK_1:
		{
			if (controller)
			{
				//controller->TerminateAttack();
			}
		}
		break;

		case ESagaRpcProtocol::RPC_BEG_ATTACK_2:
		{}
		break;

		case ESagaRpcProtocol::RPC_BEG_ATTACK_3:
		{}
		break;

		case ESagaRpcProtocol::RPC_SKILL_0:
		{}
		break;

		case ESagaRpcProtocol::RPC_SKILL_1:
		{}
		break;

		case ESagaRpcProtocol::RPC_SKILL_2:
		{}
		break;

		case ESagaRpcProtocol::RPC_SKILL_3:
		{}
		break;

		case ESagaRpcProtocol::RPC_USE_ITEM_0:
		{}
		break;

		case ESagaRpcProtocol::RPC_USE_ITEM_1:
		{}
		break;

		case ESagaRpcProtocol::RPC_USE_ITEM_2:
		{}
		break;

		case ESagaRpcProtocol::RPC_USE_ITEM_3:
		{}
		break;

		case ESagaRpcProtocol::RPC_USE_ITEM_4:
		{}
		break;

		case ESagaRpcProtocol::RPC_CHANGE_HAND_ITEM:
		{}
		break;

		case ESagaRpcProtocol::RPC_DEAD:
		{}
		break;

		default:
			break;
		}
	}
	else // 원격 클라이언트
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][RPC] This is user %s's rpc message"), id);


		auto& character = user.remoteCharacter;
		switch (cat)
		{
		case ESagaRpcProtocol::RPC_UNKNOWN:
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][RPC] Cannot rpc script by user %d"), id);
		}
		break;

		case ESagaRpcProtocol::RPC_BEG_WALK:
		{
			if (character)
			{
				//character->ExecuteWalk();
			}
		}
		break;

		case ESagaRpcProtocol::RPC_END_WALK:
		{
			if (character)
			{
				//character->TerminateWalk();
			}
		}
		break;

		case ESagaRpcProtocol::RPC_BEG_RUN:
		{
			if (character)
			{
				//character->ExecuteRun();
			}
		}
		break;

		case ESagaRpcProtocol::RPC_END_RUN:
		{
			if (character)
			{
				//character->TerminateRun();
			}
		}
		break;

		case ESagaRpcProtocol::RPC_BEG_JUMP:
		{
			if (character)
			{
				//character->ExecuteJump();
			}
		}
		break;

		// 수호자 탑승
		case ESagaRpcProtocol::RPC_BEG_RIDE:
		{
			if (character)
			{
				//character->ExecuteRide();
			}
		}
		break;

		// 수호자 하차
		case ESagaRpcProtocol::RPC_END_RIDE:
		{
			if (character)
			{
				//character->TerminateRide();
			}
		}
		break;

		case ESagaRpcProtocol::RPC_BEG_ATTACK_0:
		{
			if (character)
			{
				//character->ExecuteAttack();
			}
		}
		break;

		case ESagaRpcProtocol::RPC_END_ATTACK_0:
		{
			if (character)
			{
				//character->TerminateAttack();
			}
		}
		break;

		case ESagaRpcProtocol::RPC_BEG_ATTACK_1:
		{
			if (character)
			{
				//character->ExecuteAttack();
			}
		}
		break;

		case ESagaRpcProtocol::RPC_END_ATTACK_1:
		{
			if (character)
			{
				//character->TerminateAttack();
			}
		}
		break;

		case ESagaRpcProtocol::RPC_BEG_ATTACK_2:
		{}
		break;

		case ESagaRpcProtocol::RPC_BEG_ATTACK_3:
		{}
		break;

		case ESagaRpcProtocol::RPC_SKILL_0:
		{}
		break;

		case ESagaRpcProtocol::RPC_SKILL_1:
		{}
		break;

		case ESagaRpcProtocol::RPC_SKILL_2:
		{}
		break;

		case ESagaRpcProtocol::RPC_SKILL_3:
		{}
		break;

		case ESagaRpcProtocol::RPC_USE_ITEM_0:
		{}
		break;

		case ESagaRpcProtocol::RPC_USE_ITEM_1:
		{}
		break;

		case ESagaRpcProtocol::RPC_USE_ITEM_2:
		{}
		break;

		case ESagaRpcProtocol::RPC_USE_ITEM_3:
		{}
		break;

		case ESagaRpcProtocol::RPC_USE_ITEM_4:
		{}
		break;

		case ESagaRpcProtocol::RPC_CHANGE_HAND_ITEM:
		{}
		break;

		case ESagaRpcProtocol::RPC_DEAD:
		{}
		break;

		default:
			break;
		}
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
