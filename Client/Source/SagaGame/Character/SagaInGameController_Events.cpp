#include "SagaInGamePlayerController.h"
#include <Templates/Casts.h>

#include "SagaCharacterPlayer.h"
#include "SagaPlayableCharacter.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

void
ASagaInGamePlayerController::OnUpdateTransform()
{
	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

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
			system->SendRpcPacket(ESagaRpcProtocol::RPC_POSITION, arg0, arg1);

			lastCharacterPosition = loc;
		}

		const auto rot = pawn->GetActorRotation();
		if (lastCharacterRotation != rot)
		{
			int64 arg0{};
			int32 arg1{};

			SerializePosition(FVector{ rot.Pitch, rot.Yaw, rot.Roll }, arg0, arg1);

			//system->SendRotationPacket(rot.Pitch, rot.Yaw, rot.Roll);
			system->SendRpcPacket(ESagaRpcProtocol::RPC_ROTATION, arg0, arg1);

			lastCharacterRotation = rot;
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
			UE_LOG(LogSagaGame, Error, TEXT("[OnLevelReady] Clent is not connected to server!"));
		}
	}
	else
	{
		UE_LOG(LogSagaGame, Warning, TEXT("[OnLevelReady] (Offline Mode)"));
	}
}

void
ASagaInGamePlayerController::OnGameStarted()
{
	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not system->IsOfflineMode())
	{
		UE_LOG(LogSagaGame, Log, TEXT("[OnGameStarted] Game is started."));

		UE_LOG(LogSagaGame, Log, TEXT("[OnGameStarted] Starting periodic transformer timer."));
		GetWorldTimerManager().SetTimer(transformUpdateTimer, this, &ASagaInGamePlayerController::OnUpdateTransform, 0.03f, true, 1.0f);
	}
	else
	{
		UE_LOG(LogSagaGame, Log, TEXT("[OnGameStarted] Game is started. (Offline Mode)"));
	}
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
			system->SetCharacterHandle(user_id, nullptr);
			character->Destroy();
		}
	}
	else
	{
		UE_LOG(LogSagaGame, Error, TEXT("[OnLeftRoom] Could not find the user %d"), user_id);
	}
}
