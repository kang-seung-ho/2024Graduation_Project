#include "SagaGameSubsystem.h"
#include <Containers/UnrealString.h>
#include <UObject/Object.h>
#include <Engine/World.h>
#include <GameFramework/Actor.h>
#include <Subsystems/SubsystemCollection.h>

#include "Player/SagaPlayerTeam.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

USagaGameSubsystem::USagaGameSubsystem()
	: Super()
	, localPlayerSpawner()
	, gameWinnerId(-1)
	, RedTeamScore(), BluTeamScore()
	, RedTeamPinataBroken(), BluTeamPinataBroken()
{}

void
USagaGameSubsystem::Initialize(FSubsystemCollectionBase& collection)
{

}

void
USagaGameSubsystem::Deinitialize()
{

}

bool
USagaGameSubsystem::ShouldCreateSubsystem(UObject* outer)
const
{
	return true;
}

USagaGameSubsystem*
USagaGameSubsystem::GetSubSystem(const UWorld* world)
{
	const auto singleton = world->GetGameInstance();

	return singleton->GetSubsystem<USagaGameSubsystem>();
}

void
USagaGameSubsystem::AssignLocalPlayerSpawner(AActor* spawner)
noexcept
{
	localPlayerSpawner = spawner;
}

void
USagaGameSubsystem::SetScore(ESagaPlayerTeam team, int32 score)
noexcept
{
	const auto world = GetWorld();
	const auto net = USagaNetworkSubSystem::GetSubSystem(world);

	if (team == ESagaPlayerTeam::Red)
	{
		RedTeamScore = score;
	}
	else
	{
		BluTeamScore = score;
	}
}

void
USagaGameSubsystem::AddScore(ESagaPlayerTeam team, int32 score)
noexcept
{
	const auto world = GetWorld();
	const auto net = USagaNetworkSubSystem::GetSubSystem(world);

	if (team == ESagaPlayerTeam::Red)
	{
		RedTeamScore += score;
	}
	else
	{
		BluTeamScore += score;
	}
}

void
USagaGameSubsystem::SetWinner(const int32 winner_team_id)
noexcept
{
	gameWinnerId = winner_team_id;
}

bool
USagaGameSubsystem::TrySetWinner(const int32 winner_team_id)
noexcept
{
	if (gameWinnerId == -1)
	{
		gameWinnerId = winner_team_id;

		return true;
	}
	else
	{
		return false;
	}
}

void
USagaGameSubsystem::SetWhoWonByPinata(int32 TeamPinataColor)
noexcept
{
	if (gameWinnerId == -1)
	{
		const auto world = GetWorld();
		const auto net = USagaNetworkSubSystem::GetSubSystem(world);

		if (TeamPinataColor == 0) //redteam's pinata broken
		{
			SetWinner(2);

			RedTeamPinataBroken = true;
		}
		else
		{
			SetWinner(1);

			BluTeamPinataBroken = true;
		}
	}
}

void
USagaGameSubsystem::CacheLocalPlayerTeam(ESagaPlayerTeam team)
noexcept
{
	localPlayerTeam = team;
}

void
USagaGameSubsystem::ClearWinner()
noexcept
{
	gameWinnerId = -1;
}

int32
USagaGameSubsystem::GetWinnerId()
const noexcept
{
	return gameWinnerId;
}

FString
USagaGameSubsystem::GetWhoWon()
{
	const auto world = GetWorld();
	const auto net = USagaNetworkSubSystem::GetSubSystem(world);

	if (net->IsOfflineMode())
	{
		if (RedTeamPinataBroken)
		{
			return TEXT("Blue Win!"); // I'm in Blue Team And My team Win
		}
		else if (BluTeamPinataBroken)
		{
			return TEXT("Red Win!");
		}
		else if (RedTeamScore > BluTeamScore)
		{
			return TEXT("Red Win!");
		}
		else if (RedTeamScore < BluTeamScore)
		{
			return TEXT("Blue Win!");
		}
		else
		{
			return TEXT("Draw"); // Draw
		}
	}
	else
	{
		if (gameWinnerId == 3)
		{
			return TEXT("Draw");
		}
		else if (localPlayerTeam == ESagaPlayerTeam::Red)
		{
			if (gameWinnerId == 1)
			{
				return TEXT("Victory");
			}
			else
			{
				return TEXT("Lose");
			}
		}
		else if (localPlayerTeam == ESagaPlayerTeam::Blue)
		{
			if (gameWinnerId == 2)
			{
				return TEXT("Victory");
			}
			else
			{
				return TEXT("Lose");
			}
		}
		else
		{
			return TEXT("Error");
		}
	}
}

const AActor*
USagaGameSubsystem::GetLocalPlayerSpawner()
const noexcept
{
	return localPlayerSpawner;
}

int32
USagaGameSubsystem::GetRedTeamScore()
const noexcept
{
	return RedTeamScore;
}

int32
USagaGameSubsystem::GetBlueTeamScore()
const noexcept
{
	return BluTeamScore;
}
