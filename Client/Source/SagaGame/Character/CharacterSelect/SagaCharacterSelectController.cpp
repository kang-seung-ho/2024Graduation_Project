// Fill out your copyright notice in the Description page of Project Settings.


#include "SagaCharacterSelectController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "../../Input/SagaInputSystem.h"
#include "../../UI/SagaCharacterSelectWidget.h"

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

void ASagaCharacterSelectController::BeginPlay()
{
	Super::BeginPlay();

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

void ASagaCharacterSelectController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);
	ensure(Input);

	const USagaCharacterSelectInputSystem* InputSystem = GetDefault<USagaCharacterSelectInputSystem>();

	Input->BindAction(InputSystem->Click, ETriggerEvent::Completed, this, &ASagaCharacterSelectController::OnClick);
}

void ASagaCharacterSelectController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult result;
	bool Hit = GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel5, false, result);

	if (Hit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mouse On"));
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
	UE_LOG(LogTemp, Warning, TEXT("Clicked"));

	if (mSelectActor)
	{
		mSelectWidget->StartButtonEnable(true);
		UE_LOG(LogTemp, Warning, TEXT("Actor Selected"));
		UE_LOG(LogTemp, Warning,TEXT("%s"),  *mSelectActor->GetName());
	}
	else
	{
		mSelectWidget->StartButtonEnable(false);
	}
}
