#include "SagaInGamePlayerController.h"
#include <Templates/Casts.h>

#include "Character/SagaCharacterBase.h"
#include "SagaPlayableCharacter.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

void
ASagaInGamePlayerController::HandleUpdateTransform()
{
	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (system->IsConnected())
	{
		const auto pawn = GetPawn<ASagaCharacterBase>();

		if (pawn->IsAlive())
		{
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
	else
	{
		GetWorldTimerManager().ClearTimer(transformUpdateTimer);
	}
}

void
ASagaInGamePlayerController::OnGameStarted()
{
	const auto world = GetWorld();
	const auto system = USagaNetworkSubSystem::GetSubSystem(world);

	if (not system->IsOfflineMode())
	{
		if (system->IsConnected())
		{
			UE_LOG(LogSagaGame, Log, TEXT("[ASagaInGamePlayerController][OnGameStarted] Starting periodic timers."));

			GetWorldTimerManager().SetTimer(transformUpdateTimer, this, &ASagaInGamePlayerController::HandleUpdateTransform, 0.03f, true, 1.0f);
		}
		else
		{
			UE_LOG(LogSagaGame, Error, TEXT("[ASagaInGamePlayerController][OnGameStarted] Game controller could not start periodic timers!"));
		}
	}
}
