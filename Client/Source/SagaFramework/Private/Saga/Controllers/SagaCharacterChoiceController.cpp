#include "Saga/Controllers/SagaCharacterChoiceController.h"
#include <Engine/EngineTypes.h>
#include <Engine/HitResult.h>
#include <GameFramework/PlayerController.h>
#include <Components/Widget.h>
#include <Containers/UnrealString.h>
#include <Templates/Casts.h>
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include <InputActionValue.h>
#include <InputTriggers.h>
#include <Blueprint/UserWidget.h>
#include <Misc/AssertionMacros.h>

#include "SagaGameInfo.h"
#include "SagaGame/Character/CharacterSelect/SagaSelectCharacter.h"
#include "SagaGame/Player/SagaPlayerWeaponTypes.h"
#include "SagaGame/Input/SagaInputSystem.h"
#include "SagaGame/UI/SagaCharacterSelectWidget.h"
#include "SagaGame/Player/SagaPlayerWeaponTypes.h"

#include "Saga/Network/SagaRpcProtocol.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaCharacterChoiceController::ASagaCharacterChoiceController(const FObjectInitializer& initializer)
	: Super(initializer)
	, mSelectWidgetClass(), mSelectWidget()
	, myHoveredCharacter(), mySelectedCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bShowMouseCursor = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_CharacterSelect.UI_CharacterSelect_C'"));

	if (WidgetClass.Succeeded())
	{
		mSelectWidgetClass = WidgetClass.Class;
	}

	isGameStartable = false;
}

void
ASagaCharacterChoiceController::Tick(float delta_time)
{
	Super::Tick(delta_time);

	FHitResult result{};

	if (GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel5, false, result))
	{
		const auto target = result.GetActor();

		if (myHoveredCharacter != target)
		{
			UE_LOG(LogSagaFramework, Warning, TEXT("[ASagaCharacterChoiceController] Mouse On"));
			myHoveredCharacter = target;
		}
	}
	else
	{
		myHoveredCharacter = nullptr;
	}
}

void ASagaCharacterChoiceController::CountDown()
{}

void
ASagaCharacterChoiceController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI mode{};
	SetInputMode(mode);
}

void
ASagaCharacterChoiceController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);
	//ensure(Input);

	//const auto InputSystem = GetDefault<USagaCharacterSelectInputSystem>();

	//Input->BindAction(InputSystem->Click, ETriggerEvent::Completed, this, &ASagaCharacterChoiceController::OnClick);
}

void
ASagaCharacterChoiceController::OnClick(const FInputActionValue & input)
{
	if (myHoveredCharacter)
	{
		const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		ASagaSelectCharacter* selection{};
		//ASagaSelectCharacter* selection = Cast<ASagaSelectCharacter>(myHoveredCharacter);
		if (selection)
		{
			//EPlayerWeapon WeaponType = selection->GetWeapon();
			EPlayerWeapon WeaponType{};

			isGameStartable = true;

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
						UE_LOG(LogSagaFramework, Warning, TEXT("Network subsystem is not ready."));
					}
				}
				else
				{
					system->SetOfflineWeapon(EPlayerWeapon::LightSabor);
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
						UE_LOG(LogSagaFramework, Warning, TEXT("Network subsystem is not ready."));
					}
				}
				else
				{
					system->SetOfflineWeapon(EPlayerWeapon::WaterGun);
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
						UE_LOG(LogSagaFramework, Warning, TEXT("Network subsystem is not ready."));
					}
				}
				else
				{
					system->SetOfflineWeapon(EPlayerWeapon::Hammer);
				}
			}
			break;
			}

			UE_LOG(LogSagaFramework, Warning, TEXT("Selected weapon: %s"), *WeaponName);
		}
		else
		{
			isGameStartable = false;
		}
	}
	else
	{
		isGameStartable = false;
	}
}
