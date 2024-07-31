#include "Saga/LevelScripters/SagaGameLevel.h"
#include <HAL/Platform.h>
#include <Math/UnrealMathUtility.h>
#include <UObject/ConstructorHelpers.h>
#include <UObject/Object.h>
#include <Delegates/Delegate.h>
#include <Blueprint/UserWidget.h>

#include "Saga/Interface/SagaGameLevelUiWidget.h"

#include "Saga/Network/SagaRpcProtocol.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaGameLevel::ASagaGameLevel()
noexcept
	: Super()
	, levelUiClass(), levelUiInstance()
	, countdownTimerHandle()
	, storedGameTime(defaultGameSecondsPeriod)
	, choosenWeapon(false)
{
	SetNextLevelName(TEXT("GameEndLevel"));
	SetPrevLevelName(TEXT("MainMenuLevel"));

	static ConstructorHelpers::FClassFinder<USagaGameLevelUiWidget> level_ui{ TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_ScoreBoard.UI_ScoreBoard_C'") };

	if (level_ui.Succeeded())
	{
		levelUiClass = level_ui.Class;
	}
	else
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[ASagaGameLevel][Ctor] Could not find the class of level ui for game."));
	}
}

void
ASagaGameLevel::BeginPlay()
{
	Super::BeginPlay();

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not net->IsOfflineMode())
	{
		net->OnDisconnected.AddDynamic(this, &ASagaGameLevel::OnDisconnected);
		net->OnLeftRoomBySelf.AddDynamic(this, &ASagaGameLevel::OnLeftRoomBySelf);
		net->OnGameStarted.AddDynamic(this, &ASagaGameLevel::OnGameStarted);
		net->OnRpc.AddDynamic(this, &ASagaGameLevel::OnRpc);
	}

	GetWorldTimerManager().SetTimer(countdownTimerHandle, this, &ASagaGameLevel::HandleCountdown, 1.0f, true);

	if (IsValid(levelUiClass))
	{
		levelUiInstance = CreateWidget<USagaGameLevelUiWidget>(GetWorld(), levelUiClass);

		if (IsValid(levelUiInstance))
		{
			levelUiInstance->AddToViewport(1);
		}
	}
}

void
ASagaGameLevel::BeginDestroy()
{
	Super::BeginDestroy();
}

void
ASagaGameLevel::OnDisconnected()
{
	GotoPrevLevel();
}

void
ASagaGameLevel::OnLeftRoomBySelf()
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (net->IsConnected())
	{
		SetPrevLevelName(TEXT("LobbyLevel"));
	}

	GotoPrevLevel();
}

void
ASagaGameLevel::OnGameStarted()
{
	choosenWeapon = true;
}

void
ASagaGameLevel::OnRpc(ESagaRpcProtocol cat, int32 id, int64 arg0, int32 arg1)
{
	if (cat == ESagaRpcProtocol::RPC_WEAPON_TIMER)
	{
		const auto rep = arg0 / 1000'0000LL;

		if (rep <= 0)
		{
			choosenWeapon = true;
		}
	}
	else if (cat == ESagaRpcProtocol::RPC_GAME_TIMER)
	{
		GetWorldTimerManager().PauseTimer(countdownTimerHandle);

		const auto rep = arg0 / 1000'0000LL;
		//UE_LOG(LogSagaFramework, Log, TEXT("[ASagaGameLevel][OnRpc] Countdown: %d"), rep);

		if (rep <= 0)
		{
			GotoNextLevel();
		}
		else
		{
			if (IsValid(levelUiInstance))
			{
				const auto time = FMath::Max(0, static_cast<int32>(rep));
				const auto min = time / 60;
				const auto sec = time % 60;

				levelUiInstance->SetMinutes(min);
				levelUiInstance->SetSeconds(sec);
			}

			GetWorldTimerManager().UnPauseTimer(countdownTimerHandle);
		}
	}
}

void
ASagaGameLevel::HandleCountdown()
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (net->IsOfflineMode())
	{
		if (--storedGameTime <= 0)
		{
			GotoNextLevel();
		}
	}
	else if (net->IsConnected())
	{
		if (not choosenWeapon)
		{
			net->SendRpcPacket(ESagaRpcProtocol::RPC_WEAPON_TIMER);
		}
		else
		{
			net->SendRpcPacket(ESagaRpcProtocol::RPC_GAME_TIMER);
		}
	}
	else
	{
		GetWorldTimerManager().ClearTimer(countdownTimerHandle);
	}
}
