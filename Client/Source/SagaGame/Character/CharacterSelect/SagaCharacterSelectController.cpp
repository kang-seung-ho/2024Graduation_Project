#include "SagaCharacterSelectController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "../../Input/SagaInputSystem.h"
#include "../../UI/SagaCharacterSelectWidget.h"
#include "SagaSelectCharacter.h"

#include "Saga/Network/SagaNetworkSettings.h"
#include "Saga/Network/SagaNetworkSubSystem.h"
#include "Saga/Network/SagaRpcProtocol.h"

ASagaCharacterSelectController::ASagaCharacterSelectController()
{
	PrimaryActorTick.bCanEverTick = true;

	bShowMouseCursor = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_CharacterSelect.UI_CharacterSelect_C'"));

	if (WidgetClass.Succeeded())
	{
		mSelectWidgetClass = WidgetClass.Class;
	}

}

void
ASagaCharacterSelectController::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASagaCharacterSelectController::CountDown, 1.0f, true, 0.0);


	if (IsValid(mSelectWidgetClass))
	{
		mSelectWidget = CreateWidget<USagaCharacterSelectWidget>(GetWorld(), mSelectWidgetClass);

		if (IsValid(mSelectWidget))
		{
			mSelectWidget->AddToViewport();
		}
	}

	FInputModeGameAndUI InputMode;
	SetInputMode(InputMode);

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

void ASagaCharacterSelectController::OnClick(const FInputActionValue& Value)
{
	mSelectActor = mUnderCursorActor;

	if (mSelectActor)
	{
		ASagaSelectCharacter* SelectedCharacter = Cast<ASagaSelectCharacter>(mSelectActor);
		if (SelectedCharacter)
		{
			EPlayerWeapon WeaponType = SelectedCharacter->GetWeapon();

			mSelectWidget->StartButtonEnable(true);

			// 무기 유형에 따라 서버전송 처리를 수행
			FString WeaponName = TEXT("Unknown");
			auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());
			switch (WeaponType)
			{
			case EPlayerWeapon::LightSabor:
				WeaponName = TEXT("Light Sabor");				
				system->SetWeaponType(EPlayerWeapon::LightSabor);
				if constexpr (not saga::IsOfflineMode)
				{
					if (nullptr != system and system->GetLocalUserId() != -1)
					{
						system->SendRpcPacket(ESagaRpcProtocol::RPC_MAIN_WEAPON, 0);
					}
					else
					{
						UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
					}
				}
				break;
			case EPlayerWeapon::WaterGun:
				WeaponName = TEXT("Water Gun");
				system->SetWeaponType(EPlayerWeapon::WaterGun);
				if constexpr (not saga::IsOfflineMode)
				{
					if (nullptr != system and system->GetLocalUserId() != -1)
					{
						system->SendRpcPacket(ESagaRpcProtocol::RPC_MAIN_WEAPON, 1);
					}
					else
					{
						UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
					}
				}
				break;
			case EPlayerWeapon::Hammer:
				WeaponName = TEXT("Hammer");
				system->SetWeaponType(EPlayerWeapon::Hammer);
				if constexpr (not saga::IsOfflineMode)
				{
					if (nullptr != system and system->GetLocalUserId() != -1)
					{
						system->SendRpcPacket(ESagaRpcProtocol::RPC_MAIN_WEAPON, 2);
					}
					else
					{
						UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
					}
				}
				break;
			}
			UE_LOG(LogSagaGame, Warning, TEXT("Selected weapon: %s"), *WeaponName);
		}
		else
		{
			mSelectWidget->StartButtonEnable(false);
		}
	}
	else
	{
		mSelectWidget->StartButtonEnable(false);
	}
}
