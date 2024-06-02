#include "Saga/Level/SagaGameLevel.h"
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
{
	SetNextLevelName(TEXT("GameVictoryLevel"));
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

	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not system->IsOfflineMode())
	{
		system->OnDisconnected.AddDynamic(this, &ASagaGameLevel::OnDisconnected);
		system->OnLeftRoomBySelf.AddDynamic(this, &ASagaGameLevel::OnLeftRoomBySelf);
		system->OnRpc.AddDynamic(this, &ASagaGameLevel::OnRpc);
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
	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (system->IsConnected())
	{
		SetPrevLevelName(TEXT("LobbyLevel"));
	}

	GotoPrevLevel();
}

void
ASagaGameLevel::OnRpc(ESagaRpcProtocol cat, int32 id, int64 arg0, int32 arg1)
{
	if (cat == ESagaRpcProtocol::RPC_GAME_TIMER)
	{
		GetWorldTimerManager().PauseTimer(countdownTimerHandle);

		const auto rep = FMath::Max(0LL, arg0 / 1000'0000LL);
		UE_LOG(LogSagaFramework, Log, TEXT("[ASagaGameLevel][OnRpc] Countdown: %d"), rep);

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
	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (system->IsOfflineMode())
	{
		if (--storedGameTime <= 0)
		{
			GotoNextLevel();
		}
	}
	else if (system->IsConnected())
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_GAME_TIMER);
	}
}
