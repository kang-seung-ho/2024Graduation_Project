#include "SagaInGameMode.h"
#include <Engine/World.h>
#include <UObject/Object.h>
#include <Templates/Casts.h>

#include "Character/SagaCharacterPlayer.h"
#include "Character/SagaGummyBearPlayer.h"

#include "Saga/Network/SagaRpcProtocol.h"
#include "Saga/Network/SagaVirtualUser.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

void
ASagaInGameMode::OnRpc(ESagaRpcProtocol cat, int32 id, int64 arg0, int32 arg1)
{
	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	bool is_remote{};
	FSagaVirtualUser user{};

	if (not system->FindUser(id, user))
	{
		UE_LOG(LogSagaGame, Error, TEXT("[RPC] Cannot find remote user %d."), id);
		return;
	}

	ASagaCharacterPlayer* character = user.GetCharacterHandle();

	if (id == system->GetLocalUserId()) // 로컬 클라이언트
	{
#pragma region RPC from Local Client
		//UE_LOG(LogSagaGame, Log, TEXT("[RPC] This is my rpc message."));
#pragma endregion
	}
	else // 원격 클라이언트
	{
#pragma region RPC from Remote Client
		//UE_LOG(LogSagaGame, Log, TEXT("[RPC] This is user %d's rpc message."), id);
		is_remote = true;
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
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_BEG_WALK] by user %d, no character."), id);
			break;
		}

		if (is_remote)
		{
			//UE_LOG(LogSagaGame, Log, TEXT("[RPC][Remote] Begin Walking"));
		}
		else
		{
			//UE_LOG(LogSagaGame, Log, TEXT("[RPC] Begin Walking"));
		}

		const auto xdir = static_cast<int>(arg0);
		if (0 != xdir)
		{
			character->ExecuteStrafeWalk(xdir);
		}

		const auto ydir = static_cast<int>(arg1);
		if (0 != ydir)
		{
			character->ExecuteStraightWalk(ydir);
		}
	}
	break;

	case ESagaRpcProtocol::RPC_END_WALK:
	{
		if (not IsValid(character))
		{
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_END_WALK] by user %d, no character."), id);
			break;
		}

		if (is_remote)
		{
			//UE_LOG(LogSagaGame, Log, TEXT("[RPC][Remote] End Walking"));
		}
		else
		{
			//UE_LOG(LogSagaGame, Log, TEXT("[RPC] End Walking"));
		}

		const auto xdir = static_cast<int>(arg0);
		if (0 == xdir)
		{
			character->ExecuteStrafeWalk(xdir);
		}

		const auto ydir = static_cast<int>(arg1);
		if (0 == ydir)
		{
			character->ExecuteStraightWalk(ydir);
		}
	}
	break;

	case ESagaRpcProtocol::RPC_BEG_RUN:
	{
		if (not IsValid(character))
		{
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_BEG_RUN] by user %d, no character."), id);
			break;
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
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_END_RUN] by user %d, no character."), id);
			break;
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
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_BEG_JUMP] by user %d, no character."), id);
			break;
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
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_BEG_RIDE] by user %d, no character."), id);
			break;
		}

		const auto guardian_id = arg0;
		const auto guardian_info = arg1;

		//UE_LOG(LogSagaGame, Log, TEXT("[RPC] Begin Ride"));
		if (is_remote)
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC][Remote] user '%d' Begin ride guardian '%d'"), id, guardian_id);

			FActorSpawnParameters SpawnParams{};
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 충돌 처리 설정

			const auto World = GetWorld();

			const auto old_character = character;
			const auto& transform = old_character->GetTransform();
			FVector location = old_character->GetActorLocation();
			FRotator rotation = old_character->GetActorRotation();

			const auto bear = World->SpawnActor<ASagaGummyBearPlayer>((ASagaGummyBearPlayer::StaticClass(), location, rotation, SpawnParams));
			UGameplayStatics::FinishSpawningActor(bear, transform);
			//bear->SetTre

			/*
				old_character의 속성은 ASagaInGamePlayerController::OnCreatingCharacter에서
				이미 가져왔으므로 현재 읽은 FSagaVirtualUser는 수정할 필요없음
			*/
			// 곰에 속성 전달
			old_character->TranslateProperties(bear);

			// 파괴
			old_character->Destroy();

			// 혹시 character를 쓸 일이 있으면 대입해줘야 함
			character = bear;

			system->SetCharacterHandle(id, bear);
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
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_END_RIDE] by user %d, no character."), id);
			break;
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
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_POSITION] Cannot find a character of user %d'."), id);

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

			//UE_LOG(LogSagaGame, Log, TEXT("[RPC] Positioning remote player %d to (%f,%f,%f)."), id, x, y, z);
			character->SetActorLocation(FVector{ x, y, z });
		}
	}
	break;

	case ESagaRpcProtocol::RPC_ROTATION:
	{
		if (not IsValid(character))
		{
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_ROTATION] by user %d, no character."), id);
			break;
		}

		if (is_remote)
		{
			float p{};
			float y{};
			float r{};

			std::memcpy(&p, &arg0, 4);
			std::memcpy(&y, reinterpret_cast<char*>(&arg0) + 4, 4);
			std::memcpy(&r, &arg1, 4);

			//UE_LOG(LogSagaGame, Log, TEXT("[RPC] Rotating remote player %d by (%f,%f,%f)."), id, p, y, r);
			character->SetActorRotation(FRotator{ p, y, r });
		}
	}
	break;

	case ESagaRpcProtocol::RPC_BEG_ATTACK_0:
	{
		if (IsValid(character))
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC_BEG_ATTACK_0] by user %d."), id);
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_BEG_ATTACK_0] by user %d, no character."), id);
			break;
		}

		if (is_remote)
		{
		}

		character->ExecuteAttack();
	}
	break;

	case ESagaRpcProtocol::RPC_END_ATTACK_0:
	{
		if (IsValid(character))
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC_END_ATTACK_0] by user %d."), id);
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_END_ATTACK_0] by user %d, no character."), id);
			break;
		}

		if (is_remote)
		{
		}

		character->TerminateAttack();
	}
	break;

	case ESagaRpcProtocol::RPC_BEG_ATTACK_1:
	{
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

		const auto name = UEnum::GetValueAsString(new_weapon);

		UE_LOG(LogSagaGame, Warning, TEXT("[RPC] %d's weapon is %s"), id, *name);
	}
	break;

	case ESagaRpcProtocol::RPC_DMG_PLYER:
	{
		if (IsValid(character))
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC_DMG_PLYER] by user %d."), id);
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_DMG_PLYER] by user %d, no character."), id);
			break;
		}

		float dmg{};
		memcpy(&dmg, reinterpret_cast<const char*>(&arg0), 4);

		UE_LOG(LogSagaGame, Warning, TEXT("[RPC_DMG_PLYER] User %d is damaged for %f"), id, dmg);

		if (is_remote)
		{

		}

		// TODO: RPC_DMG_PLYER
		//character->ExecuteHurt(dmg);
	}
	break;

	case ESagaRpcProtocol::RPC_DMG_GUARDIAN:
	{
		if (IsValid(character))
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC_DMG_GUARDIAN] by user %d."), id);
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_DMG_GUARDIAN] by user %d, no character."), id);
			break;
		}

		if (is_remote)
		{

		}
	}
	break;

	case ESagaRpcProtocol::RPC_DMG_GUARDIANS_PART:
	{
		if (IsValid(character))
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC_DMG_GUARDIANS_PART] by user %d."), id);
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_DMG_GUARDIANS_PART] by user %d, no character."), id);
			break;
		}

		if (is_remote)
		{

		}
	}
	break;

	case ESagaRpcProtocol::RPC_DMG_BASE:
	{
		if (is_remote)
		{

		}
	}
	break;

	case ESagaRpcProtocol::RPC_DEAD:
	{
		if (IsValid(character))
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC_DEAD] User %d is dead."), id);
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_DEAD] User %d is dead, no character."), id);
			break;
		}

		// TODO: RPC_DEAD
		character->ExecuteDeath();

		EUserTeam user_team{};
		if (system->GetTeam(id, user_team))
		{
			const auto team_name = UEnum::GetValueAsString(user_team);

			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_DEAD] Give a score to team '%s'."), *team_name);
			system->AddScore(user_team == EUserTeam::Red ? EUserTeam::Blue : EUserTeam::Red, 1);
		}
	}
	break;

	case ESagaRpcProtocol::RPC_RESPAWN:
	{
		if (IsValid(character))
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC_RESPAWN] User %d is respawning..."), id);
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_RESPAWN] User %d is respawning, no character."), id);
			break;
		}

		// TODO: RPC_RESPAWN
		character->ExecuteRespawn();
	}
	break;
	
	case ESagaRpcProtocol::RPC_RESPAWN_TIMER:
	{
		UE_LOG(LogSagaGame, Warning, TEXT("[RPC_RESPAWN_TIMER] Time: %d"), arg0);
	}
	break;

	default:
		break;
	}
}
