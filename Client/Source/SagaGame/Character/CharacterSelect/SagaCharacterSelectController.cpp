#include "SagaCharacterSelectController.h"
#include <Engine/EngineTypes.h>
#include <Engine/HitResult.h>
#include <UObject/Object.h>
#include <Containers/UnrealString.h>
#include <InputActionValue.h>
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>

#include "SagaGameInfo.h"
#include "SagaGame/Character/CharacterSelect/SagaSelectCharacter.h"
#include "SagaGame/Player/SagaPlayerWeaponTypes.h"
#include "SagaGame/Input/SagaInputSystem.h"

#include "Saga/Network/SagaRpcProtocol.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaCharacterSelectController::ASagaCharacterSelectController(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, OnClickedCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void
ASagaCharacterSelectController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI mode{};
	SetInputMode(mode);

	SetShowMouseCursor(true);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	const USagaCharacterSelectInputSystem* InputSystem = GetDefault<USagaCharacterSelectInputSystem>();
	Subsystem->AddMappingContext(InputSystem->DefaultContext, 0);
}

void
ASagaCharacterSelectController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);
	ensure(Input);

	const USagaCharacterSelectInputSystem* InputSystem = GetDefault<USagaCharacterSelectInputSystem>();

	Input->BindAction(InputSystem->Click, ETriggerEvent::Completed, this, &ASagaCharacterSelectController::OnClick);
}

void
ASagaCharacterSelectController::OnClick(const FInputActionValue& Value)
{
	FHitResult result;
	bool Hit = GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel5, false, result);

	if (Hit)
	{
		UE_LOG(LogSagaGame, Warning, TEXT("[ASagaCharacterSelectController][OnClick] Mouse on a character"));

		const auto character = Cast<ASagaSelectCharacter>(result.GetActor());

		if (nullptr != character)
		{
			const EPlayerWeapon weapon = character->GetWeapon();

#if WITH_EDITOR
			const FString name = UEnum::GetValueAsString(weapon);
			UE_LOG(LogSagaGame, Warning, TEXT("Selected weapon: %s"), *name);
#endif

			const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

			if (not system->IsOfflineMode())
			{
				if (system->IsConnected())
				{
					system->SendRpcPacket(ESagaRpcProtocol::RPC_MAIN_WEAPON, static_cast<int64>(weapon));
				}
			}
			else
			{
				system->SetLocalUserWeapon(weapon);
			}
		}

		if (OnClickedCharacter.IsBound())
		{
			OnClickedCharacter.Broadcast(character);
		}
	}
}
