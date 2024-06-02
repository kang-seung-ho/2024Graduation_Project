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
	: Super()
{
	static ConstructorHelpers::FClassFinder<ASagaPlayableCharacter> pawn_class{ TEXT("/Game/PlayerAssets/BP/BP_SagaPlayableCharacter.BP_SagaPlayableCharacter_C") };
	if (pawn_class.Class != nullptr)
	{
		DefaultPawnClass = pawn_class.Class;
	}

	PlayerControllerClass = ASagaInGamePlayerController::StaticClass();
}

ASagaCharacterSpawner*
ASagaInGameMode::GetSpawnerBy(EUserTeam team)
const
{
	switch (team)
	{
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

ASagaCharacterSpawner*
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
ASagaInGameMode::InitGame(const FString& mapname, const FString& optios, FString& err_msg)
{
	UE_LOG(LogSagaGame, Log, TEXT("[ASagaInGameMode][InitGame]"));

	AGameModeBase::InitGame(mapname, optios, err_msg);

	const auto world = GetWorld();

	TArray<TObjectPtr<class ASagaCharacterSpawner>> player_spawners{};

	for (TActorIterator<ASagaCharacterSpawner> it{ world }; it; ++it)
	{
		const auto spawner = *it;

		player_spawners.Add(spawner);
	}

	const auto spawners_number = player_spawners.Num();
	UE_LOG(LogSagaGame, Log, TEXT("%d spawner(s) has found."), spawners_number);

	if (0 == spawners_number)
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

	system->OnCreatingCharacter.AddDynamic(this, &ASagaInGameMode::OnCreatingCharacter);
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

void ASagaInGameMode::OnCreatingCharacter(int32 user_id, EUserTeam team, EPlayerWeapon weapon)
{
	const auto world = GetWorld();
	const auto system = USagaNetworkSubSystem::GetSubSystem(world);
	const auto controller = world->GetFirstPlayerController<ASagaInGamePlayerController>();

	if (not system->IsOfflineMode())
	{
		if (not system->IsConnected())
		{
			UE_LOG(LogSagaGame, Error, TEXT("[OnCreatingCharacter] Client is not connected."));
			return;
		}

		const auto local_id = system->GetLocalUserId();
		if (-1 == local_id)
		{
			UE_LOG(LogSagaGame, Error, TEXT("[OnCreatingCharacter] Local user could not create a playable character."));
			return;
		}

		ASagaCharacterPlayer* character;

		if (local_id == user_id)
		{
			UE_LOG(LogSagaGame, Log, TEXT("[OnCreatingCharacter] Local user `%d` doesn't need to create a character."), user_id);

			// NOTICE: 여기서 로컬 캐릭터 할당
			character = controller->GetPawn<ASagaCharacterPlayer>();
			system->SetCharacterHandle(local_id, character);

			// The id was stored on LobbyLevel
			character->SetUserId(user_id);
			// The team was stored on RoomSessionLevel
			character->SetTeamColorAndCollision(team);
			// The weapon was stored on CharacterSelectLevel
			character->SetWeapon(weapon);
			character->AttachWeapon();

			// Making a spawn point
			const FVector pos = character->GetActorLocation();

			// store the initial spawn point (local)
			system->StorePosition(user_id, pos.X, pos.Y, pos.Z);

			UE_LOG(LogSagaGame, Log, TEXT("[OnCreatingCharacter] Local user `%d` created a spawn point at (%f,%f,%f)"), user_id, pos.X, pos.Y, pos.Z);

			int64 arg0{};
			int32 arg1{};
			const auto x = static_cast<float>(pos.X);
			const auto y = static_cast<float>(pos.Y);
			const auto z = static_cast<float>(pos.Z);

			memcpy(reinterpret_cast<char*>(&arg0), &x, 4);
			memcpy(reinterpret_cast<char*>(&arg0) + 4, &y, 4);
			memcpy(reinterpret_cast<char*>(&arg1), &z, 4);

			// let others know this
			system->SendRpcPacket(ESagaRpcProtocol::RPC_POSITION, arg0, arg1);
		}
		else
		{
			UE_LOG(LogSagaGame, Log, TEXT("[OnCreatingCharacter] User `%d` would create a playable character."), user_id);

			const AActor* spawner = GetSpawnerBy(team);
			if (nullptr == spawner)
			{
				spawner = world->GetWorldSettings();
			}

			FActorSpawnParameters setting{};
			setting.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			const FTransform& transform = spawner->GetTransform();

			character = world->SpawnActor<ASagaPlayableCharacter>(DefaultPawnClass, transform, setting);

			if (IsValid(character))
			{
				system->SetCharacterHandle(user_id, character);

				// The id was stored on LobbyLevel
				character->SetUserId(user_id);
				// The team was stored on RoomSessionLevel
				character->SetTeamColorAndCollision(team);
				// The weapon was stored on CharacterSelectLevel
				character->SetWeapon(weapon);
				character->AttachWeapon();
			}
			else
			{
				UE_LOG(LogSagaGame, Error, TEXT("[OnCreatingCharacter] User %d could not create a character!"), user_id);
			}
		}
	}
	else
	{
		UE_LOG(LogSagaGame, Log, TEXT("[OnCreatingCharacter] Local user `%d` doesn't need to create a character. (Offline Mode)"), user_id);

		const auto character = controller->GetPawn<ASagaCharacterPlayer>();
		system->SetCharacterHandle(user_id, character);

		character->SetUserId(user_id);
		character->SetTeamColorAndCollision(team);
		character->SetWeapon(weapon);
		character->AttachWeapon();
	}
}
