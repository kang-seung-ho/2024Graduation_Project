#include "SagaInGameMode.h"
#include <Engine/World.h>
#include <Engine/DamageEvents.h>
#include <UObject/Object.h>
#include <Templates/Casts.h>

#include "PlayerControllers/SagaInGamePlayerController.h"
#include "Character/SagaCharacterBase.h"
#include "Character/SagaPlayableCharacter.h"
#include "Character/SagaGummyBearPlayer.h"
#include "Obstacles/MapObstacle1.h"
#include "Item/SagaItemTypes.h"
#include "Item/SagaItemBox.h"
#include "Interface/SagaCharacterItemInterface.h"

#include "Saga/Network/SagaRpcProtocol.h"
#include "Saga/Network/SagaVirtualUser.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

void
ASagaInGameMode::OnRpc(ESagaRpcProtocol cat, int32 id, int64 arg0, int32 arg1)
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	bool is_remote{};

	ASagaCharacterBase* character = net->GetCharacterHandle(id);

	if (id == net->GetLocalUserId()) // 로컬 클라이언트
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
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_BEG_WALK] by user %d, has no character."), id);
			break;
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

	// 수호자 상하차
	case ESagaRpcProtocol::RPC_BEG_RIDE:
	{
		if (not IsValid(character))
		{
			UE_LOG(LogSagaGame, Error, TEXT("[RPC_BEG_RIDE] by user %d, has no character."), id);
			break;
		}

		const auto human = Cast<ASagaPlayableCharacter>(character);

		if (not IsValid(human))
		{
			UE_LOG(LogSagaGame, Error, TEXT("[RPC_BEG_RIDE] User %d's character is not a human."), id);
			break;
		}

		const auto guardian_info = arg0;
		const auto guardian_id = arg1;

		const auto guardian = FindBear(guardian_id);

		if (IsValid(guardian))
		{
			if (not guardian->IsAlive())
			{
#if WITH_EDITOR

				UE_LOG(LogSagaGame, Warning, TEXT("[RPC_BEG_RIDE] The guardian with id %d is dead."), guardian_id);
#endif
				break;
			}

#if WITH_EDITOR

			const auto guardian_name = guardian->GetName();

			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_BEG_RIDE] User %d would ride a guardian '%s' with id %d."), id, *guardian_name, guardian_id);
#endif

			net->SetCharacterHandle(id, guardian);

			human->ExecuteGuardianAction(guardian);
			guardian->ExecuteGuardianAction(human);

			if (is_remote)
			{
#if WITH_EDITOR

				UE_LOG(LogSagaGame, Log, TEXT("[RPC][Remote] User %d rides guardian %d"), id, guardian_id);
#endif
			}
			else // IF (is_remote)
			{
#if WITH_EDITOR

				UE_LOG(LogSagaGame, Log, TEXT("[RPC][Local] User %d rides guardian %d"), id, guardian_id);
#endif

				storedLocalPlayerController->Possess(guardian);
			} // IF NOT (is_remote)
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_BEG_RIDE] There is no guardian with id %d."), guardian_id);
		}
	}
	break;

	// 수호자 하차
	case ESagaRpcProtocol::RPC_END_RIDE:
	{
		if (not IsValid(character))
		{
			UE_LOG(LogSagaGame, Error, TEXT("[RPC_END_RIDE] by user %d, no character."), id);

			break;
		}

		const auto guardian = Cast<ASagaGummyBearPlayer>(character);

		if (not IsValid(guardian))
		{
			UE_LOG(LogSagaGame, Error, TEXT("[RPC_END_RIDE] User %d's character is not a guardian."), id);

			break;
		}

		ASagaCharacterBase* human = guardian->ownerData.GetCharacterHandle();

		net->SetCharacterHandle(id, human);

		guardian->TerminateGuardianAction();
		human->TerminateGuardianAction();

		if (is_remote)
		{
#if WITH_EDITOR

			UE_LOG(LogSagaGame, Log, TEXT("[RPC][Remote] User %d unrides from guardian %d"), id, guardian->GetBearId());
#endif
		}
		else
		{
#if WITH_EDITOR

			UE_LOG(LogSagaGame, Log, TEXT("[RPC][Local] User %d unrides from guardian %d"), id, guardian->GetBearId());
#endif

			storedLocalPlayerController->Possess(human);
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

			net->StorePosition(id, x, y, z);
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
			UE_LOG(LogSagaGame, Error, TEXT("[RPC_BEG_ATTACK_0] by user %d, has no character."), id);

			break;
		}

		character->ExecuteAttackAnimation();
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
			UE_LOG(LogSagaGame, Error, TEXT("[RPC_END_ATTACK_0] by user %d, has no character."), id);
			break;
		}

		if (is_remote)
		{
		}

		character->TerminateAttack();
	}
	break;

	case ESagaRpcProtocol::RPC_SKILL_0:
	{}
	break;

	case ESagaRpcProtocol::RPC_DESTROY_ITEM_BOX:
	{
#if WITH_EDITOR

		UE_LOG(LogSagaGame, Log, TEXT("[RPC_DESTROY_ITEM_BOX] target item id: %d"), arg1);
#endif

		for (auto& item_spawner : everyItemSpawnEntities)
		{
			if (IsValid(item_spawner.Get()) and item_spawner->GetID() == arg1)
			{
				//const auto box = item_spawner->SpawnItemBox();
				//ensure(IsValid(box));

				//everyItemBoxes.Add(box);

#if WITH_EDITOR

				//const auto name = box->GetName();

				//UE_LOG(LogSagaGame, Log, TEXT("[RPC_DESTROY_ITEM_BOX] item %s is spawned."), *name);
#endif

				//item_spawner->Destroy();
				//everyItemSpawnEntities.RemoveSwap(item_spawner);
				break;
			}
		}
	}
	break;

	case ESagaRpcProtocol::RPC_GRAB_ITEM:
	{
		UE_LOG(LogSagaGame, Log, TEXT("[RPC_GRAB_ITEM] item id: %d"), arg1);

		for (auto& item_spawner : everyItemSpawnEntities)
		{
			if (IsValid(item_spawner.Get()) and item_spawner->GetID() == arg1)
			{
#if WITH_EDITOR

				const auto name = item_spawner->GetName();

				UE_LOG(LogSagaGame, Log, TEXT("[RPC_GRAB_ITEM] item spawner %s is destroyed."), *name);
#endif

				item_spawner->Destroy();
				break;
			}
		}

		for (auto& item_box : everyItemBoxes)
		{
			if (IsValid(item_box.Get()) and item_box->GetItemId() == arg1)
			{
#if WITH_EDITOR

				const auto name = item_box->GetName();

				UE_LOG(LogSagaGame, Log, TEXT("[RPC_GRAB_ITEM] item %d (%s) is destroyed."), arg1, *name);
#endif

				item_box->Destroy();
				//everyItemBoxes.RemoveSwap(item_box);
				break;
			}
		}

		if (not is_remote)
		{
			ISagaCharacterItemInterface* item_inf = Cast<ISagaCharacterItemInterface>(character);

			if (item_inf)
			{
				const auto rng_item = FMath::RandRange(0, 2);

				UE_LOG(LogSagaGame, Log, TEXT("[RPC_GRAB_ITEM] User %d is getting item."), rng_item);

				item_inf->TakeItem(static_cast<ESagaItemTypes>(rng_item));
			}
		}
	}
	break;

	case ESagaRpcProtocol::RPC_USE_ITEM_0:
	{
		UE_LOG(LogSagaGame, Log, TEXT("[RPC_USE_ITEM_0] item id: %d"), arg1);

		if (is_remote)
		{
			//storedLocalPlayerController->inventory
		}
		else
		{

		}

		switch (arg1)
		{
		case 0:
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC_USE_ITEM_0] Using Energy Drink"));

			character->ExecuteUseItem(ESagaItemTypes::Drink);
		}
		break;

		case 1:
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC_USE_ITEM_0] Gumball"));

			character->ExecuteUseItem(ESagaItemTypes::Gum);
		}
		break;

		case 2:
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC_USE_ITEM_0] Smoke Bomb"));

			character->ExecuteUseItem(ESagaItemTypes::SmokeBomb);
		}
		break;

		default:
		{
			UE_LOG(LogSagaGame, Error, TEXT("[USagaInventoryWidget] Invalid item id"));
		}
		break;
		}
	}
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
		net->SetWeapon(id, new_weapon);

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
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_DMG_PLYER] by user %d, has no character."), id);

			break;
		}

		float dmg{};
		memcpy(&dmg, reinterpret_cast<const char*>(&arg0), 4);

		UE_LOG(LogSagaGame, Log, TEXT("[RPC_DMG_PLYER] User %d is damaged for %f"), id, dmg);

		const int32 causer_id = arg1;

		FSagaVirtualUser other_user{};
		if (net->FindUser(causer_id, other_user))
		{
			const auto other_character = other_user.GetCharacterHandle();

			if (IsValid(other_character))
			{
				UE_LOG(LogSagaGame, Log, TEXT("[RPC_DMG_PLYER] User %d is taking damage from user %d."), id, causer_id);
			}
			else
			{
				UE_LOG(LogSagaGame, Error, TEXT("[RPC_DMG_PLYER] Causer %d does not have a character."), causer_id);
			}
		}
		else if (-2 == causer_id)
		{
			///TODO: 된다면 작은 곰에도 ID 부여해야함, 나중에 확인 필요
			UE_LOG(LogSagaGame, Log, TEXT("[RPC_DMG_PLYER] User %d is taking damage from the mini bear."), id);
		}
		else
		{
			UE_LOG(LogSagaGame, Error, TEXT("[RPC_DMG_PLYER] Causer %d does not exist."), causer_id);
		}

		character->ExecuteHurt(dmg);
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
			UE_LOG(LogSagaGame, Warning, TEXT("[RPC_DMG_GUARDIAN] by user %d, has no character."), id);

			break;
		}

		const auto guardian_info = arg0;
		const auto guardian_id = arg1;

		// 현재 id는 수호자(guardian)를 타고 있다가 데미지를 받은 플레이어임.
		const auto guardian = FindBear(guardian_id);

		if (IsValid(guardian))
		{
			if (not guardian->IsAlive())
			{
#if WITH_EDITOR

				UE_LOG(LogSagaGame, Warning, TEXT("[RPC_DMG_GUARDIAN] The guardian with id %d is already dead."), guardian_id);
#endif

				break;
			}

			float dmg{};
			memcpy(&dmg, reinterpret_cast<const char*>(&arg0), 4);

			UE_LOG(LogSagaGame, Log, TEXT("[RPC_DMG_GUARDIAN] Guardian %d is damaged for %f"), guardian_id, dmg);

			guardian->ExecuteHurt(dmg);

			if (not guardian->IsAlive())
			{
#if WITH_EDITOR

				UE_LOG(LogSagaGame, Warning, TEXT("[RPC_DMG_GUARDIAN] The guardian with id %d is dead."), guardian_id);

				const auto guardian_name = guardian->GetName();

				UE_LOG(LogSagaGame, Warning, TEXT("[RPC_DMG_GUARDIAN] User %d would unride from the guardian '%s' with id %d."), id, *guardian_name, guardian_id);
#endif

				// 사망 시에는 하차 처리를 함.
				ASagaCharacterBase* human = guardian->ownerData.GetCharacterHandle();

				if (IsValid(human))
				{
					net->SetCharacterHandle(id, human);

					human->TerminateGuardianAction();
					guardian->TerminateGuardianAction();

					if (is_remote)
					{
#if WITH_EDITOR

						UE_LOG(LogSagaGame, Log, TEXT("[RPC_DMG_GUARDIAN][Remote] User %d would unride from the guardian %d"), id, guardian_id);
#endif
					}
					else // IF (is_remote)
					{
#if WITH_EDITOR

						UE_LOG(LogSagaGame, Log, TEXT("[RPC_DMG_GUARDIAN][Local] User %d would unride from the guardian %d"), id, guardian_id);
#endif

						storedLocalPlayerController->Possess(human);
					} // IF NOT (is_remote)
				}
			}
		}
		else
		{
			UE_LOG(LogSagaGame, Error, TEXT("[RPC_BEG_RIDE] There is no guardian with id %d."), guardian_id);
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
			UE_LOG(LogSagaGame, Error, TEXT("[RPC_DEAD] User %d is dead, has no character."), id);

			break;
		}

		character->ExecuteDeath();
	}
	break;

	case ESagaRpcProtocol::RPC_RESPAWN:
	{
		if (IsValid(character))
		{
			UE_LOG(LogSagaGame, Log, TEXT("[RPC_RESPAWN] User %d is respawning..."), id);

			character->ExecuteRespawn();
		}
		else
		{
			ESagaPlayerTeam team{};
			if (net->GetTeam(id, team))
			{
				if (is_remote)
				{
					UE_LOG(LogSagaGame, Warning, TEXT("[RPC_RESPAWN][Remote] User %d is respawning, no character."), id);


				}
				else
				{
					UE_LOG(LogSagaGame, Warning, TEXT("[RPC_RESPAWN][Local] User %d is respawning, no character."), id);


				}
			}
			else
			{
				UE_LOG(LogSagaGame, Error, TEXT("[RPC_RESPAWN] User %d could not find its team!"), id);

				return;
			}

			break;
		}
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
