#include "Saga/LevelScripters/SagaCharacterChoiceLevel.h"
#include <Math/UnrealMathUtility.h>
#include <UObject/Object.h>
#include <Delegates/Delegate.h>
#include <Blueprint/UserWidget.h>

#include "Saga/Interface/SagaUiButton.h"
#include "Saga/Interface/SagaTimerUiWidget.h"
#include "Saga/PlayerControllers/SagaCharacterChoiceController.h"

#include "SagaGame/UI/SagaCharacterSelectWidget.h"
#include "SagaGame/Character/CharacterSelect/SagaSelectCharacter.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaCharacterChoiceLevel::ASagaCharacterChoiceLevel()
noexcept
	: Super()
	, levelUiClass(), levelUiInstance()
	, choiceTimer()
{
	SetNextLevelName(TEXT("SagaGameLevel"));
	SetPrevLevelName(TEXT("MainMenuLevel"));

	static ConstructorHelpers::FClassFinder<USagaCharacterSelectWidget> widget_class(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Level/SagaCharacterSelectLevelUI.SagaCharacterSelectLevelUI_C'"));

	if (widget_class.Succeeded())
	{
		levelUiClass = widget_class.Class;
	}
	else
	{
		const auto my_name = GetName();

		UE_LOG(LogSagaFramework, Fatal, TEXT("[ASagaCharacterChoiceController] '%s' could not find the character selection ui class."), *my_name);
	}
}

void
ASagaCharacterChoiceLevel::BeginPlay()
{
	Super::BeginPlay();

	const auto controller = GetWorld()->GetFirstPlayerController<ASagaCharacterChoiceController>();

	if (IsValid(controller))
	{
		auto& callback = controller->OnClickedCharacter;

		callback.AddDynamic(this, &ASagaCharacterChoiceLevel::HandleClickedCharacter);
	}
	else
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[ASagaCharacterChoiceLevel] No selector controller"));
	}

	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not system->IsOfflineMode())
	{
		system->OnDisconnected.AddDynamic(this, &ASagaCharacterChoiceLevel::OnDisconnected);
		system->OnFailedToStartGame.AddDynamic(this, &ASagaCharacterChoiceLevel::OnFailedToStartGame);
		system->OnRpc.AddDynamic(this, &ASagaCharacterChoiceLevel::OnRpc);

		levelUiInstance = CreateWidget<USagaCharacterSelectWidget>(GetWorld(), levelUiClass);
		if (nullptr == levelUiInstance)
		{
			const auto my_name = GetName();

			UE_LOG(LogSagaFramework, Fatal, TEXT("[ASagaCharacterChoiceController] '%s' could not create the level ui for room session."), *my_name);
		}
		else
		{
			levelUiInstance->AddToViewport(1);
		}

		GetWorldTimerManager().SetTimer(choiceTimer, this, &ASagaCharacterChoiceLevel::HandlePeriodicUpdate, 1.0f, true);
	}
}

void
ASagaCharacterChoiceLevel::HandlePeriodicUpdate()
{
	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (system->IsConnected())
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_WEAPON_TIMER);
	}
}

void
ASagaCharacterChoiceLevel::HandleClickedCharacter(ASagaSelectCharacter* character)
{
	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not system->IsOfflineMode())
	{
		if (IsValid(character))
		{
			levelUiInstance->myStartButton->SetUserInteractable(true);
		}
		else
		{
			levelUiInstance->myStartButton->SetUserInteractable(false);
		}
	}
	else
	{
		HandleStartGame();
	}
}

void
ASagaCharacterChoiceLevel::HandleStartGame()
{
	UE_LOG(LogSagaFramework, Log, TEXT("[ASagaCharacterChoiceLevel][HandleStartGame] Let's start game!"));

	GotoNextLevel();
}

void
ASagaCharacterChoiceLevel::OnDisconnected()
{
	GetWorldTimerManager().ClearTimer(choiceTimer);

	GotoPrevLevel();
}

void
ASagaCharacterChoiceLevel::OnFailedToStartGame(ESagaGameContract reason)
{
	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (system->IsConnected() and -1 != system->GetCurrentRoomId())
	{
		SetPrevLevelName(TEXT("LobbyLevel"));
		GotoPrevLevel();
	}
	else
	{
		GotoPrevLevel();
	}
}

void
ASagaCharacterChoiceLevel::OnRpc(ESagaRpcProtocol cat, int32 user_id, int64 arg0, int32 arg1)
{
	if (cat == ESagaRpcProtocol::RPC_WEAPON_TIMER)
	{
		GetWorldTimerManager().PauseTimer(choiceTimer);

		const auto rep = FMath::Max(0LL, arg0 / 1000'0000LL);
		UE_LOG(LogSagaFramework, Log, TEXT("[ASagaCharacterChoiceLevel][OnRpc] Countdown: %d"), rep);

		if (rep <= 0)
		{
			HandleStartGame();
		}
		else
		{
			levelUiInstance->myTimer->SetSeconds(static_cast<int32>(rep));

			GetWorldTimerManager().UnPauseTimer(choiceTimer);
		}
	}
}
