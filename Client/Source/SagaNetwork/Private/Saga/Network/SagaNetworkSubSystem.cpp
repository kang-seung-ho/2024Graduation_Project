#include "Saga/Network/SagaNetworkSubSystem.h"
#include <Engine/World.h>
#include <UObject/UObjectGlobals.h>
#include <Containers/Array.h>
#include <SocketTypes.h>
#include <Sockets.h>
#include <SocketSubsystem.h>
#include <Interfaces/IPv4/IPv4Address.h>
#include <Templates/SharedPointer.h>

#include "Saga/Network/SagaNetworkUtility.h"

USagaNetworkSubSystem::USagaNetworkSubSystem()
	: UGameInstanceSubsystem()
	, netIsOfflineMode()
	, netConnectionCategory(ESagaNetworkConnectionCategory::Remote)
	, netRemoteAddress(), netRemotePort(), netLocalPort()
	, localUser()
	, currentRoom()
	, OnNetworkInitialized(), OnConnected(), OnFailedToConnect(), OnDisconnected(), OnSignedIn()
	, OnRoomCreated(), OnJoinedRoom(), OnOtherJoinedRoom(), OnLeftRoomBySelf(), OnLeftRoom()
	, OnRespondVersion(), OnUpdateRoomList(), OnUpdateMembers()
	, OnTeamChanged()
	, OnGetPreparedGame(), OnStartGame(), OnCreatingCharacter()
	, OnUpdatePosition(), OnUpdateRotation(), OnRpc()
	, clientSocket(), netWorker(), taskWorker()
	, recvBuffer(), recvBytes()
	, taskBarrier()
	, everyUsers(), everyRooms(), wasUsersUpdated(true), wasRoomsUpdated(true)
{}

ESagaConnectionContract
USagaNetworkSubSystem::ConnectToServer(const FText& nickname)
{
	if (IsOfflineMode())
	{
		UE_LOG(LogSagaNetwork, Log, TEXT("The network subsystem is getting started. (Offline Mode)"));

		CallFunctionOnGameThread([this, nickname]()
			{
				SetLocalUserId(0);
				SetLocalUserName(nickname);
			}
		);

		return ESagaConnectionContract::Success;
	}
	else
	{
		if (IsConnected())
		{
			UE_LOG(LogSagaNetwork, Warning, TEXT("The network subsystem was started."));

			return ESagaConnectionContract::Success;
		}
		else
		{
			if (not IsSocketAvailable())
			{
				UE_LOG(LogSagaNetwork, Error, TEXT("[ConnectToServer] The socket was not initialized."));

				CallPureFunctionOnGameThread([this]()
					{
						BroadcastOnFailedToConnect(ESagaConnectionContract::NoSocket);
					}
				);

				return ESagaConnectionContract::SubSystemError;
			}

			auto remote_endpoint = CreateRemoteEndPoint(netConnectionCategory);
			if (not remote_endpoint->IsValid())
			{
				const FString err_msg = saga::GetLastErrorContents();
				UE_LOG(LogSagaNetwork, Error, TEXT("[ConnectToServer] The endpoint has fault, due to '%s'"), *err_msg);

				CallPureFunctionOnGameThread([this]()
					{
						BroadcastOnFailedToConnect(ESagaConnectionContract::WrongAddress);
					}
				);

				return ESagaConnectionContract::WrongAddress;
			}

			UE_LOG(LogSagaNetwork, Log, TEXT("Connecting..."));

			if (not clientSocket->Connect(*remote_endpoint))
			{
				// 연결 실패 처리
				const FString err_msg = saga::GetLastErrorContents();
				UE_LOG(LogSagaNetwork, Error, TEXT("[ConnectToServer] Cannot connect to the server, due to '%s'"), *err_msg);

				CallPureFunctionOnGameThread([this]()
					{
						BroadcastOnFailedToConnect(ESagaConnectionContract::ConnectError);
					}
				);

				return ESagaConnectionContract::ConnectError;
			}

			// 클라는 접속 이후에 닉네임 패킷을 보내야 한다.
			const auto sent_r = SendSignInPacket(nickname);
			if (sent_r <= 0)
			{
				const FString err_msg = saga::GetLastErrorContents();
				UE_LOG(LogSagaNetwork, Error, TEXT("[ConnectToServer] First try of sending signin packet has been failed, due to '%s'"), *err_msg);

				CallPureFunctionOnGameThread([this]()
					{
						BroadcastOnFailedToConnect(ESagaConnectionContract::SignInFailed);
					}
				);

				return ESagaConnectionContract::SignInFailed;
			}
			else
			{
				UE_LOG(LogSagaNetwork, Log, TEXT("Local client has sent a signing in packet."));
			}

			UE_LOG(LogSagaNetwork, Log, TEXT("The network subsystem is getting started."));

			CallFunctionOnGameThread([this, nickname]()
				{
					SetLocalUserName(nickname);

					BroadcastOnConnected();
				}
			);

			// 서버가 닉네임 패킷을 받으면 서버는 ID 부여 패킷을 보낸다.
			// 클라는 ID 부여 패킷을 받아서 갱신하고, 게임 or 메뉴 레벨로 넘어가야 한다.
			return ESagaConnectionContract::Success;
		}
	}
}

bool
USagaNetworkSubSystem::CloseNetwork_Implementation()
{
	return std::exchange(clientSocket, CreateSocket())->Close();
}

bool
USagaNetworkSubSystem::Close()
{
	if (not IsOfflineMode())
	{
		if (not IsSocketAvailable())
		{
			UE_LOG(LogSagaNetwork, Warning, TEXT("The socket of client is null."));

			return true;
		}
		else if (not IsConnected())
		{
			UE_LOG(LogSagaNetwork, Warning, TEXT("The network subsystem had been closed."));

			return CloseNetwork_Implementation();
		}
		else
		{
			UE_LOG(LogSagaNetwork, Warning, TEXT("Closing the network subsystem..."));

			clientSocket->Shutdown(ESocketShutdownMode::ReadWrite);

			return CloseNetwork_Implementation();
		}
	}
	else
	{
		UE_LOG(LogSagaNetwork, Warning, TEXT("Closing the network subsystem... (Offline Mode)"));

		return true;
	}
}

void
USagaNetworkSubSystem::SetOfflineMode(bool flag)
noexcept
{
	netIsOfflineMode = flag;
}

void
USagaNetworkSubSystem::SetConnectionOption(ESagaNetworkConnectionCategory category)
noexcept
{
	netConnectionCategory = category;
}

void
USagaNetworkSubSystem::SetRemoteAddress(const FString& address)
{
	netRemoteAddress = address;
}

void
USagaNetworkSubSystem::SetRemotePort(int32 port)
noexcept
{
	netRemotePort = port;
}

void
USagaNetworkSubSystem::SetLocalPort(int32 port)
noexcept
{
	netLocalPort = port;
}

ESagaNetworkConnectionCategory
USagaNetworkSubSystem::GetConnectionOption()
const noexcept
{
	return netConnectionCategory;
}

FString
USagaNetworkSubSystem::GetRemoteAddress()
const
{
	return netRemoteAddress;
}

int32
USagaNetworkSubSystem::GetRemotePort()
const noexcept
{
	return netRemotePort;
}

int32
USagaNetworkSubSystem::GetLocalPort()
const noexcept
{
	return netLocalPort;
}

bool
USagaNetworkSubSystem::IsOfflineMode()
const noexcept
{
	return netIsOfflineMode;
}

bool
USagaNetworkSubSystem::IsSocketAvailable()
const noexcept
{
	return nullptr != clientSocket;
}

bool
USagaNetworkSubSystem::IsConnected()
const noexcept
{
	if (IsOfflineMode())
	{
		return true;
	}
	else if (clientSocket != nullptr and clientSocket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
	{
		return true;
	}
	else
	{
		return false;
	}
}

USagaNetworkSubSystem*
USagaNetworkSubSystem::GetSubSystem(const UWorld* world)
noexcept
{
	const auto singleton = world->GetGameInstance();

	return singleton->GetSubsystem<USagaNetworkSubSystem>();
}

FSocket*
USagaNetworkSubSystem::CreateSocket()
{
	FSocket* socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

	socket->SetReuseAddr();

	return socket;
}

TSharedRef<FInternetAddr>
USagaNetworkSubSystem::CreateRemoteEndPoint(ESagaNetworkConnectionCategory category)
const
{
	if (category == ESagaNetworkConnectionCategory::Local)
	{
		return saga::MakeEndPoint(FIPv4Address::Any, netRemotePort);
	}
	else if (category == ESagaNetworkConnectionCategory::Host)
	{
		return saga::MakeEndPoint(FIPv4Address::InternalLoopback, netRemotePort);
	}
	else if (category == ESagaNetworkConnectionCategory::Remote)
	{
		return saga::MakeEndPointFrom(netRemoteAddress, netRemotePort);
	}
	else
	{
		throw "error!";
	}
}
