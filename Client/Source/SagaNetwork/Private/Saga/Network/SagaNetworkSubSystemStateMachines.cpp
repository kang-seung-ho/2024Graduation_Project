#include "Saga/Network/SagaNetworkSubSystem.h"
#include <Delegates/Delegate.h>
#include <Subsystems/SubsystemCollection.h>

void
USagaNetworkSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogSagaNetwork, Log, TEXT("[USagaNetworkSubSystem] Loading configuration..."));
	UE_LOG(LogSagaNetwork, Log, TEXT("[USagaNetworkSubSystem] Is Offline Mode: %s"), IsOfflineMode() ? TEXT("True") : TEXT("False"));

	const auto option = UEnum::GetValueAsString(GetConnectionOption());
	UE_LOG(LogSagaNetwork, Log, TEXT("[USagaNetworkSubSystem] Connect Option: %s"), *option);

	const auto address = GetRemoteAddress();
	UE_LOG(LogSagaNetwork, Log, TEXT("[USagaNetworkSubSystem] Remote Address: %s"), *address);
	UE_LOG(LogSagaNetwork, Log, TEXT("[USagaNetworkSubSystem] Remote Port: %d"), GetRemotePort());
	UE_LOG(LogSagaNetwork, Log, TEXT("[USagaNetworkSubSystem] Local Port: %d"), GetLocalPort());

	OnNetworkInitialized.AddDynamic(this, &USagaNetworkSubSystem::OnNetworkInitialized_Implementation);
	OnFailedToConnect.AddDynamic(this, &USagaNetworkSubSystem::OnFailedToConnect_Implementation);
	OnConnected.AddDynamic(this, &USagaNetworkSubSystem::OnConnected_Implementation);
	OnDisconnected.AddDynamic(this, &USagaNetworkSubSystem::OnDisconnected_Implementation);
	OnRoomCreated.AddDynamic(this, &USagaNetworkSubSystem::OnRoomCreated_Implementation);
	OnLeftRoomBySelf.AddDynamic(this, &USagaNetworkSubSystem::OnLeftRoomBySelf_Implementation);
	OnLeftRoom.AddDynamic(this, &USagaNetworkSubSystem::OnLeftRoom_Implementation);
	OnRespondVersion.AddDynamic(this, &USagaNetworkSubSystem::OnRespondVersion_Implementation);
	OnFailedToStartGame.AddDynamic(this, &USagaNetworkSubSystem::OnFailedToStartGame_Implementation);
	OnRpc.AddDynamic(this, &USagaNetworkSubSystem::OnRpc_Implementation);

	everyUsers.Reserve(100);
	everyRooms.Reserve(100);

	recvBuffer.Init(0, recvLimit);
	transitBuffer.Init(0, recvLimit);

	clientSocket = CreateSocket();

	// NOTICE: 클라는 바인드 금지
	//auto local_endpoint = saga::MakeEndPoint(FIPv4Address::InternalLoopback, saga::GetLocalPort());
	//if (not clientSocket->Bind(*local_endpoint))
	//{
	//	return false;
	//}

	if (nullptr != clientSocket)
	{
		UE_LOG(LogSagaNetwork, Log, TEXT("The socket is initialized."));
		BroadcastOnNetworkInitialized();
	}
	else
	{
		UE_LOG(LogSagaNetwork, Fatal, TEXT("The socket is null."));
		BroadcastOnFailedToInitializeNetwork();
	}
}

void
USagaNetworkSubSystem::Deinitialize()
{
	Super::Deinitialize();

	UE_LOG(LogSagaNetwork, Log, TEXT("Saving configuration..."));
	SaveConfig();

	if (not IsOfflineMode())
	{
		if (IsSocketAvailable())
		{
			Close();
		}
		else
		{
			UE_LOG(LogSagaNetwork, Warning, TEXT("The network subsystem has been destroyed."));
		}
	}
	else
	{
		UE_LOG(LogSagaNetwork, Warning, TEXT("The network subsystem has been destroyed. (Offline Mode)"));
	}
}

bool
USagaNetworkSubSystem::ShouldCreateSubsystem(UObject* Outer)
const
{
	return true;
}
