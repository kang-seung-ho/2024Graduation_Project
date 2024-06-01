#include "Saga/Level/SagaCharacterChoiceLevel.h"
#include <UObject/Object.h>
#include <Delegates/Delegate.h>
#include <Blueprint/UserWidget.h>

#include "Saga/Interface/SagaUiButton.h"

#include "SagaGame/Character/CharacterSelect/SagaCharacterSelectController.h"
#include "SagaGame/UI/SagaCharacterSelectWidget.h"
#include "SagaGame/Character/CharacterSelect/SagaSelectCharacter.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaCharacterChoiceLevel::ASagaCharacterChoiceLevel()
noexcept
	: Super()
	, levelUiClass(), levelUiInstance()
	, choiceTimerHandle()
	, countdownTimerHandle()
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

		UE_LOG(LogSagaFramework, Fatal, TEXT("[ASagaCharacterSelectController] '%s' could not find the character selection ui class."), *my_name);
	}
}

void
ASagaCharacterChoiceLevel::BeginPlay()
{
	Super::BeginPlay();

	const auto controller = GetWorld()->GetFirstPlayerController<ASagaCharacterSelectController>();

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

			UE_LOG(LogSagaFramework, Fatal, TEXT("[ASagaCharacterSelectController] '%s' could not create the level ui for room session."), *my_name);
		}
		else
		{
			levelUiInstance->AddToViewport(1);
		}

		GetWorldTimerManager().SetTimer(choiceTimerHandle, this, &ASagaCharacterChoiceLevel::HandlePeriodicUpdate, 1.0f, true);

		//GetWorldTimerManager().SetTimer(countdownTimerHandle, this, &ASagaCharacterChoiceLevel::HandleCompleteCountdown, 30.0f, false);
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
ASagaCharacterChoiceLevel::HandleCompleteCountdown()
{
	UE_LOG(LogSagaFramework, Log, TEXT("[ASagaCharacterChoiceLevel][OnRpc] Let's start game!"));

	GotoNextLevel();
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
		HandleCompleteCountdown();
	}
}

void
ASagaCharacterChoiceLevel::OnDisconnected()
{
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
		if (arg0 <= 1)
		{
			HandleCompleteCountdown();
		}
		else
		{
			UE_LOG(LogSagaFramework, Log, TEXT("[ASagaCharacterChoiceLevel][OnRpc] Countdown: %d"), arg0);
		}
	}
}
