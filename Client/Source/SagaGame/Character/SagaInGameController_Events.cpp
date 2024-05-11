#include "SagaInGamePlayerController.h"
#include "SagaCharacterPlayer.h"

#include "Saga/Network/SagaNetworkSettings.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

void
ASagaInGamePlayerController::OnCreatingCharacter(int32 user_id, EUserTeam team, EPlayerWeapon weapon)
{
	// TODO: 스폰 지점 설정 #1

	FTransform transform{};
	transform.TransformPosition({ 0, 0, 300 });

	if constexpr (not saga::IsOfflineMode)
	{
		const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());
		const auto character_class = ASagaPlayableCharacter::StaticClass();

		if (nullptr != system)
		{
			const auto local_id = system->GetLocalUserId();
			if (-1 == local_id)
			{
				UE_LOG(LogSagaGame, Error, TEXT("[OnCreatingCharacter] Local user could not create a playable character."));
				return;
			}

			if (local_id == user_id)
			{
				UE_LOG(LogSagaGame, Log, TEXT("[OnCreatingCharacter] Local user `%d` doesn't need to create a character."), user_id);

				// NOTICE: 여기서 로컬 캐릭터 할당
				auto character = GetPawn<ASagaCharacterPlayer>();
				system->SetCharacterHandle(local_id, character);

				// The id was stored on the LobbyLevel
				character->SetUserId(user_id);
				// The team was stored on the RoomSessionLevel
				character->SetTeamColorAndCollision(team);
				// The weapon was stored on the CharacterSelectLevel
				character->SetWeapon(weapon);
			}
			else
			{
				UE_LOG(LogSagaGame, Log, TEXT("[OnCreatingCharacter] User `%d` would create a playable character."), user_id);

				auto character = Cast<ASagaCharacterPlayer>(CreatePlayableCharacter(character_class, transform));

				if (nullptr != character)
				{
					system->SetCharacterHandle(user_id, character);

					// The id was stored on the LobbyLevel
					character->SetUserId(user_id);
					// The team was stored on the RoomSessionLevel
					character->SetTeamColorAndCollision(team);
					// The weapon was stored on the CharacterSelectLevel
					character->SetWeapon(weapon);
				}
				else
				{
					UE_LOG(LogSagaGame, Error, TEXT("[OnCreatingCharacter] User %d could not create a character!"), user_id);
				}
			}
		}
		else
		{
			UE_LOG(LogSagaGame, Error, TEXT("[OnCreatingCharacter] Network subsystem is not ready."));
		}
	}
	else
	{
		UE_LOG(LogSagaGame, Log, TEXT("[OnCreatingCharacter] Local user `%d` doesn't need to create a character. (Offline Mode)"), user_id);

		const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		auto character = GetPawn<ASagaCharacterPlayer>();
		system->SetCharacterHandle(user_id, character);

		// The id was stored on the LobbyLevel
		character->SetUserId(user_id);
		// The team was stored on the RoomSessionLevel
		character->SetTeamColorAndCollision(team);
		// The weapon was stored on the CharacterSelectLevel
		character->SetWeapon(weapon);
	}
}

void
ASagaInGamePlayerController::OnUpdatePosition(int32 user_id, float x, float y, float z)
{
	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (user_id == system->GetLocalUserId()) // 로컬 클라이언트
	{
	}
	else // 원격 클라이언트
	{
		auto character = system->GetCharacterHandle(user_id);
		if (not IsValid(character))
		{
			UE_LOG(LogSagaGame, Error, TEXT("[OnUpdatePosition] Cannot find a character of remote player %d'."), user_id);
			return;
		}

		UE_LOG(LogSagaGame, Log, TEXT("[OnUpdatePosition] Rotating remote player %d by (%f,%f,%f)."), user_id, x, y, z);
		character->SetActorLocation(FVector{ x, y, z });
	}
}

void
ASagaInGamePlayerController::OnUpdateRotation(int32 user_id, float p, float y, float r)
{
	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (user_id == system->GetLocalUserId()) // 로컬 클라이언트
	{
	}
	else // 원격 클라이언트
	{
		auto character = system->GetCharacterHandle(user_id);
		if (not IsValid(character))
		{
			UE_LOG(LogSagaGame, Error, TEXT("[OnUpdateRotation] Cannot find a character of remote player %d'."), user_id);
			return;
		}

		UE_LOG(LogSagaGame, Log, TEXT("[OnUpdateRotation] Rotating remote player %d by (%f,%f,%f)."), user_id, p, y, r);
		character->SetActorRotation(FRotator{ p, y, r });
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
	UE_LOG(LogSagaGame, Log, TEXT("[OnLevelReady] Game controller would send a loaded packet"));

	if constexpr (not saga::IsOfflineMode)
	{
		auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		if (nullptr != system and system->GetLocalUserId() != -1)
		{
			UE_LOG(LogSagaGame, Log, TEXT("[OnLevelReady] Sending GameIsLoadedPacket..."));
			system->SendGameIsLoadedPacket();
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("[OnLevelReady] Network subsystem is not ready, so give player default properties."));

			OnCreatingCharacter(0, EUserTeam::Red, EPlayerWeapon::LightSabor);
		}
	}
	else
	{
		OnCreatingCharacter(0, EUserTeam::Red, EPlayerWeapon::LightSabor);
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
