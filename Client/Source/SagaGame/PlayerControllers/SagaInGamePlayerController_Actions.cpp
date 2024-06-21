#include "PlayerControllers/SagaInGamePlayerController.h"
#include <Logging/LogMacros.h>
#include <Containers/UnrealString.h>
#include <UObject/Object.h>
#include <InputActionValue.h>

#include "Character/SagaCharacterBase.h"
#include "Character/SagaPlayableCharacter.h"
#include "Character/SagaGummyBearPlayer.h"

#include "Saga/Network/SagaVirtualUser.h"
#include "Saga/Network/SagaRpcProtocol.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

void
PrintVector(const FVector& vector)
{
	const FString str = vector.ToString();
	//UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] Movement Vector: (%s)"), *str);
}

void
ASagaInGamePlayerController::BeginForwardWalk(const FInputActionValue& input)
{
	//UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] Begin Walking Straight"));

	auto dir = input.Get<FVector>();

	walkDirection.Y = input.Get<FVector>().Y;
	PrintVector(walkDirection);

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (net->IsOfflineMode())
	{
		const auto character = GetPawn<ASagaCharacterBase>();

		character->ExecuteStraightWalk(static_cast<int>(walkDirection.Y));
		character->ExecuteStrafeWalk(static_cast<int>(walkDirection.X));
	}
	else
	{
		net->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_WALK, walkDirection.X, walkDirection.Y);
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
	//UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] End Walking Straight"));

	walkDirection.Y = input.Get<FVector>().Y;
	PrintVector(walkDirection);

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (net->IsOfflineMode())
	{
		const auto character = GetPawn<ASagaCharacterBase>();

		character->ExecuteStraightWalk(static_cast<int>(walkDirection.Y));
		character->ExecuteStrafeWalk(static_cast<int>(walkDirection.X));
	}
	else
	{
		net->SendRpcPacket(ESagaRpcProtocol::RPC_END_WALK, walkDirection.X, walkDirection.Y);
	}
}

void
ASagaInGamePlayerController::BeginStrafeWalk(const FInputActionValue& input)
{
	//UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] Begin Walking Strafe"));

	walkDirection.X = input.Get<FVector>().X;
	PrintVector(walkDirection);

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (net->IsOfflineMode())
	{
		const auto character = GetPawn<ASagaCharacterBase>();

		character->ExecuteStraightWalk(static_cast<int>(walkDirection.Y));
		character->ExecuteStrafeWalk(static_cast<int>(walkDirection.X));
	}
	else
	{
		net->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_WALK, walkDirection.X, walkDirection.Y);
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
	//UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] End Walking Strafe"));

	walkDirection.X = input.Get<FVector>().X;
	PrintVector(walkDirection);

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (net->IsOfflineMode())
	{
		const auto character = GetPawn<ASagaCharacterBase>();

		character->ExecuteStraightWalk(static_cast<int>(walkDirection.Y));
		character->ExecuteStrafeWalk(static_cast<int>(walkDirection.X));
	}
	else
	{
		net->SendRpcPacket(ESagaRpcProtocol::RPC_END_WALK, walkDirection.X, walkDirection.Y);
	}
}

void
ASagaInGamePlayerController::BeginRun()
{
	//UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] Begin Running"));

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (net->IsOfflineMode())
	{
		const auto character = GetPawn<ASagaCharacterBase>();

		character->ExecuteRun();
	}
	else
	{
		net->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_RUN);
	}
}

void
ASagaInGamePlayerController::EndRun()
{
	//UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] End Running"));

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (net->IsOfflineMode())
	{
		const auto character = GetPawn<ASagaCharacterBase>();

		character->TerminateRun();
	}
	else
	{
		net->SendRpcPacket(ESagaRpcProtocol::RPC_END_RUN);
	}
}

void
ASagaInGamePlayerController::BeginJump()
{
	//UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] Begin Jumping"));

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (net->IsOfflineMode())
	{
		const auto character = GetPawn<ASagaCharacterBase>();

		character->ExecuteJump();
	}
	else
	{
		net->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_JUMP);
	}
}

void
ASagaInGamePlayerController::EndJump()
{
	//UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] End Jumping"));

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (net->IsOfflineMode())
	{
		const auto pawn = GetPawn<ASagaCharacterBase>();

		pawn->TerminateJump();
	}
	else
	{
		net->SendRpcPacket(ESagaRpcProtocol::RPC_END_JUMP);
	}
}

void
ASagaInGamePlayerController::BeginRotate(const FInputActionValue& input)
{
	const FVector InputValue = input.Get<FVector>();

	const auto pawn = GetPawn<ASagaCharacterBase>();

	if (IsValid(pawn))
	{
		if (pawn->IsAlive())
		{
			AddYawInput(InputValue.X);
		}

		pawn->ExecuteRotate(InputValue.Y);
	}
}

void
ASagaInGamePlayerController::BeginAttack(const FInputActionValue& input)
{
	UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] Begin Attack"));

	if (not isAttacking)
	{
		const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());
		const auto pawn = GetPawn<ASagaCharacterBase>();

		if (pawn->IsAlive())
		{
			if (net->IsOfflineMode())
			{
				pawn->ExecuteAttackAnimation();
			}
			else
			{
				net->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_ATTACK_0);
			}
		}

		isAttacking = true;
	}
}

void
ASagaInGamePlayerController::EndAttack()
{
	UE_LOG(LogSagaGame, Log, TEXT("[Local][Controller] End Attack"));

	if (isAttacking)
	{
		auto character = GetPawn<ASagaCharacterBase>();

		const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		if (net->IsOfflineMode())
		{
		}
		else
		{
			net->SendRpcPacket(ESagaRpcProtocol::RPC_END_ATTACK_0);
		}

		isAttacking = false;
	}
}

void
ASagaInGamePlayerController::BeginGuardianAction()
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (net->IsOfflineMode())
	{
		UE_LOG(LogSagaGame, Log, TEXT("[BeginGuardianAction] Started. (Offline Mode)"));

		// 사람 캐릭터
		const auto human = GetPawn<ASagaPlayableCharacter>();

		if (IsValid(human))
		{
			const auto guardian = human->GetNeareastCollidedBear();

			// 승차
			if (IsValid(guardian))
			{
#if WITH_EDITOR

				const auto guardian_id = guardian->GetBearId();
				const auto guardian_name = guardian->GetName();

				UE_LOG(LogSagaGame, Warning, TEXT("[BeginGuardianAction] Riding a bear '%s' with id %d. (Offline Mode)"), *guardian_name, guardian_id);
#endif

				net->SetCharacterHandle(net->GetLocalUserId(), guardian);

				human->ExecuteGuardianAction(guardian);
				guardian->ExecuteGuardianAction(human);

				Possess(guardian);
			}
			else
			{
#if WITH_EDITOR

				UE_LOG(LogSagaGame, Warning, TEXT("[BeginGuardianAction] There is no bear nearby. (Offline Mode)"));
#endif
			}
		}
		else
		{
			// 수호자 캐릭터
			const auto guardian = GetPawn<ASagaGummyBearPlayer>();

			// 하차
			if (IsValid(guardian))
			{
				ASagaCharacterBase* backup = guardian->ownerData.GetCharacterHandle();

				net->SetCharacterHandle(net->GetLocalUserId(), backup);

				guardian->TerminateGuardianAction();
				backup->TerminateGuardianAction();

				Possess(backup);
			}
		}
	}
	else // IF (Offline Mode)
	{
		const auto human = GetPawn<ASagaPlayableCharacter>();

		if (IsValid(human))
		{
			const auto guardian = human->GetNeareastCollidedBear();

			// 승차
			if (IsValid(guardian))
			{
#if WITH_EDITOR

				UE_LOG(LogSagaGame, Log, TEXT("[BeginGuardianAction] Trying to ride a guardian with id %d."), guardian->GetBearId());
#endif

				net->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_RIDE, 0, guardian->GetBearId());
			}
		}
		else
		{
#if WITH_EDITOR

			UE_LOG(LogSagaGame, Log, TEXT("[BeginGuardianAction] Current player character is not human."));
#endif
			// 수호자 캐릭터
			const auto guardian = GetPawn<ASagaGummyBearPlayer>();

			// 하차
			if (IsValid(guardian))
			{
				net->SendRpcPacket(ESagaRpcProtocol::RPC_END_RIDE, 0, guardian->GetBearId());
			}
		}
	} // IF NOT (Offline Mode)
}
