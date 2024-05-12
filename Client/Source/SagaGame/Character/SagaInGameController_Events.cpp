#include "SagaInGamePlayerController.h"
#include "SagaCharacterPlayer.h"

#include "Saga/Network/SagaNetworkSettings.h"
#include "Saga/Network/SagaNetworkSubSystem.h"
#include "SagaPlayableCharacter.h"

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
				character->AttachWeapon(weapon);
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
					character->AttachWeapon(weapon);
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
		character->AttachWeapon(weapon);
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
				const auto x = float(loc.X);
				const auto y = float(loc.Y);
				const auto z = float(loc.Z);

				int64 arg0{};
				int32 arg1{};

				memcpy((char*)(&arg0), &x, 4);
				memcpy((char*)(&arg0) + 4, &y, 4);
				memcpy((char*)(&arg1), &z, 4);

				//system->SendPositionPacket(loc.X, loc.Y, loc.Z);
				SendRpc(ESagaRpcProtocol::RPC_POSITION, arg0, arg1);

				lastCharacterPosition = loc;
			}

			const auto rot = pawn->K2_GetActorRotation();
			if (lastCharacterRotation != rot)
			{
				const auto x = float(rot.Pitch);
				const auto y = float(rot.Yaw);
				const auto z = float(rot.Roll);

				int64 arg0{};
				int32 arg1{};

				memcpy((char*)(&arg0), (char*)(&x), 4);
				memcpy((char*)(&arg0) + 4, (char*)(&y), 4);
				memcpy((char*)(&arg1), (char*)(&z), 4);

				//system->SendRotationPacket(rot.Pitch, rot.Yaw, rot.Roll);
				SendRpc(ESagaRpcProtocol::RPC_ROTATION, arg0, arg1);

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
