#include "SagaInGamePlayerController.h"
#include "SagaCharacterPlayer.h"
#include "SagaPlayableCharacter.h"

#include "Character/SagaCharacterSpawner.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

void
ASagaInGamePlayerController::OnCreatingCharacter(int32 user_id, EUserTeam team, EPlayerWeapon weapon)
{
	FTransform transform{};
	transform.TransformPosition({ 0, 0, 300 });

	const auto handle = playerSpawners.Find(team);
	ensure(handle != nullptr);

	const auto spawner = *handle;
	ensure(spawner != nullptr);

	const auto world = GetWorld();
	const auto system = USagaNetworkSubSystem::GetSubSystem(world);

	if (not system->IsOfflineMode())
	{
		if (not system->IsConnected())
		{
			UE_LOG(LogSagaGame, Error, TEXT("[OnCreatingCharacter] Network subsystem is not ready."));
			return;
		}

		const auto character_class = ASagaPlayableCharacter::StaticClass();

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

			// The id was stored on LobbyLevel
			character->SetUserId(user_id);
			// The team was stored on RoomSessionLevel
			character->SetTeamColorAndCollision(team);
			// The weapon was stored on CharacterSelectLevel
			character->SetWeapon(weapon);
			character->AttachWeapon(weapon);

			// Making a spawn point
			const auto pos = spawner->GetInitialSpawnPoint();

			// store the initial spawn point (local)
			system->StorePosition(user_id, pos.X, pos.Y, pos.Z);

			UE_LOG(LogSagaGame, Log, TEXT("[OnCreatingCharacter] Local user `%d` created a spawn point at (%f,%f,%f)"), user_id, pos.X, pos.Y, pos.Z);

			int64 arg0{};
			int32 arg1{};
			SerializePosition(pos, arg0, arg1);

			// let others know this
			SendRpc(ESagaRpcProtocol::RPC_POSITION, arg0, arg1);

			//character->SetActorLocation(system->GetStoredPosition(user_id));
			character->SetActorLocation(pos);
		}
		else
		{
			UE_LOG(LogSagaGame, Log, TEXT("[OnCreatingCharacter] User `%d` would create a playable character."), user_id);

			auto character = Cast<ASagaCharacterPlayer>(CreatePlayableCharacter(character_class, transform));

			if (nullptr != character)
			{
				system->SetCharacterHandle(user_id, character);

				// The id was stored on LobbyLevel
				character->SetUserId(user_id);
				// The team was stored on RoomSessionLevel
				character->SetTeamColorAndCollision(team);
				// The weapon was stored on CharacterSelectLevel
				character->SetWeapon(weapon);
				character->AttachWeapon(weapon);
				// The position was backed up on CharacterSelectLevel or SagaGameLevel from another client 
				character->SetActorLocation(system->GetStoredPosition(user_id));
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

		auto character = GetPawn<ASagaCharacterPlayer>();
		system->SetCharacterHandle(user_id, character);

		character->SetUserId(user_id);
		character->SetTeamColorAndCollision(team);
		character->SetWeapon(weapon);
		character->AttachWeapon(weapon);
	}
}

void
ASagaInGamePlayerController::OnUpdateTransform()
{
	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not system->IsOfflineMode())
	{
		//UE_LOG(LogSagaGame, Log, TEXT("[Character] Synchronizing Transform"));

		if (system->IsConnected())
		{
			const auto pawn = GetPawn();

			const auto loc = pawn->GetActorLocation();
			if (lastCharacterPosition != loc)
			{
				int64 arg0{};
				int32 arg1{};

				SerializePosition(loc, arg0, arg1);

				//system->SendPositionPacket(loc.X, loc.Y, loc.Z);
				SendRpc(ESagaRpcProtocol::RPC_POSITION, arg0, arg1);

				lastCharacterPosition = loc;
			}

			const auto rot = pawn->GetActorRotation();
			if (lastCharacterRotation != rot)
			{
				int64 arg0{};
				int32 arg1{};

				SerializePosition(FVector{ rot.Pitch, rot.Yaw, rot.Roll }, arg0, arg1);

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
	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not system->IsOfflineMode())
	{
		if (system->IsConnected())
		{
			UE_LOG(LogSagaGame, Log, TEXT("[OnLevelReady] Game controller would send a loaded packet"));
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
		UE_LOG(LogSagaGame, Warning, TEXT("[OnLevelReady] (Offline Mode)"));

		OnCreatingCharacter(0, EUserTeam::Red, system->GetLocalUserWeapon());
	}
}

void
ASagaInGamePlayerController::OnGameStarted()
{
	UE_LOG(LogSagaGame, Log, TEXT("[OnGameStarted] Starting a periodic transformer timer"));

	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not system->IsOfflineMode())
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
		UE_LOG(LogSagaGame, Warning, TEXT("[OnGameStarted] Network subsystem is offline mode."));
	}

	GetWorldTimerManager().SetTimer(transformUpdateTimer, this, &ASagaInGamePlayerController::OnUpdateTransform, 0.05f, true, 2.0f);
}

void
ASagaInGamePlayerController::OnLeftRoom(int32 user_id)
{
	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	FSagaVirtualUser user{};

	if (system->FindUser(user_id, user))
	{
		const auto character = user.GetCharacterHandle();

		if (IsValid(character))
		{
			character->Destroy();
		}
	}
	else
	{
		UE_LOG(LogSagaGame, Error, TEXT("[OnLeftRoom] Could not find the user %d"), user_id);
	}
}

void
ASagaInGamePlayerController::OnLeftRoomBySelf()
{
	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not system->IsConnected())
	{
		UGameplayStatics::OpenLevel(this, TEXT("MainMenuLevel"));
	}
	else
	{
		UGameplayStatics::OpenLevel(this, TEXT("LobbyLevel"));
	}
}
