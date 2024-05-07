#include "SagaInGamePlayerController.h"

#include "SagaCharacterPlayer.h"
#include "Saga/Network/SagaNetworkSettings.h"
#include "Saga/Network/SagaNetworkSubSystem.h"
#include "Saga/Network/SagaRpcProtocol.h"

void
ASagaInGamePlayerController::OnAttack(const FInputActionValue& input)
{
	auto pawn = GetPawn<ASagaCharacterPlayer>();

	pawn->PlayAttackAnimation();
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
			const auto pawn = GetPawn();

			if (wasMoved)
			{
				const auto loc = pawn->K2_GetActorLocation();
				system->SendPositionPacket(loc.X, loc.Y, loc.Z);
			}

			if (wasTilted)
			{
				const auto rot = pawn->K2_GetActorRotation();
				system->SendRotationPacket(rot.Pitch, rot.Yaw, rot.Roll);
			}
		}
		else
		{
			//UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
		}
	}
}

void
ASagaInGamePlayerController::OnGameStarted()
{
	GetWorldTimerManager().SetTimer(tranformUpdateTimer, this, &ASagaInGamePlayerController::OnUpdateTransform, 0.2f, true, 2.0f);
}
