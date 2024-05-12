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
		character = user.GetCharacterHandle();
#pragma endregion
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
		if (not IsValid(character))
		{
			UE_LOG(LogSagaGame, Error, TEXT("[RPC][BEG_WALK] Cannot find a character of user %d'."), id);
			return;
		}

		if (is_remote)
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC][Remote] Begin Walking"));
		}
		else
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC] Begin Walking"));
		}

		const auto xdir = static_cast<int>(arg0);
		const auto ydir = static_cast<int>(arg1);

		character->ExecuteStrafeWalk(xdir);
		character->ExecuteStraightWalk(ydir);
	}
	break;

	case ESagaRpcProtocol::RPC_END_WALK:
	{
		if (not IsValid(character))
		{
			UE_LOG(LogSagaGame, Error, TEXT("[RPC][END_WALK] Cannot find a character of user %d'."), id);
			return;
		}

		if (is_remote)
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC][Remote] End Walking"));
		}
		else
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC] End Walking"));
		}

		const auto xdir = static_cast<int>(arg0);
		const auto ydir = static_cast<int>(arg1);

		character->ExecuteStrafeWalk(xdir);
		character->ExecuteStraightWalk(ydir);
	}
	break;

	case ESagaRpcProtocol::RPC_BEG_RUN:
	{
		if (not IsValid(character))
		{
			UE_LOG(LogSagaGame, Error, TEXT("[RPC][BEG_RUN] Cannot find a character of user %d'."), id);
			return;
		}

		if (is_remote)
		{

		}

		character->ExecuteRun();
	}
	break;

	case ESagaRpcProtocol::RPC_END_RUN:
	{
		if (not IsValid(character))
		{
			UE_LOG(LogSagaGame, Error, TEXT("[RPC][END_RUN] Cannot find a character of user %d'."), id);
			return;
		}

		if (is_remote)
		{

		}

		character->TerminateRun();
	}
	break;

	case ESagaRpcProtocol::RPC_BEG_JUMP:
	{
		if (not IsValid(character))
		{
			UE_LOG(LogSagaGame, Error, TEXT("[RPC][BEG_JUMP] Cannot find a character of user %d'."), id);
			return;
		}

		if (is_remote)
		{
		}

		character->ExecuteJump();
	}
	break;

	// 수호자 탑승
	case ESagaRpcProtocol::RPC_BEG_RIDE:
	{
		if (not IsValid(character))
		{
			UE_LOG(LogSagaGame, Error, TEXT("[RPC][BEG_RIDE] Cannot find a character of user %d'."), id);
			return;
		}

		UE_LOG(LogSagaGame, Log, TEXT("[RPC] Begin Ride"));
		if (is_remote)
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC][Remote] Ride guardian"));

			FActorSpawnParameters SpawnParams{};
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 충돌 처리 설정

			auto guardian_id = arg0;
			auto guardian_info = arg1;

			auto old_character = user.GetCharacterHandle();

			UWorld* World = GetWorld();
			FVector location{};
			FRotator rotation{};

			if (IsValid(old_character))
			{
				location = old_character->GetActorLocation();
				rotation = old_character->GetActorRotation();
				old_character->Destroy();
			}

			auto bear = World->SpawnActor<ASagaGummyBearPlayer>((ASagaGummyBearPlayer::StaticClass(), location, rotation, SpawnParams));

			// 곰에 속성 전달
			if (IsValid(old_character))
			{
				/*
					old_character의 속성은 ASagaInGamePlayerController::OnCreatingCharacter에서
					이미 가져왔으므로 서브시스템에 있는 FSagaVirtualUser는 수정할 필요없음
				*/
				old_character->TranslateProperties(bear);
			}

			// 혹시 character를 쓸 일이 있으면 대입해줘야 함
			character = bear;

			user.SetCharacterHandle(bear);
		}
		else
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC][Local] Ride guardian"));
		}

		//character->ExecuteRide();
	}
	break;

	// 수호자 하차
	case ESagaRpcProtocol::RPC_END_RIDE:
	{
		if (not IsValid(character))
		{
			UE_LOG(LogSagaGame, Error, TEXT("[RPC][END_RIDE] Cannot find a character of user %d'."), id);
			return;
		}

		UE_LOG(LogSagaGame, Log, TEXT("[RPC] Take off guardian"));

		if (is_remote)
		{
			//character->TerminateRide();
		}
	}
	break;
	
	case ESagaRpcProtocol::RPC_POSITION:
	{
		if (not IsValid(character))
		{
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC][POSITION] Cannot find a character of user %d'."), id);

			float x{};
			float y{};
			float z{};

			std::memcpy(&x, &arg0, 4);
			std::memcpy(&y, reinterpret_cast<char*>(&arg0) + 4, 4);
			std::memcpy(&z, &arg1, 4);

			system->StorePosition(id, x, y, z);
		}
		else if (is_remote)
		{
			float x{};
			float y{};
			float z{};

			std::memcpy(&x, &arg0, 4);
			std::memcpy(&y, reinterpret_cast<char*>(&arg0) + 4, 4);
			std::memcpy(&z, &arg1, 4);

			UE_LOG(LogSagaGame, Log, TEXT("[RPC] Positioning remote player %d to (%f,%f,%f)."), id, x, y, z);
			character->SetActorLocation(FVector{ x, y, z });
		}
	}
	break;
	
	case ESagaRpcProtocol::RPC_ROTATION:
	{
		if (is_remote)
		{
			float p{};
			float y{};
			float r{};

			std::memcpy(&p, &arg0, 4);
			std::memcpy(&y, reinterpret_cast<char*>(&arg0) + 4, 4);
			std::memcpy(&r, &arg1, 4);

			UE_LOG(LogSagaGame, Log, TEXT("Rotating remote player %d by (%f,%f,%f)."), id, p, y, r);
			character->SetActorRotation(FRotator{ p, y, r });
		}
	}
	break;

	case ESagaRpcProtocol::RPC_BEG_ATTACK_0:
	{
		if (is_remote)
		{
		}

		character->ExecuteAttack();
	}
	break;

	case ESagaRpcProtocol::RPC_END_ATTACK_0:
	{
		if (is_remote)
		{
		}

		character->TerminateAttack();
	}
	break;

	case ESagaRpcProtocol::RPC_BEG_ATTACK_1:
	{
		character->ExecuteAttack();
		if (is_remote)
		{
			
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

	case ESagaRpcProtocol::RPC_MAIN_WEAPON:
	{
		const auto new_weapon = static_cast<EPlayerWeapon>(arg0);
		system->SetWeapon(id, new_weapon);

		EPlayerWeapon test{};
		system->GetWeapon(id, test);
		ensure(new_weapon == test);

		UE_LOG(LogSagaGame, Warning, TEXT("[RPC] %d's weapon is %d"), id, static_cast<int>(test));
	}
	break;

	case ESagaRpcProtocol::RPC_DEAD:
	{}
	break;

	default:
		break;
	}
}

bool
ASagaInGamePlayerController::SendRpc(ESagaRpcProtocol rpc, const int64 arg0, const int32 arg1)
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
