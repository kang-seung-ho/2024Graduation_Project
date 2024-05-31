#include "Saga/Network/SagaNetworkSubSystem.h"
#include <UObject/ObjectMacros.h>
#include <UObject/Object.h>
#include <Subsystems/SubsystemCollection.h>
#include <Delegates/Delegate.h>

#include <Saga/Network/SagaNetworkSettings.h>

void
USagaNetworkSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogSagaNetwork, Log, TEXT("[USagaNetworkSubSystem] Loading configuration..."));

	const auto settings = GetDefault<USagaNetworkSettings>();
	netIsOfflineMode = settings->IsOfflineMode;
	netConnectionCategory = settings->ConnectionCategory;
	netRemoteAddress = settings->RemoteAddress;
	netRemotePort = settings->RemotePort;
	netLocalPort = settings->LocalPort;

	UE_LOG(LogSagaNetwork, Log, TEXT("[USagaNetworkSubSystem] Is Offline Mode: %s"), netIsOfflineMode ? TEXT("True") : TEXT("False"));

	const auto option = UEnum::GetValueAsString(netConnectionCategory);
	UE_LOG(LogSagaNetwork, Log, TEXT("[USagaNetworkSubSystem] Connect Option: %s"), *option);

	UE_LOG(LogSagaNetwork, Log, TEXT("[USagaNetworkSubSystem] Remote Address: %s"), *netRemoteAddress);
	UE_LOG(LogSagaNetwork, Log, TEXT("[USagaNetworkSubSystem] Remote Port: %d"), netRemotePort);
	UE_LOG(LogSagaNetwork, Log, TEXT("[USagaNetworkSubSystem] Local Port: %d"), netLocalPort);

	OnNetworkInitialized.AddDynamic(this, &USagaNetworkSubSystem::OnNetworkInitialized_Implementation);
	OnFailedToConnect.AddDynamic(this, &USagaNetworkSubSystem::OnFailedToConnect_Implementation);
	OnConnected.AddDynamic(this, &USagaNetworkSubSystem::OnConnected_Implementation);
	OnDisconnected.AddDynamic(this, &USagaNetworkSubSystem::OnDisconnected_Implementation);
	OnFailedToStartGame.AddDynamic(this, &USagaNetworkSubSystem::OnFailedToStartGame_Implementation);
	OnRpc.AddDynamic(this, &USagaNetworkSubSystem::OnRpc_Implementation);

	everyUsers.Reserve(100);
	everyRooms.Reserve(100);

	recvBuffer.Init(0, recvLimit);

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

	auto settings = GetMutableDefault<USagaNetworkSettings>();
	settings->IsOfflineMode = netIsOfflineMode;
	settings->ConnectionCategory = netConnectionCategory;
	settings->RemoteAddress = netRemoteAddress;
	settings->RemotePort = netRemotePort;
	settings->LocalPort = netLocalPort;

	const FString default_config = settings->GetDefaultConfigFilename();
	settings->SaveConfig(CPF_Config, *default_config);

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
