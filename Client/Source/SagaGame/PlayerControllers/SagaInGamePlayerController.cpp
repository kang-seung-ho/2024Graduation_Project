#include "PlayerControllers/SagaInGamePlayerController.h"
#include <Math/MathFwd.h>
#include <UObject/Object.h>
#include <UObject/ObjectPtr.h>
#include <UObject/UObjectGlobals.h>
#include <Kismet/GameplayStatics.h>
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>

#include "Input/SagaInputSystem.h"
#include "Blueprint/UserWidget.h"

ASagaInGamePlayerController::ASagaInGamePlayerController(const FObjectInitializer& initializer)
noexcept
	: APlayerController(initializer)
	, InventoryWidgetClass(), InventoryWidget(nullptr)
	, bIsInventoryVisible(false)
	, walkDirection(), isAttacking()
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

	Input->BindAction(InputSystem->Inventory, ETriggerEvent::Started, this, &ASagaInGamePlayerController::ToggleInventory);
	Input->BindAction(InputSystem->Interact, ETriggerEvent::Started, this, &ASagaInGamePlayerController::BeginGuardianAction);
}

void ASagaInGamePlayerController::ToggleInventory()
{
	UE_LOG(LogTemp, Warning, TEXT("Toggle Inventory called"));

	if (!InventoryWidget && InventoryWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Creating Inventory Widget"));
		InventoryWidget = CreateWidget<UUserWidget>(this, InventoryWidgetClass);
		if (InventoryWidget)
		{
			InventoryWidget->AddToViewport();
			InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (InventoryWidget)
	{
		bIsInventoryVisible = InventoryWidget->GetVisibility() == ESlateVisibility::Visible;

		bIsInventoryVisible = !bIsInventoryVisible;
		InventoryWidget->SetVisibility(bIsInventoryVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

		UE_LOG(LogTemp, Warning, TEXT("Inventory visibility set to: %s"), bIsInventoryVisible ? TEXT("Visible") : TEXT("Collapsed"));

		UpdateInputMode();
	}
}

void ASagaInGamePlayerController::UpdateInputMode()
{
	if (bIsInventoryVisible)
	{
		UE_LOG(LogTemp, Warning, TEXT("Setting input mode to UI only"));
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Setting input mode to Game only"));
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}

	UE_LOG(LogTemp, Warning, TEXT("Input mode and cursor visibility updated"));
}

void ASagaInGamePlayerController::SetInventoryVisibility(bool bVisible)
{
	if (InventoryWidget)
	{
		bIsInventoryVisible = bVisible;
		InventoryWidget->SetVisibility(bIsInventoryVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

		UE_LOG(LogTemp, Warning, TEXT("Inventory visibility set to: %s"), bIsInventoryVisible ? TEXT("Visible") : TEXT("Collapsed"));

		UpdateInputMode();
	}
}

void ASagaInGamePlayerController::Tick(float delta_time)
{
	Super::Tick(delta_time);

	const auto pawn = GetPawn();
	if (!IsValid(pawn))
	{
		return;
	}

	auto pos = pawn->GetActorLocation();

	bool out_of_boundary = false;
	if (pos.X < -4500)
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
	}

	if (out_of_boundary)
	{
		pawn->SetActorLocation(pos);
	}
}
