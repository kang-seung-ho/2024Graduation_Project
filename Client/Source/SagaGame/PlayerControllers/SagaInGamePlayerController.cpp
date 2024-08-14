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
	: APlayerController(initializer)
	, walkDirection()
	, InventoryWidgetClass(), InventoryWidget(nullptr)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> InventoryWidgetRef{ TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_Inventory.UI_Inventory_C'") };
	if (InventoryWidgetRef.Succeeded())
	{
		InventoryWidgetClass = InventoryWidgetRef.Class;
	}
}

void
ASagaInGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(InventoryWidgetClass))
	{
		UE_LOG(LogSagaGame, Log, TEXT("[ASagaInGamePlayerController][BeginPlay] Creating Inventory Widget..."));

		InventoryWidget = CreateWidget<USagaInventoryWidget>(this, InventoryWidgetClass);

		if (IsValid(InventoryWidget))
		{
			InventoryWidget->AddToViewport(5); //this means that zorder is 5
			InventoryWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		UE_LOG(LogSagaGame, Fatal, TEXT("[ASagaInGamePlayerController][BeginPlay] Could not create the UI of inventory."));
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
ASagaInGamePlayerController::UpdateInputMode()
{
	/*if (IsInventoryVisible())
	{
		UE_LOG(LogSagaGame, Warning, TEXT("Setting input mode to UI only"));

		SetInputMode(FInputModeUIOnly());

		bShowMouseCursor = true;
	}
	else
	{
		UE_LOG(LogSagaGame, Warning, TEXT("Setting input mode to Game only"));

		SetInputMode(FInputModeGameOnly());

		bShowMouseCursor = false;
	}

	UE_LOG(LogSagaGame, Warning, TEXT("Input mode and cursor visibility updated"));*/
}

//void
//ASagaInGamePlayerController::SetInventoryVisibility(bool flag)
//{
//	InventoryWidget->SetVisibility(flag ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
//
//	UE_LOG(LogSagaGame, Log, TEXT("Inventory visibility is set to: %s"), flag ? TEXT("Visible") : TEXT("Collapsed"));
//
//	if (flag)
//	{
//		UE_LOG(LogSagaGame, Log, TEXT("[ASagaInGamePlayerController] Setting input mode to Game on UI both"));
//
//		SetInputMode(FInputModeGameAndUI());
//
//		bShowMouseCursor = true;
//	}
//	else
//	{
//		UE_LOG(LogSagaGame, Log, TEXT("[ASagaInGamePlayerController] Setting input mode to Game only"));
//
//		SetInputMode(FInputModeGameOnly());
//
//		bShowMouseCursor = false;
//	}
//
//
//	UE_LOG(LogSagaGame, Log, TEXT("Input mode and cursor visibility updated"));
//}

//ESlateVisibility
//ASagaInGamePlayerController::GetInventoryVisibility()
//const
//{
//	return InventoryWidget->GetVisibility();
//}

//bool
//ASagaInGamePlayerController::IsInventoryVisible() const
//{
//	return InventoryWidget->GetVisibility() == ESlateVisibility::Visible;
//}

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

void
ASagaInGamePlayerController::Tick(float delta_time)
{
	Super::Tick(delta_time);

	const auto pawn = GetPawn();
	if (!IsValid(pawn))
	{
		return;
	}

	auto pos = pawn->GetActorLocation();

	bool out_of_boundary = false;
	/*if (pos.X < -4500)
	{
		pos.X = -4500;
		out_of_boundary = true;
	}
	else if (4500 < pos.X)
	{
		pos.X = 4500;
		out_of_boundary = true;
	}

	if (pos.Y < -4500)
	{
		pos.Y = -4500;
		out_of_boundary = true;
	}
	else if (4500 < pos.Y)
	{
		pos.Y = 4500;
		out_of_boundary = true;
	}*/

	if (out_of_boundary)
	{
		pawn->SetActorLocation(pos);
	}
}

void
ASagaInGamePlayerController::OnPossess(APawn* pawn)
{
#if WITH_EDITOR

	if (IsValid(pawn))
	{
		const auto name = pawn->GetName();
		UE_LOG(LogSagaFramework, Log, TEXT("[ASagaInGamePlayerController][OnPossess] Possessioning '%s'..."), *name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[ASagaInGamePlayerController][OnPossess] The pawn is null."));
	}
#endif

	Super::OnPossess(pawn);
}
