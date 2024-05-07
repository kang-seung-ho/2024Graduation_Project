#include "SagaInGamePlayerController.h"

#include "SagaCharacterPlayer.h"
#include "Saga/Network/SagaRpcProtocol.h"

void
PrintVector(const FVector& vector)
{
	const FString str = vector.ToString();
	UE_LOG(LogSagaGame, Log, TEXT("[Local][Character] Movement Vector: (%s)"), *str);
}

void
ASagaInGamePlayerController::BeginForwardWalk(const FInputActionValue& input)
{
	UE_LOG(LogSagaGame, Log, TEXT("[Local][Character] Begin Walking Straight"));

	walkDirection.Y = input.Get<FVector>().Y;
	PrintVector(walkDirection);

	SendRpc(ESagaRpcProtocol::RPC_BEG_WALK, walkDirection.X, walkDirection.Y);
}

void
ASagaInGamePlayerController::EndForwardWalk(const FInputActionValue& input)
{
	UE_LOG(LogSagaGame, Log, TEXT("[Character] End Walking Straight"));

	walkDirection.Y = input.Get<FVector>().Y;
	PrintVector(walkDirection);

	SendRpc(ESagaRpcProtocol::RPC_END_WALK, walkDirection.X, walkDirection.Y);
}

void
ASagaInGamePlayerController::BeginStrafeWalk(const FInputActionValue& input)
{
	UE_LOG(LogSagaGame, Log, TEXT("[Local][Character] Begin Walking Strafe"));

	walkDirection.X = input.Get<FVector>().X;
	PrintVector(walkDirection);

	SendRpc(ESagaRpcProtocol::RPC_BEG_WALK, walkDirection.X, walkDirection.Y);
}

void
ASagaInGamePlayerController::EndStrafeWalk(const FInputActionValue& input)
{
	UE_LOG(LogSagaGame, Log, TEXT("[Local][Character] End Walking Strafe"));

	walkDirection.X = input.Get<FVector>().X;
	PrintVector(walkDirection);

	SendRpc(ESagaRpcProtocol::RPC_END_WALK, walkDirection.X, walkDirection.Y);
}

void
ASagaInGamePlayerController::BeginRun()
{
	UE_LOG(LogSagaGame, Warning, TEXT("[Local][Character] Begin Running"));

	SendRpc(ESagaRpcProtocol::RPC_BEG_RUN);
}

void
ASagaInGamePlayerController::EndRun()
{
	UE_LOG(LogSagaGame, Warning, TEXT("[Local][Character] End Running"));

	SendRpc(ESagaRpcProtocol::RPC_END_RUN);
}

void
ASagaInGamePlayerController::BeginJump()
{
	UE_LOG(LogSagaGame, Warning, TEXT("[Local][Character] Begin Jumping"));

	SendRpc(ESagaRpcProtocol::RPC_BEG_JUMP);
}

void
ASagaInGamePlayerController::EndJump()
{
	UE_LOG(LogSagaGame, Warning, TEXT("[Local][Character] End Jumping"));

	SendRpc(ESagaRpcProtocol::RPC_END_JUMP);
}

void
ASagaInGamePlayerController::BeginRotate(const FInputActionValue& input)
{
	wasTilted = true;

	const FVector InputValue = input.Get<FVector>();

	AddYawInput(InputValue.X);

	ASagaCharacterPlayer* ControlledPawn = GetPawn<ASagaCharacterPlayer>();
	if (ControlledPawn != nullptr)
	{
		ControlledPawn->RotationCameraArm(InputValue.Y);
	}
}

void
ASagaInGamePlayerController::EndRotate(const FInputActionValue& input)
{
	//UE_LOG(LogSagaGame, Log, TEXT("[Local][Character] End Rotating"));

	wasTilted = false;
}

void
ASagaInGamePlayerController::BeginAttack()
{
	SendRpc(ESagaRpcProtocol::RPC_BEG_ATTACK_0);
}

void
ASagaInGamePlayerController::EndAttack()
{
	SendRpc(ESagaRpcProtocol::RPC_END_ATTACK_0);
}

void
ASagaInGamePlayerController::BeginRide()
{
	SendRpc(ESagaRpcProtocol::RPC_BEG_RIDE, 0);
}

void
ASagaInGamePlayerController::EndRide()
{
	SendRpc(ESagaRpcProtocol::RPC_END_RIDE, 0);
}
