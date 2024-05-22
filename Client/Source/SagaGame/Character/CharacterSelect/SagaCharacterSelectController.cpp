#include "SagaCharacterSelectController.h"
#include <InputActionValue.h>
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>

#include "SagaGameInfo.h"
#include "SagaGame/Character/CharacterSelect/SagaSelectCharacter.h"
#include "SagaGame/Player/SagaPlayerWeaponTypes.h"
#include "SagaGame/Input/SagaInputSystem.h"
#include "SagaGame/UI/SagaCharacterSelectWidget.h"

#include "Saga/Network/SagaRpcProtocol.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaCharacterSelectController::ASagaCharacterSelectController(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, isStartButtonClicked(), isGameStartable()
	, mSelectWidgetClass(), mSelectWidget()
{
	PrimaryActorTick.bCanEverTick = true;

	FInputModeGameAndUI mode{};
	SetInputMode(mode);

	SetShowMouseCursor(true);

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_CharacterSelect.UI_CharacterSelect_C'"));

	if (WidgetClass.Succeeded())
	{
		mSelectWidgetClass = WidgetClass.Class;
	}
	else
	{
		const auto my_name = GetName();

		UE_LOG(LogSagaGame, Fatal, TEXT("[ASagaCharacterSelectController] '%s' could not find the selectable character class."), *my_name);
	}
}

void
ASagaCharacterSelectController::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle TimerHandle{};
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASagaCharacterSelectController::CountDown, 1.0f, true, 0.0);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	const USagaCharacterSelectInputSystem* InputSystem = GetDefault<USagaCharacterSelectInputSystem>();
	Subsystem->AddMappingContext(InputSystem->DefaultContext, 0);
}

void
ASagaCharacterSelectController::CountDown()
{
	if (Seconds != 0)
	{
		Seconds = Seconds - 1;
	}
	else
	{
		if (Minutes == 0 && Seconds == 0)
		{
			// 게임 시작(레벨전환) + 서버에게 게임 시작을 알림
			UGameplayStatics::OpenLevel(GetWorld(), TEXT("SagaGameLevel"));
			UE_LOG(LogSagaGame, Warning, TEXT("Game Start"));
		}
		else
		{
			Minutes = Minutes - 1;
			Seconds = 59;
		}
	}
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
ASagaCharacterSelectController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult result;
	bool Hit = GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel5, false, result);

	if (Hit)
	{
		UE_LOG(LogSagaGame, Warning, TEXT("Mouse On"));
		mUnderCursorActor = result.GetActor();
	}
	else
	{
		mUnderCursorActor = nullptr;
	}
}

void
ASagaCharacterSelectController::OnClick(const FInputActionValue& Value)
{
	mSelectActor = mUnderCursorActor;

	if (mSelectActor)
	{
		const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());
		ASagaSelectCharacter* SelectedCharacter = Cast<ASagaSelectCharacter>(mSelectActor);

		if (SelectedCharacter)
		{
			EPlayerWeapon WeaponType = SelectedCharacter->GetWeapon();

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
						UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
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
						UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
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
						UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
					}
				}
				else
				{
					system->SetOfflineWeapon(EPlayerWeapon::Hammer);
				}
			}
			break;
			}

			UE_LOG(LogSagaGame, Warning, TEXT("Selected weapon: %s"), *WeaponName);
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
