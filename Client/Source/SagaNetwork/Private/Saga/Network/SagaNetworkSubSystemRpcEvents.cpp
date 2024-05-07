#include "Saga/Network/SagaNetworkSubSystem.h"
#include "Character/SagaCharacterPlayer.h"
#include "Character/SagaInGamePlayerController.h"
#include "Character/SagaGummyBearPlayer.h"

void
USagaNetworkSubSystem::OnRpc_Implementation(ESagaRpcProtocol cat, int32 id, int64 arg0, int32 arg1)
{
	bool is_remote{};

	FSagaVirtualUser user{};
	auto& character = user.remoteCharacter;

	if (id == GetLocalUserId()) // 로컬 클라이언트
	{
#pragma region RPC from Local Client
		UE_LOG(LogSagaNetwork, Log, TEXT("[SagaGame][RPC] This is my rpc message."));
#pragma endregion
	}
	else // 원격 클라이언트
	{
#pragma region RPC from Remote Client

		if (not FindUser(id, user))
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][RPC] Cannot find remote player %d."), id);
			return;
		}

		UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][RPC] This is user %d's rpc message."), id);

		if (not IsValid(character))
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][RPC] Cannot find a character of user %d'."), id);
			return;
		}

		is_remote = true;
#pragma endregion
	}

	switch (cat)
	{
	case ESagaRpcProtocol::RPC_UNKNOWN:
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("[SagaGame][RPC] Cannot execute a rpc script by user %d."), id);
	}
	break;

	case ESagaRpcProtocol::RPC_BEG_WALK:
	{
		if (is_remote)
		{
			UE_LOG(LogSagaNetwork, Log, TEXT("[SagaGame][RPC][Remote] Begin Walk"));

			const double denorm = static_cast<double>(arg0) * 45.0;

			const auto direction = FMath::DegreesToRadians(denorm);
			const auto xdir = FMath::Cos(direction);
			const auto ydir = FMath::Sin(direction);

			//character->ProcessStrafeWalk(xdir);
			//character->ProcessForwardWalk(ydir);

			switch (arg0)
			{
				// 0º
			case 0:
			{
				//character->BeginStrafeWalk(1);
			}
			break;

			// 45º
			case 1:
			{
				//character->BeginForwardWalk(1);
				//character->BeginStrafeWalk(1);
			}
			break;

			// 90º
			case 2:
			{
				//character->BeginForwardWalk(1);
			}
			break;

			// 135º
			case 3:
			{
				//character->BeginForwardWalk(1);
			}
			break;
			}
		}
	}
	break;

	case ESagaRpcProtocol::RPC_END_WALK:
	{
		if (is_remote)
		{
			UE_LOG(LogSagaNetwork, Log, TEXT("[SagaGame][RPC][Remote] End Walk"));

			const double denorm = static_cast<double>(arg0) * 45.0;

			const auto direction = FMath::DegreesToRadians(denorm);

			// do same process as RPC_BEG_WALK
			character->ProcessStrafeWalk(FMath::Cos(direction));
			character->ProcessForwardWalk(FMath::Sin(direction));
		}
	}
	break;

	case ESagaRpcProtocol::RPC_BEG_RUN:
	{
		if (is_remote)
		{
			//character->ExecuteRun();
		}
	}
	break;

	case ESagaRpcProtocol::RPC_END_RUN:
	{
		if (is_remote)
		{
			//character->TerminateRun();
		}
	}
	break;

	case ESagaRpcProtocol::RPC_BEG_JUMP:
	{
		if (is_remote)
		{
			//character->ExecuteJump();
		}
	}
	break;

	// 수호자 탑승
	case ESagaRpcProtocol::RPC_BEG_RIDE:
	{
		if (is_remote)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 충돌 처리 설정
			UWorld* World = GetWorld();
			auto BearCharacter = World->SpawnActor<ASagaGummyBearPlayer>((ASagaGummyBearPlayer::StaticClass(), user.remoteCharacter->GetActorLocation(), user.remoteCharacter->GetActorRotation(), SpawnParams));
			

			UE_LOG(LogSagaNetwork, Log, TEXT("[SagaGame][RPC][Remote] Begin Ride"));
			if (IsValid(user.remoteCharacter))
			{
				user.remoteCharacter->Destroy();
			}

			user.remoteCharacter = BearCharacter;
		}
	}
	break;

	// 수호자 하차
	case ESagaRpcProtocol::RPC_END_RIDE:
	{
		if (is_remote)
		{
			//character->TerminateRide();
		}
	}
	break;

	case ESagaRpcProtocol::RPC_BEG_ATTACK_0:
	{
		if (is_remote)
		{
			//character->ExecuteAttack();
		}
	}
	break;

	case ESagaRpcProtocol::RPC_END_ATTACK_0:
	{
		if (is_remote)
		{
			//character->TerminateAttack();
		}
	}
	break;

	case ESagaRpcProtocol::RPC_BEG_ATTACK_1:
	{
		if (is_remote)
		{
			//character->ExecuteAttack();
		}
	}
	break;

	case ESagaRpcProtocol::RPC_END_ATTACK_1:
	{
		if (is_remote)
		{
			//character->TerminateAttack();
		}
	}
	break;

	case ESagaRpcProtocol::RPC_BEG_ATTACK_2:
	{}
	break;

	case ESagaRpcProtocol::RPC_BEG_ATTACK_3:
	{}
	break;

	case ESagaRpcProtocol::RPC_SKILL_0:
	{}
	break;

	case ESagaRpcProtocol::RPC_SKILL_1:
	{}
	break;

	case ESagaRpcProtocol::RPC_SKILL_2:
	{}
	break;

	case ESagaRpcProtocol::RPC_SKILL_3:
	{}
	break;

	case ESagaRpcProtocol::RPC_USE_ITEM_0:
	{}
	break;

	case ESagaRpcProtocol::RPC_USE_ITEM_1:
	{}
	break;

	case ESagaRpcProtocol::RPC_USE_ITEM_2:
	{}
	break;

	case ESagaRpcProtocol::RPC_USE_ITEM_3:
	{}
	break;

	case ESagaRpcProtocol::RPC_USE_ITEM_4:
	{}
	break;

	case ESagaRpcProtocol::RPC_CHANGE_HAND_ITEM:
	{}
	break;

	case ESagaRpcProtocol::RPC_DEAD:
	{}
	break;

	default:
		break;
	}
}
