#include "SagaInGamePlayerController.h"
#include "SagaCharacterPlayer.h"

#include "Saga/Network/SagaNetworkSettings.h"
#include "Saga/Network/SagaNetworkSubSystem.h"
#include "Saga/Network/SagaRpcProtocol.h"

void
ASagaInGamePlayerController::OnCreatingCharacter(int32 user_id, EUserTeam team, UClass* character_class)
{
	// TODO: 스폰 지점 설정 #1

	FTransform transform{};
	transform.TransformPosition({ 0, 0, 300 });

	auto character = Cast<ASagaCharacterPlayer>(CreatePlayableCharacter(character_class, transform));

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (nullptr != system and system->GetLocalUserId() != -1)
	{
		if (nullptr != character)
		{
			system->SetCharacterHandle(user_id, character);
			system->SetTeam(user_id, team);

			character->SetUserId(user_id);
			character->SetTeamColorAndCollision(team);

			// The weapon was stored on the CharacterSelectLevel
			EPlayerWeapon weapon{};
			if (system->GetWeapon(user_id, weapon))
			{
				character->SetWeapon(weapon);
			}
			else
			{
				character->SetWeapon(EPlayerWeapon::LightSabor);
			}
		}
		else
		{
			UE_LOG(LogSagaGame, Error, TEXT("User %d could not create a character!"), user_id);
		}
	}
	else
	{
		UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
	}
}

void
ASagaInGamePlayerController::OnUpdateTransform()
{
	if constexpr (not saga::IsOfflineMode)
	{
		//UE_LOG(LogSagaGame, Log, TEXT("[Character] Synchronizing Transform"));

		auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		if (nullptr != system and system->GetLocalUserId() != -1)
		{
			const auto pawn = GetPawn<ASagaCharacterPlayer>();

			const auto loc = pawn->K2_GetActorLocation();
			if (lastCharacterPosition != loc)
			{
				system->SendPositionPacket(loc.X, loc.Y, loc.Z);
				lastCharacterPosition = loc;
			}

			const auto rot = pawn->K2_GetActorRotation();
			if (lastCharacterRotation != rot)
			{
				system->SendRotationPacket(rot.Pitch, rot.Yaw, rot.Roll);
				lastCharacterRotation = rot;
			}
		}
		else
		{
			//UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
		}
	}
}

void
ASagaInGamePlayerController::OnLevelReady()
{
	UE_LOG(LogSagaGame, Log, TEXT("[Network] Game controller would send a loaded packet"));

	if constexpr (not saga::IsOfflineMode)
	{
		auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		if (nullptr != system and system->GetLocalUserId() != -1)
		{
			UE_LOG(LogSagaGame, Log, TEXT("[Network] Sending GameIsLoadedPacket..."));
			system->SendGameIsLoadedPacket();
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
		}
	}
}

void
ASagaInGamePlayerController::OnGameStarted()
{
	UE_LOG(LogSagaGame, Log, TEXT("[OnGameStarted] Starting a periodic transformer timer"));

	if constexpr (not saga::IsOfflineMode)
	{
		auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());
		if (nullptr != system)
		{
			const auto pawn = GetPawn<ASagaCharacterPlayer>();

			// TODO: 스폰 지점 설정 #2

			// 버그 있으므로 하면 안됨
			//const auto loc = pawn->K2_GetActorLocation();
			//system->SendPositionPacket(loc.X, loc.Y, loc.Z);
			//const auto rot = pawn->K2_GetActorRotation();
			//system->SendRotationPacket(rot.Pitch, rot.Yaw, rot.Roll);
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
		}
	}

	GetWorldTimerManager().SetTimer(tranformUpdateTimer, this, &ASagaInGamePlayerController::OnUpdateTransform, 0.2f, true, 2.0f);
}
