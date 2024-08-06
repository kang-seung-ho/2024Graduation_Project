#include "Saga/Network/SagaNetworkSubSystem.h"
#include <Interfaces/IPv4/IPv4Address.h>
#include <Sockets.h>
#include <SocketSubsystem.h>

#include "Saga/Network/SagaNetworkUtility.h"
#include "Saga/Network/SagaNetworkSettings.h"

ESagaConnectionContract
USagaNetworkSubSystem::ConnectToServer_Implementation()
{
	if (not IsSocketAvailable())
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("The socket is not available."));
		return ESagaConnectionContract::NoSocket;
	}

	// 연결
	if (not IsOfflineMode())
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

			return ESagaConnectionContract::ConnectError;
		}

		// 클라는 접속 이후에 닉네임 패킷을 보내야 한다.
		auto sent_r = SendSignInPacket(localUserName);
		if (sent_r <= 0)
		{
			const auto err_msg = saga::GetLastErrorContents();
			UE_LOG(LogSagaNetwork, Error, TEXT("First try of sending signin packet has been failed, due to '%s'"), *err_msg);

			return ESagaConnectionContract::SignInFailed;
		}
		else
		{
			UE_LOG(LogSagaNetwork, Log, TEXT("Local client has sent a signing in packet."));
		}
	}

	return ESagaConnectionContract::Success;
}

bool
USagaNetworkSubSystem::CloseNetwork_Implementation()
{
	//clientSocket->Shutdown(ESocketShutdownMode::ReadWrite);

	return std::exchange(clientSocket, CreateSocket())->Close();
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

FSocket*
USagaNetworkSubSystem::CreateSocket()
{
	FSocket* socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

	socket->SetReuseAddr();

	return socket;
}

TSharedRef<FInternetAddr>
USagaNetworkSubSystem::CreateRemoteEndPoint()
{
	const auto settings = GetDefault<USagaNetworkSettings>();

	if (settings->ConnectionCategory == ESagaNetworkConnectionCategory::Local)
	{
		return saga::MakeEndPoint(FIPv4Address::Any, settings->RemotePort);
	}
	else if (settings->ConnectionCategory == ESagaNetworkConnectionCategory::Host)
	{
		return saga::MakeEndPoint(FIPv4Address::InternalLoopback, settings->RemotePort);
	}
	else if (settings->ConnectionCategory == ESagaNetworkConnectionCategory::Remote)
	{
		return saga::MakeEndPointFrom(settings->RemoteAddress, settings->RemotePort);
	}
	else
	{
		throw "error!";
	}
}

USagaNetworkSubSystem*
USagaNetworkSubSystem::GetSubSystem(const UWorld* world) noexcept
{
	auto singleton = world->GetGameInstance();

	return singleton->GetSubsystem<USagaNetworkSubSystem>();
}
