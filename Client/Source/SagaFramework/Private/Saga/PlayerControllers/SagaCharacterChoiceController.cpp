#include "Saga/PlayerControllers/SagaCharacterChoiceController.h"
#include <Misc/AssertionMacros.h>
#include <Logging/LogMacros.h>
#include <HAL/Platform.h>
#include <Engine/EngineTypes.h>
#include <Engine/HitResult.h>
#include <Engine/LocalPlayer.h>
#include <UObject/Class.h>
#include <UObject/UObjectGlobals.h>
#include <Containers/UnrealString.h>
#include <InputActionValue.h>
#include <InputTriggers.h>
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include <Templates/Casts.h>

#include "SagaGame/Player/SagaPlayerWeaponTypes.h"
#include "SagaGame/Input/SagaInputSystem.h"
#include "SagaGame/CharacterChoice/SagaSelectCharacter.h"

#include "Saga/Network/SagaRpcProtocol.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaCharacterChoiceController::ASagaCharacterChoiceController(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, OnClickedCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void
ASagaCharacterChoiceController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI mode{};
	SetInputMode(mode);

	SetShowMouseCursor(true);

	const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	const auto InputSystem = GetDefault<USagaCharacterSelectInputSystem>();
	Subsystem->AddMappingContext(InputSystem->DefaultContext, 0);
}

void
ASagaCharacterChoiceController::SetupInputComponent()
{
	Super::SetupInputComponent();

	auto input = Cast<UEnhancedInputComponent>(InputComponent);
	ensure(nullptr != input);

	const auto input_sytem = GetDefault<USagaCharacterSelectInputSystem>();

	input->BindAction(input_sytem->Click, ETriggerEvent::Completed, this, &ASagaCharacterChoiceController::OnClick);
}

void
ASagaCharacterChoiceController::OnClick(const FInputActionValue& Value)
{
	FHitResult result;
	bool Hit = GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel5, false, result);

	if (Hit)
	{
		UE_LOG(LogSagaFramework, Warning, TEXT("[ASagaCharacterChoiceController][OnClick] Mouse on a character"));

		const auto character = Cast<ASagaSelectCharacter>(result.GetActor());

		if (nullptr != character)
		{
			const EPlayerWeapon weapon = character->GetWeapon();

#if WITH_EDITOR
			const FString name = UEnum::GetValueAsString(weapon);
			UE_LOG(LogSagaFramework, Warning, TEXT("Selected weapon: %s"), *name);
#endif

			const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

			if (not net->IsOfflineMode())
			{
				if (net->IsConnected())
				{
					net->SendRpcPacket(ESagaRpcProtocol::RPC_MAIN_WEAPON, static_cast<int64>(weapon));
				}
			}
			else
			{
				net->SetLocalUserWeapon(weapon);
			}
		}

		if (OnClickedCharacter.IsBound())
		{
			OnClickedCharacter.Broadcast(character);
		}
	}
}
