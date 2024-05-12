#include "SagaInGamePlayerController.h"

#include "SagaCharacterPlayer.h"
#include "Saga/Serializer.h"
#include "Saga/Network/SagaNetworkSettings.h"
#include "Saga/Network/SagaRpcProtocol.h"

void
PrintVector(const FVector& vector)
{
	const FString str = vector.ToString();
	UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] Movement Vector: (%s)"), *str);
}

void
ASagaInGamePlayerController::BeginForwardWalk(const FInputActionValue& input)
{
	UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] Begin Walking Straight"));

	auto dir = input.Get<FVector>();

	walkDirection.Y = input.Get<FVector>().Y;
	PrintVector(walkDirection);

	if constexpr (saga::IsOfflineMode)
	{
		auto character = GetPawn<ASagaCharacterPlayer>();

		character->ExecuteStraightWalk(static_cast<int>(walkDirection.Y));
		character->ExecuteStrafeWalk(static_cast<int>(walkDirection.X));
	}
	else
	{
		SendRpc(ESagaRpcProtocol::RPC_BEG_WALK, walkDirection.X, walkDirection.Y);
	}
}

void
ASagaInGamePlayerController::MidForwardWalk(const FInputActionValue& input)
{
	if (walkDirection.Y != input.Get<FVector>().Y)
	{
		BeginForwardWalk(input);
	}
}

void
ASagaInGamePlayerController::EndForwardWalk(const FInputActionValue& input)
{
	UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] End Walking Straight"));

	walkDirection.Y = input.Get<FVector>().Y;
	PrintVector(walkDirection);

	if constexpr (saga::IsOfflineMode)
	{
		auto character = GetPawn<ASagaCharacterPlayer>();

		character->ExecuteStraightWalk(static_cast<int>(walkDirection.Y));
		character->ExecuteStrafeWalk(static_cast<int>(walkDirection.X));
	}
	else
	{
		SendRpc(ESagaRpcProtocol::RPC_END_WALK, walkDirection.X, walkDirection.Y);
	}
}

void
ASagaInGamePlayerController::BeginStrafeWalk(const FInputActionValue& input)
{
	UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] Begin Walking Strafe"));

	walkDirection.X = input.Get<FVector>().X;
	PrintVector(walkDirection);

	if constexpr (saga::IsOfflineMode)
	{
		auto character = GetPawn<ASagaCharacterPlayer>();

		character->ExecuteStraightWalk(static_cast<int>(walkDirection.Y));
		character->ExecuteStrafeWalk(static_cast<int>(walkDirection.X));
	}
	else
	{
		SendRpc(ESagaRpcProtocol::RPC_BEG_WALK, walkDirection.X, walkDirection.Y);
	}
}

void
ASagaInGamePlayerController::MidStrafeWalk(const FInputActionValue& input)
{
	if (walkDirection.X != input.Get<FVector>().X)
	{
		BeginStrafeWalk(input);
	}
}

void
ASagaInGamePlayerController::EndStrafeWalk(const FInputActionValue& input)
{
	UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] End Walking Strafe"));

	walkDirection.X = input.Get<FVector>().X;
	PrintVector(walkDirection);

	if constexpr (saga::IsOfflineMode)
	{
		auto character = GetPawn<ASagaCharacterPlayer>();

		character->ExecuteStraightWalk(static_cast<int>(walkDirection.Y));
		character->ExecuteStrafeWalk(static_cast<int>(walkDirection.X));
	}
	else
	{
		SendRpc(ESagaRpcProtocol::RPC_END_WALK, walkDirection.X, walkDirection.Y);
	}
}

void
ASagaInGamePlayerController::BeginRun()
{
	UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] Begin Running"));

	if constexpr (saga::IsOfflineMode)
	{
		auto character = GetPawn<ASagaCharacterPlayer>();

		character->ExecuteRun();
	}
	else
	{
		SendRpc(ESagaRpcProtocol::RPC_BEG_RUN);
	}
}

void
ASagaInGamePlayerController::EndRun()
{
	UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] End Running"));

	if constexpr (saga::IsOfflineMode)
	{
		auto character = GetPawn<ASagaCharacterPlayer>();

		character->TerminateRun();
	}
	else
	{
		SendRpc(ESagaRpcProtocol::RPC_END_RUN);
	}
}

void
ASagaInGamePlayerController::BeginJump()
{
	UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] Begin Jumping"));

	if constexpr (saga::IsOfflineMode)
	{
		auto character = GetPawn<ASagaCharacterPlayer>();

		character->ExecuteJump();
	}
	else
	{
		SendRpc(ESagaRpcProtocol::RPC_BEG_JUMP);
	}
}

void
ASagaInGamePlayerController::EndJump()
{
	UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] End Jumping"));

	if constexpr (saga::IsOfflineMode)
	{
		auto pawn = GetPawn<ASagaCharacterPlayer>();

		pawn->TerminateJump();
	}
	else
	{
		SendRpc(ESagaRpcProtocol::RPC_END_JUMP);
	}
}

void
ASagaInGamePlayerController::BeginRotate(const FInputActionValue& input)
{
	const FVector InputValue = input.Get<FVector>();

	AddYawInput(InputValue.X);

	auto pawn = GetPawn<ASagaCharacterPlayer>();
	if (pawn != nullptr)
	{
		pawn->RotateCameraArm(InputValue.Y);
	}
}

void
ASagaInGamePlayerController::BeginAttack(const FInputActionValue& input)
{
	if constexpr (saga::IsOfflineMode)
	{
		auto pawn = GetPawn<ASagaCharacterPlayer>();

		pawn->PlayAttackAnimation();
	}
	else
	{
		if (not isAttacking)
		{
			isAttacking = true;

			SendRpc(ESagaRpcProtocol::RPC_BEG_ATTACK_0);
		}
	}
}

void
ASagaInGamePlayerController::EndAttack()
{
	if constexpr (saga::IsOfflineMode)
	{
		auto character = GetPawn<ASagaCharacterPlayer>();
	}
	else
	{
		if (isAttacking)
		{
			SendRpc(ESagaRpcProtocol::RPC_END_ATTACK_0);

			isAttacking = false;
		}
	}
}

void
ASagaInGamePlayerController::BeginRide()
{
	if constexpr (saga::IsOfflineMode)
	{
		auto character = GetPawn<ASagaCharacterPlayer>();
	}
	else
	{
		SendRpc(ESagaRpcProtocol::RPC_BEG_RIDE, 0);
	}
}

void
ASagaInGamePlayerController::EndRide()
{
	if constexpr (saga::IsOfflineMode)
	{
		auto character = GetPawn<ASagaCharacterPlayer>();
	}
	else
	{
		SendRpc(ESagaRpcProtocol::RPC_END_RIDE, 0);
	}
}
