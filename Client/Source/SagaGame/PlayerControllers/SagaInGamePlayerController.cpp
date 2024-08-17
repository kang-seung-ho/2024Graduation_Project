#include "PlayerControllers/SagaInGamePlayerController.h"
#include <Math/MathFwd.h>
#include <UObject/Object.h>
#include <UObject/ObjectPtr.h>
#include <UObject/UObjectGlobals.h>
#include <Engine/EngineBaseTypes.h>
#include <Engine/Texture2D.h>
#include <Kismet/GameplayStatics.h>
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include <Blueprint/UserWidget.h>

#include "Input/SagaInputSystem.h"
#include "Item/SagaItemTypes.h"
#include "UI/InventoryItemData.h"
#include "UI/SagaInventoryWidget.h"

ASagaInGamePlayerController::ASagaInGamePlayerController(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, InventoryWidgetClass(), InventoryWidget(nullptr)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> InventoryWidgetRef{ TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_Inventory.UI_Inventory_C'") };
	if (InventoryWidgetRef.Succeeded())
	{
		InventoryWidgetClass = InventoryWidgetRef.Class;
	}
	else
	{
#if WITH_EDITOR

		UE_LOG(LogSagaGame, Error, TEXT("[ASagaInGamePlayerController][Ctor] Could not find a class of the inventory ui."));
#endif
	}
}

void
ASagaInGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(InventoryWidgetClass))
	{
#if WITH_EDITOR

		UE_LOG(LogSagaGame, Log, TEXT("[ASagaInGamePlayerController][BeginPlay] Creating Inventory Widget..."));
#endif

		InventoryWidget = CreateWidget<USagaInventoryWidget>(this, InventoryWidgetClass);

		if (IsValid(InventoryWidget))
		{
			InventoryWidget->AddToViewport(5); //this means that zorder is 5
			InventoryWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
#if WITH_EDITOR

		UE_LOG(LogSagaGame, Fatal, TEXT("[ASagaInGamePlayerController][BeginPlay] Could not create the UI of inventory."));
#endif
	}

	FInputModeGameOnly mode{};
	SetInputMode(mode);

	UGameplayStatics::SetViewportMouseCaptureMode(this, EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown);
	SetShowMouseCursor(false);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	const USagaInputSystem* InputSystem = GetDefault<USagaInputSystem>();
	Subsystem->AddMappingContext(InputSystem->DefaultContext, 0);
}

void
ASagaInGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);
	ensure(Input);

	const USagaInputSystem* InputSystem = GetDefault<USagaInputSystem>();

	Input->BindAction(InputSystem->ForwardBackMove, ETriggerEvent::Started, this, &ASagaInGamePlayerController::BeginForwardWalk);
	Input->BindAction(InputSystem->ForwardBackMove, ETriggerEvent::Triggered, this, &ASagaInGamePlayerController::MidForwardWalk);
	Input->BindAction(InputSystem->ForwardBackMove, ETriggerEvent::Completed, this, &ASagaInGamePlayerController::EndForwardWalk);

	Input->BindAction(InputSystem->StrafeMove, ETriggerEvent::Started, this, &ASagaInGamePlayerController::BeginStrafeWalk);
	Input->BindAction(InputSystem->StrafeMove, ETriggerEvent::Triggered, this, &ASagaInGamePlayerController::MidStrafeWalk);
	Input->BindAction(InputSystem->StrafeMove, ETriggerEvent::Completed, this, &ASagaInGamePlayerController::EndStrafeWalk);

	Input->BindAction(InputSystem->Sprint, ETriggerEvent::Started, this, &ASagaInGamePlayerController::BeginRun);
	Input->BindAction(InputSystem->Sprint, ETriggerEvent::Completed, this, &ASagaInGamePlayerController::EndRun);

	Input->BindAction(InputSystem->Jump, ETriggerEvent::Started, this, &ASagaInGamePlayerController::BeginJump);

	Input->BindAction(InputSystem->Rotate, ETriggerEvent::Triggered, this, &ASagaInGamePlayerController::BeginRotate);

	Input->BindAction(InputSystem->Attack, ETriggerEvent::Started, this, &ASagaInGamePlayerController::BeginAttack);
	Input->BindAction(InputSystem->Attack, ETriggerEvent::Completed, this, &ASagaInGamePlayerController::EndAttack);

	Input->BindAction(InputSystem->Interact, ETriggerEvent::Started, this, &ASagaInGamePlayerController::BeginGuardianAction);

	Input->BindAction(InputSystem->Skill1, ETriggerEvent::Started, this, &ASagaInGamePlayerController::OnSkill1);
	Input->BindAction(InputSystem->Skill2, ETriggerEvent::Started, this, &ASagaInGamePlayerController::OnSkill2);
	Input->BindAction(InputSystem->Skill3, ETriggerEvent::Started, this, &ASagaInGamePlayerController::OnSkill3);
	Input->BindAction(InputSystem->Skill4, ETriggerEvent::Started, this, &ASagaInGamePlayerController::OnSkill4);

	Input->BindAction(InputSystem->Item1, ETriggerEvent::Started, this, &ASagaInGamePlayerController::OnItem1);
	Input->BindAction(InputSystem->Item2, ETriggerEvent::Started, this, &ASagaInGamePlayerController::OnItem2);
	Input->BindAction(InputSystem->Item3, ETriggerEvent::Started, this, &ASagaInGamePlayerController::OnItem3);


}

void
ASagaInGamePlayerController::AddItemToInventory(ESagaItemTypes ItemType)
{
	if (InventoryWidget)
	{
		FString IconPath;
		FString ItemName;

		switch (ItemType)
		{
		case ESagaItemTypes::Drink:
		{
			IconPath = TEXT("/Script/Engine.Texture2D'/Game/UI/Images/Tex_heart.Tex_heart'");
			ItemName = TEXT("EnergyDrink");
		}
		break;

		case ESagaItemTypes::Gum:
		{
			IconPath = TEXT("/Script/Engine.Texture2D'/Game/UI/Images/gumball_machine.gumball_machine'");
			ItemName = TEXT("Gumball");
		}
		break;

		case ESagaItemTypes::SmokeBomb:
		{
			IconPath = TEXT("/Script/Engine.Texture2D'/Game/UI/Images/smoke_bomb.smoke_bomb'");
			ItemName = TEXT("SmokeBomb");
		}
		break;

		default:
			break;
		}

		UInventoryItemData* ItemData = NewObject<UInventoryItemData>();
		UTexture2D* IconTexture = LoadObject<UTexture2D>(nullptr, *IconPath);
		ItemData->SetInfo(IconTexture, ItemName, 1);

		InventoryWidget->AddItemToInventory(ItemType);
	}
}

#if WITH_EDITOR

void
ASagaInGamePlayerController::OnPossess(APawn* pawn)
{
	if (IsValid(pawn))
	{
		const auto name = pawn->GetName();
		UE_LOG(LogSagaGame, Log, TEXT("[ASagaInGamePlayerController][OnPossess] Possessioning '%s'..."), *name);
	}
	else
	{
		UE_LOG(LogSagaGame, Error, TEXT("[ASagaInGamePlayerController][OnPossess] The pawn is null."));
	}

	Super::OnPossess(pawn);
}
#endif
