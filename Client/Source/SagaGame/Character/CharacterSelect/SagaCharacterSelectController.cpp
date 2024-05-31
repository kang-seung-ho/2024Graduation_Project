#include "SagaCharacterSelectController.h"
#include <UObject/Object.h>
#include <InputActionValue.h>
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>

#include "Saga/Level/SagaCharacterChoiceLevel.h"

#include "SagaGameInfo.h"
#include "SagaGame/Character/CharacterSelect/SagaSelectCharacter.h"
#include "SagaGame/Player/SagaPlayerWeaponTypes.h"
#include "SagaGame/Input/SagaInputSystem.h"

#include "Saga/Network/SagaRpcProtocol.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaCharacterSelectController::ASagaCharacterSelectController(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, mSelectActor()
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
		UE_LOG(LogSagaGame, Warning, TEXT("[ASagaCharacterSelectController][OnClick] Mouse on character"));
		mSelectActor = result.GetActor();
	}
	else
	{
		mSelectActor = nullptr;
	}

	if (mSelectActor)
	{
		const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());
		ASagaSelectCharacter* SelectedCharacter = Cast<ASagaSelectCharacter>(mSelectActor);

		if (SelectedCharacter)
		{
			EPlayerWeapon WeaponType = SelectedCharacter->GetWeapon();

			// 무기 유형에 따라 서버전송 처리를 수행
			FString WeaponName = TEXT("Unknown");

			switch (WeaponType)
			{
			case EPlayerWeapon::LightSabor:
			{
				WeaponName = TEXT("Light Sabor");

				if (not system->IsOfflineMode())
				{
					if (system->IsConnected())
					{
						system->SendRpcPacket(ESagaRpcProtocol::RPC_MAIN_WEAPON, 0);
					}
					else
					{
						UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
					}
				}
				else
				{
					system->SetLocalUserWeapon(EPlayerWeapon::LightSabor);
				}
			}
			break;

			case EPlayerWeapon::WaterGun:
			{
				WeaponName = TEXT("Water Gun");

				if (not system->IsOfflineMode())
				{
					if (system->IsConnected())
					{
						system->SendRpcPacket(ESagaRpcProtocol::RPC_MAIN_WEAPON, 1);
					}
					else
					{
						UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
					}
				}
				else
				{
					system->SetLocalUserWeapon(EPlayerWeapon::WaterGun);
				}
			}
			break;

			case EPlayerWeapon::Hammer:
			{
				WeaponName = TEXT("Hammer");

				if (not system->IsOfflineMode())
				{
					if (system->IsConnected())
					{
						system->SendRpcPacket(ESagaRpcProtocol::RPC_MAIN_WEAPON, 2);
					}
					else
					{
						UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
					}
				}
				else
				{
					system->SetLocalUserWeapon(EPlayerWeapon::Hammer);
				}
			}
			break;
			}

			UE_LOG(LogSagaGame, Warning, TEXT("Selected weapon: %s"), *WeaponName);

			if (OnClickedCharacter.IsBound())
			{
				OnClickedCharacter.Broadcast(SelectedCharacter);
			}
		}
	}
}
