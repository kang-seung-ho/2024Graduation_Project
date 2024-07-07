#include "SagaInGameMode.h"
#include <Math/MathFwd.h>
#include <Engine/World.h>
#include <EngineUtils.h>
#include <Kismet/GameplayStatics.h>
#include <UObject/Object.h>
#include <UObject/ConstructorHelpers.h>
#include <GameFramework/Controller.h>
#include <Containers/UnrealString.h>
#include <Containers/Array.h>
#include <Templates/Casts.h>

#include "SagaGameSubsystem.h"
#include "PlayerControllers/SagaInGamePlayerController.h"
#include "Player/SagaPlayerTeam.h"
#include "Character/SagaCharacterSpawner.h"
#include "Character/SagaCharacterBase.h"
#include "Character/SagaPlayableCharacter.h"
#include "Character/SagaGummyBearPlayer.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

const FString SagaRedTeamName = TEXT("Red");
const FString SagaBluTeamName = TEXT("Blue");

ASagaInGameMode::ASagaInGameMode(const FObjectInitializer& initializer)
	: Super(initializer)
	, storedLocalPlayerController()
	, readyTimerHandle()
	, transformUpdateTimer()
	, playerSpawners()
	, lastCharacterPosition(), lastCharacterRotation()
	, everyBears()
{
	SetControllerClass(ASagaInGamePlayerController::StaticClass());

	static ConstructorHelpers::FClassFinder<ASagaPlayableCharacter> pawn_class{ TEXT("/Game/PlayerAssets/BP/BP_SagaPlayableCharacter.BP_SagaPlayableCharacter_C") };
	if (pawn_class.Succeeded())
	{
		SetPawnClass(pawn_class.Class);
	}
	else
	{
		const auto name = GetName();
		UE_LOG(LogSagaGame, Error, TEXT("[ASagaInGameMode][ctor] '%s' could not find the class of player character."), *name);
	}

	everyBears.Reserve(3);
}

ASagaGummyBearPlayer*
ASagaInGameMode::FindBear(const int32 id)
const noexcept
{
	for (auto& bear : everyBears)
	{
		if (bear->GetBearId() == id)
		{
			return bear;
		}
	}

	return nullptr;
}

void
ASagaInGameMode::InitGame(const FString& mapname, const FString& optios, FString& err_msg)
{
	UE_LOG(LogSagaGame, Log, TEXT("[ASagaInGameMode][InitGame]"));

	Super::InitGame(mapname, optios, err_msg);

	const auto world = GetWorld();

	playerSpawners.Reserve(10);
	for (TActorIterator<ASagaCharacterSpawner> it{ world }; it; ++it)
	{
		const auto spawner = *it;

		playerSpawners.Add(spawner);
	}

	const auto spawners_number = playerSpawners.Num();
	UE_LOG(LogSagaGame, Log, TEXT("%d spawner(s) has found."), spawners_number);

	if (playerSpawners.IsEmpty())
	{
		UE_LOG(LogSagaGame, Warning, TEXT("Any spawner does not exist."));

		FVector center{ 0, 0, 100 };
		const auto automatic_spawner = world->SpawnActor<ASagaCharacterSpawner>(center, FRotator::ZeroRotator);
		ensure(automatic_spawner != nullptr);

		const FTransform transform{ FRotator::ZeroRotator, center };
		UGameplayStatics::FinishSpawningActor(automatic_spawner, transform);

		UE_LOG(LogSagaGame, Log, TEXT("An automatic spawner is generated."));
	}

	const auto net = USagaNetworkSubSystem::GetSubSystem(world);

	if (not net->IsOfflineMode())
	{
		net->OnLeftRoom.AddDynamic(this, &ASagaInGameMode::OnLeftRoom);
		net->OnGameStarted.AddDynamic(this, &ASagaInGameMode::OnGameStarted);
		net->OnCreatingCharacter.AddDynamic(this, &ASagaInGameMode::OnCreatingCharacter);
		net->OnRpc.AddDynamic(this, &ASagaInGameMode::OnRpc);
	}
}

void
ASagaInGameMode::StartPlay()
{
	const auto world = GetWorld();
	const auto net = USagaNetworkSubSystem::GetSubSystem(world);
	const auto sys = USagaGameSubsystem::GetSubSystem(world);

	if (not net->IsOfflineMode())
	{
		UE_LOG(LogSagaGame, Log, TEXT("[ASagaInGameMode][StartPlay]"));
	}
	else
	{
		UE_LOG(LogSagaGame, Warning, TEXT("[ASagaInGameMode][StartPlay] (Offline Mode)"));
	}

	switch (net->GetLocalUserTeam())
	{
	case ESagaPlayerTeam::Red:
	{
		UE_LOG(LogSagaGame, Log, TEXT("Assigning a player spawner for red team..."));
		sys->AssignLocalPlayerSpawner(GetSpawner(SagaRedTeamName));
	}

	case ESagaPlayerTeam::Blue:
	{
		UE_LOG(LogSagaGame, Log, TEXT("Assigning a player spawner for blue team..."));
		sys->AssignLocalPlayerSpawner(GetSpawner(SagaRedTeamName));
	}

	default:
	{
		UE_LOG(LogSagaGame, Warning, TEXT("Assigning a player spawner for unknown team..."));
		sys->AssignLocalPlayerSpawner(world->GetWorldSettings());
	}
	break;
	}

	// Find the local controller
	// At first, there is only an ASagaInGamePlayerController
	for (TActorIterator<ASagaInGamePlayerController> it{ world }; it; ++it)
	{
		const auto controller = *it;

		storedLocalPlayerController = controller;
		storedLocalPlayerController->SetAsLocalPlayerController();
	}

	// Store every gummy bear
	for (TActorIterator<ASagaGummyBearPlayer> it{ world }; it; ++it)
	{
		const auto bear = *it;

		everyBears.Add(bear);
	}

	if (not net->IsOfflineMode())
	{
		if (net->IsConnected())
		{
			UE_LOG(LogSagaGame, Log, TEXT("[ASagaInGameMode][StartPlay] Game mode would send a loaded packet"));
			net->SendGameIsLoadedPacket();

		}
	}
	else
	{
		OnCreatingCharacter(net->GetLocalUserId(), net->GetLocalUserTeam(), net->GetLocalUserWeapon());
	}

	// First find the local controller and enumerate actors' BeginPlay
	Super::StartPlay();
}

AActor*
ASagaInGameMode::ChoosePlayerStart_Implementation(AController* player)
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	switch (net->GetLocalUserTeam())
	{
	case ESagaPlayerTeam::Red:
	{
		UE_LOG(LogSagaGame, Log, TEXT("Choosing a player spawner for red team..."));
		return GetSpawner(SagaRedTeamName);
	}

	case ESagaPlayerTeam::Blue:
	{
		UE_LOG(LogSagaGame, Log, TEXT("Choosing a player spawner for blue team..."));
		return GetSpawner(SagaBluTeamName);
	}

	default:
	{
		UE_LOG(LogSagaGame, Warning, TEXT("Choosing a player spawner for unknown team..."));
		return Super::ChoosePlayerStart_Implementation(player);
	}
	}
}

AActor*
ASagaInGameMode::FindPlayerStart_Implementation(AController* player, const FString& tag)
{
	UE_LOG(LogSagaGame, Log, TEXT("Finding a player spawn by a tag '%s'..."), *tag);

	return Super::FindPlayerStart_Implementation(player, tag);
}

void
ASagaInGameMode::EndPlay(const EEndPlayReason::Type reason)
{
	Super::EndPlay(reason);
}

AActor*
ASagaInGameMode::GetSpawnerBy(ESagaPlayerTeam team)
const
{
	switch (team)
	{
	case ESagaPlayerTeam::Unknown:
	case ESagaPlayerTeam::Red:
	{
		return GetSpawner(SagaRedTeamName);
	}

	case ESagaPlayerTeam::Blue:
	{
		return GetSpawner(SagaBluTeamName);
	}

	default:
	{
		return nullptr;
	}
	}
}

AActor*
ASagaInGameMode::GetSpawner(const FString& team_name)
const
{
	for (TActorIterator<ASagaCharacterSpawner> It{ GetWorld() }; It; ++It)
	{
		const auto starter = *It;
		if (starter && starter->PlayerStartTag == team_name)
		{
			return starter;
		}
	}

	return nullptr;
}

void
ASagaInGameMode::HandleUpdateTransform()
{
	const auto world = GetWorld();
	const auto net = USagaNetworkSubSystem::GetSubSystem(world);

	if (net->IsConnected())
	{
		const auto controller = world->GetFirstPlayerController<ASagaInGamePlayerController>();
		const auto pawn = controller->GetPawn<ASagaCharacterBase>();

		if (pawn->IsAlive())
		{
			const auto loc = pawn->GetActorLocation();
			if (lastCharacterPosition != loc)
			{
				int64 arg0{};
				int32 arg1{};

				SerializePosition(loc, arg0, arg1);

				//net->SendPositionPacket(loc.X, loc.Y, loc.Z);
				net->SendRpcPacket(ESagaRpcProtocol::RPC_POSITION, arg0, arg1);

				lastCharacterPosition = loc;
			}

			const auto rot = pawn->GetActorRotation();
			if (lastCharacterRotation != rot)
			{
				int64 arg0{};
				int32 arg1{};

				SerializePosition(FVector{ rot.Pitch, rot.Yaw, rot.Roll }, arg0, arg1);

				//net->SendRotationPacket(rot.Pitch, rot.Yaw, rot.Roll);
				net->SendRpcPacket(ESagaRpcProtocol::RPC_ROTATION, arg0, arg1);

				lastCharacterRotation = rot;
			}
		}
	}
	else
	{
		GetWorldTimerManager().ClearTimer(transformUpdateTimer);
	}
}

void
SerializePosition(const FVector& vector, int64& arg0, int32& arg1)
{
	const auto x = static_cast<float>(vector.X);
	const auto y = static_cast<float>(vector.Y);
	const auto z = static_cast<float>(vector.Z);

	memcpy(reinterpret_cast<char*>(&arg0), &x, 4);
	memcpy(reinterpret_cast<char*>(&arg0) + 4, &y, 4);
	memcpy(reinterpret_cast<char*>(&arg1), &z, 4);
}

FVector
DeserializePosition(const int64& arg0, const int32& arg1)
{
	float x{};
	float y{};
	float z{};

	std::memcpy(&x, &arg0, 4);
	std::memcpy(&y, reinterpret_cast<const char*>(&arg0) + 4, 4);
	std::memcpy(&z, &arg1, 4);

	return FVector{ x, y, z };
}