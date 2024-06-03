#include "SagaInGameMode.h"
#include <Math/MathFwd.h>
#include <Engine/World.h>
#include <EngineUtils.h>
#include <Kismet/GameplayStatics.h>
#include <UObject/Object.h>
#include <UObject/ConstructorHelpers.h>
#include <GameFramework/Actor.h>
#include <GameFramework/Controller.h>
#include <GameFramework/GameStateBase.h>
#include <Containers/UnrealString.h>
#include <Containers/Array.h>
#include <Templates/Casts.h>

#include "Player/SagaUserTeam.h"
#include "Character/SagaInGamePlayerController.h"
#include "Character/SagaCharacterSpawner.h"
#include "Character/SagaCharacterPlayer.h"
#include "Character/SagaPlayableCharacter.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

const FString SagaRedTeamName = TEXT("Red");
const FString SagaBluTeamName = TEXT("Blue");

ASagaInGameMode::ASagaInGameMode()
	: AGameModeBase()
	, playerSpawners()
	, localPlayerController()
{
	static ConstructorHelpers::FClassFinder<ASagaPlayableCharacter> pawn_class{ TEXT("/Game/PlayerAssets/BP/BP_SagaPlayableCharacter.BP_SagaPlayableCharacter_C") };
	if (pawn_class.Class != nullptr)
	{
		DefaultPawnClass = pawn_class.Class;
	}

	PlayerControllerClass = ASagaInGamePlayerController::StaticClass();
}

void
ASagaInGameMode::InitGame(const FString& mapname, const FString& optios, FString& err_msg)
{
	UE_LOG(LogSagaGame, Log, TEXT("[ASagaInGameMode][InitGame]"));

	AGameModeBase::InitGame(mapname, optios, err_msg);

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

	const auto system = USagaNetworkSubSystem::GetSubSystem(world);

	if (not system->IsOfflineMode())
	{
		system->OnLeftRoom.AddDynamic(this, &ASagaInGameMode::OnLeftRoom);
		system->OnGameStarted.AddDynamic(this, &ASagaInGameMode::OnGameStarted);
		system->OnCreatingCharacter.AddDynamic(this, &ASagaInGameMode::OnCreatingCharacter);
		system->OnRpc.AddDynamic(this, &ASagaInGameMode::OnRpc);
	}
}

void
ASagaInGameMode::StartPlay()
{
	const auto world = GetWorld();
	const auto system = USagaNetworkSubSystem::GetSubSystem(world);

	if (not system->IsOfflineMode())
	{
		UE_LOG(LogSagaGame, Log, TEXT("[ASagaInGameMode][StartPlay]"));

		if (system->IsConnected())
		{
			UE_LOG(LogSagaGame, Log, TEXT("[ASagaInGameMode][StartPlay] Game mode would send a loaded packet"));
			system->SendGameIsLoadedPacket();

			// Find the local controller
			// At first, there is only an ASagaInGamePlayerController
			for (TActorIterator<ASagaInGamePlayerController> it{ world }; it; ++it)
			{
				const auto controller = *it;

				localPlayerController = controller;
				localPlayerController->SetAsLocalPlayerController();
				localPlayerController->SetOwnerId(system->GetLocalUserId());

				switch (system->GetLocalUserTeam())
				{
				case EUserTeam::Red:
				{
					UE_LOG(LogSagaGame, Log, TEXT("Assigning a player spawner for red team..."));
					localPlayerController->AssignPlayerSpawner(GetSpawner(SagaRedTeamName));
				}

				case EUserTeam::Blue:
				{
					UE_LOG(LogSagaGame, Log, TEXT("Assigning a player spawner for blue team..."));
					localPlayerController->AssignPlayerSpawner(GetSpawner(SagaRedTeamName));
				}

				default:
				{
					UE_LOG(LogSagaGame, Warning, TEXT("Assigning a player spawner for unknown team..."));
					localPlayerController->AssignPlayerSpawner(world->GetWorldSettings());
				}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogSagaGame, Warning, TEXT("[ASagaInGameMode][StartPlay] (Offline Mode)"));
	}

	// First find the local controller and enumerate actors' BeginPlay
	AGameModeBase::StartPlay();
}

AActor*
ASagaInGameMode::ChoosePlayerStart_Implementation(AController* player)
{
	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	switch (system->GetLocalUserTeam())
	{
	case EUserTeam::Red:
	{
		UE_LOG(LogSagaGame, Log, TEXT("Choosing a player spawner for red team..."));
		return GetSpawner(SagaRedTeamName);
	}

	case EUserTeam::Blue:
	{
		UE_LOG(LogSagaGame, Log, TEXT("Choosing a player spawner for blue team..."));
		return GetSpawner(SagaBluTeamName);
	}

	default:
	{
		UE_LOG(LogSagaGame, Warning, TEXT("Choosing a player spawner for unknown team..."));
		return AGameModeBase::ChoosePlayerStart_Implementation(player);
	}
	}
}

AActor*
ASagaInGameMode::FindPlayerStart_Implementation(AController* player, const FString& tag)
{
	UE_LOG(LogSagaGame, Log, TEXT("Finding a player spawn by a tag '%s'..."), *tag);

	return AGameModeBase::FindPlayerStart_Implementation(player, tag);
}

AActor*
ASagaInGameMode::GetSpawnerBy(EUserTeam team)
const
{
	switch (team)
	{
	case EUserTeam::Unknown:
	case EUserTeam::Red:
	{
		return GetSpawner(SagaRedTeamName);
	}

	case EUserTeam::Blue:
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
