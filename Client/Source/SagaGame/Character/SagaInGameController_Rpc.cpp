#include "SagaInGamePlayerController.h"
#include "Character/SagaCharacterPlayer.h"
#include "SagaGummyBearPlayer.h"

#include "Saga/Network/SagaNetworkSettings.h"
#include "Saga/Network/SagaNetworkSubSystem.h"
#include "Saga/Network/SagaRpcProtocol.h"
#include "Saga/Network/SagaVirtualUser.h"

void
ASagaInGamePlayerController::OnRpc(ESagaRpcProtocol cat, int32 id, int64 arg0, int32 arg1)
{
	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	bool is_remote{};
	FSagaVirtualUser user{};
	ASagaCharacterPlayer* character{};

	if (id == system->GetLocalUserId()) // 로컬 클라이언트
	{
#pragma region RPC from Local Client
		UE_LOG(LogSagaGame, Log, TEXT("[RPC] This is my rpc message."));

		character = GetPawn<ASagaCharacterPlayer>();
#pragma endregion
	}
	else // 원격 클라이언트
	{
#pragma region RPC from Remote Client

		if (not system->FindUser(id, user))
		{
			UE_LOG(LogSagaGame, Error, TEXT("[RPC] Cannot find remote user %d."), id);
			return;
		}

		UE_LOG(LogSagaGame, Log, TEXT("[RPC] This is user %d's rpc message."), id);

		is_remote = true;
		character = user.remoteCharacter;
#pragma endregion
	}

	if (not IsValid(character))
	{
		UE_LOG(LogSagaGame, Error, TEXT("[RPC] Cannot find a character of user %d'."), id);
		return;
	}

	switch (cat)
	{
	case ESagaRpcProtocol::RPC_UNKNOWN:
	{
		UE_LOG(LogSagaGame, Error, TEXT("[RPC] Cannot execute a rpc script by user %d."), id);
	}
	break;

	case ESagaRpcProtocol::RPC_BEG_WALK:
	{
		UE_LOG(LogSagaGame, Log, TEXT("[RPC] Begin Walking"));

		if (is_remote)
		{
		}

		const auto xdir = static_cast<int>(arg0);
		const auto ydir = static_cast<int>(arg1);

		character->ProcessStrafeWalk(xdir);
		character->ProcessForwardWalk(ydir);
	}
	break;

	case ESagaRpcProtocol::RPC_END_WALK:
	{
		UE_LOG(LogSagaGame, Log, TEXT("[RPC] End Walking"));

		if (is_remote)
		{
		}

		const auto xdir = static_cast<int>(arg0);
		const auto ydir = static_cast<int>(arg1);

		character->ProcessStrafeWalk(xdir);
		character->ProcessForwardWalk(ydir);
	}
	break;

	case ESagaRpcProtocol::RPC_BEG_RUN:
	{
		if (is_remote)
		{
		}

		character->ExecuteRun();
	}
	break;

	case ESagaRpcProtocol::RPC_END_RUN:
	{
		if (is_remote)
		{
		}

		character->TerminateRun();
	}
	break;

	case ESagaRpcProtocol::RPC_BEG_JUMP:
	{
		if (is_remote)
		{}

		character->ExecuteJump();
	}
	break;

	// 수호자 탑승
	case ESagaRpcProtocol::RPC_BEG_RIDE:
	{
		if (is_remote)
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC][Remote] Begin Ride"));

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 충돌 처리 설정
			
			UWorld* World = GetWorld();
			auto BearCharacter = World->SpawnActor<ASagaGummyBearPlayer>((ASagaGummyBearPlayer::StaticClass(), user.remoteCharacter->GetActorLocation(), user.remoteCharacter->GetActorRotation(), SpawnParams));

			if (IsValid(user.remoteCharacter))
			{
				user.remoteCharacter->Destroy();
			}

			user.remoteCharacter = BearCharacter;
		}

		//character->ExecuteRide();
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

bool
ASagaInGamePlayerController::SendRpc(const ESagaRpcProtocol& rpc, const int64 arg0, const int32 arg1)
const
{
	if constexpr (not saga::IsOfflineMode)
	{
		//auto singleton = GEngine->GetWorld()->GetGameInstance();
		//auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();
		auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		if (nullptr != system and system->GetLocalUserId() != -1)
		{
			return 0 < system->SendRpcPacket(rpc, arg0, arg1);
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));

			return false;
		}
	}
	else
	{
		return true;
	}
}
