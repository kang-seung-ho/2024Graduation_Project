#include "SagaInGamePlayerController.h"
#include "Math/UnrealMathUtility.h"

#include "SagaCharacterPlayer.h"
#include "Saga/Network/SagaNetworkSettings.h"
#include "Saga/Network/SagaNetworkSubSystem.h"
#include "Saga/Network/SagaRpcProtocol.h"

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
ASagaInGamePlayerController::UpdateMovement(const float& delta_time)
{
	// acceleration
	constexpr double scalar_delta = 100;

	// accelerate toward to the prefered direction
	auto dir_delta = preferedDirection - walkDirection;
	const auto dir_delta_size = dir_delta.Length();

	// make it unit vector
	dir_delta.Normalize();
	walkDirection += dir_delta * FMath::Min(dir_delta_size, scalar_delta * delta_time);
}

void
PrintVector(const FVector& vector)
{
	const FString str = vector.ToString();
	UE_LOG(LogSagaGame, Log, TEXT("[Character] Movement Vector: (%s)"), *str);
}

void
ASagaInGamePlayerController::BeginForwardWalk(const FInputActionValue& Value)
{
	isForwardWalking = true;

	preferedDirection.Y = Value.Get<FVector>().Y;

	PrintVector(preferedDirection);

	if constexpr (not saga::IsOfflineMode)
	{
		UE_LOG(LogSagaGame, Log, TEXT("[Character] Walk Straight"));

		//auto singleton = GEngine->GetWorld()->GetGameInstance();
		//auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();
		auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		if (nullptr != system and system->GetLocalUserId() != -1)
		{
			system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_WALK, GetNormalizedMoveDir());
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
		}
	}
}

void
ASagaInGamePlayerController::EndForwardWalk(const FInputActionValue& Value)
{
	isForwardWalking = false;

	preferedDirection.Y = Value.Get<FVector>().Y;

	PrintVector(preferedDirection);

	if constexpr (not saga::IsOfflineMode)
	{
		auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		if (nullptr != system and system->GetLocalUserId() != -1)
		{
			system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_WALK, GetNormalizedMoveDir());
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
		}
	}
}

void
ASagaInGamePlayerController::BeginStrafeWalk(const FInputActionValue& Value)
{
	isStrafeWalking = true;

	preferedDirection.X = Value.Get<FVector>().X;

	PrintVector(preferedDirection);

	FString KeyAsString = Value.ToString();

	if constexpr (not saga::IsOfflineMode)
	{
		UE_LOG(LogSagaGame, Log, TEXT("[Character] Strafe"));

		auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		if (nullptr != system and system->GetLocalUserId() != -1)
		{
			system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_WALK, GetNormalizedMoveDir());
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
		}
	}
}

void
ASagaInGamePlayerController::EndStrafeWalk(const FInputActionValue& Value)
{
	isStrafeWalking = false;

	preferedDirection.X = Value.Get<FVector>().X;

	PrintVector(preferedDirection);

	if constexpr (not saga::IsOfflineMode)
	{
		auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		if (nullptr != system and system->GetLocalUserId() != -1)
		{
			system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_WALK, GetNormalizedMoveDir());
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
		}
	}
}

void
ASagaInGamePlayerController::BeginRun()
{
	isRunning = true;

	if constexpr (not saga::IsOfflineMode)
	{
		UE_LOG(LogSagaGame, Warning, TEXT("[Character] Run"));

		auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		if (nullptr != system and system->GetLocalUserId() != -1)
		{
			system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_RUN);
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
		}
	}
}

void
ASagaInGamePlayerController::EndRun()
{
	isRunning = false;

	if constexpr (not saga::IsOfflineMode)
	{
		auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		if (nullptr != system and system->GetLocalUserId() != -1)
		{
			system->SendRpcPacket(ESagaRpcProtocol::RPC_END_RUN);
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
		}
	}
}

void
ASagaInGamePlayerController::BeginJump()
{
	if constexpr (not saga::IsOfflineMode)
	{
		auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		if (system->GetLocalUserId() != -1)
		{
			system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_JUMP);
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
		}
	}
}

void
ASagaInGamePlayerController::EndJump()
{
	if constexpr (not saga::IsOfflineMode)
	{
		auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		if (nullptr != system and system->GetLocalUserId() != -1)
		{
			system->SendRpcPacket(ESagaRpcProtocol::RPC_END_JUMP);
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
		}
	}
}

void
ASagaInGamePlayerController::ExecuteWalk(const float& delta_time)
{
	wasMoved = true;

	APawn* ControlledPawn = GetPawn();

	const FRotator Rotation = K2_GetActorRotation();
	const FRotator YawRotation = FRotator(0.0, Rotation.Yaw, 0.0);
	const FVector ForwardVector = YawRotation.Vector();
	const FVector RightVector = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);

	ControlledPawn->AddMovementInput(ForwardVector, walkDirection.Y);
	ControlledPawn->AddMovementInput(RightVector, walkDirection.X);

	mMoveDir = walkDirection.X * 90.f;
	if (walkDirection.Y > 0.f)
	{
		if (walkDirection.X < 0.f)
		{
			mMoveDir = -45.f;
		}
		else if (walkDirection.X > 0.f)
		{
			mMoveDir = 45.f;
		}
	}
	else if (walkDirection.Y < 0.f)
	{
		if (walkDirection.X < 0.f)
		{
			mMoveDir = -135.f;
		}
		else if (walkDirection.X > 0.f)
		{
			mMoveDir = 135.f;
		}
		else
		{
			mMoveDir = 180.f;
		}
	}
}

void
ASagaInGamePlayerController::TerminateWalk(const float& delta_time)
{}

void
ASagaInGamePlayerController::ExecuteRun()
{
	ACharacter* MyCharacter = GetCharacter();
	if (MyCharacter)
	{
		// 달리기 시작할 때 속도를 높임
		MyCharacter->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
}

void
ASagaInGamePlayerController::TerminateRun()
{
	ACharacter* MyCharacter = GetCharacter();
	if (MyCharacter)
	{
		// 달리기를 멈췄을 때 속도를 원래대로 복원
		MyCharacter->GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	}
}

void
ASagaInGamePlayerController::ExecuteJump()
{
	ASagaCharacterPlayer* ControlledPawn = GetPawn<ASagaCharacterPlayer>();
	if (ControlledPawn != nullptr)
	{
		if (ControlledPawn->CanJump())
		{
			ControlledPawn->Jump();
		}
	}
}

void
ASagaInGamePlayerController::TerminateJump()
{

}

double
ASagaInGamePlayerController::GetNormalizedMoveDir()
const noexcept
{
	const auto angle = FMath::RadiansToDegrees(FMath::Atan2(preferedDirection.Y, preferedDirection.X));

	return floor(angle / 45);
}
