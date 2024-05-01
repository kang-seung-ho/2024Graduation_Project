// Fill out your copyright notice in the Description page of Project Settings.


#include "SagaCharacterSelectController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "../../Input/SagaInputSystem.h"

ASagaCharacterSelectController::ASagaCharacterSelectController()
{
	PrimaryActorTick.bCanEverTick = true;

	bShowMouseCursor = true;
}

void ASagaCharacterSelectController::BeginPlay()
{
	Super::BeginPlay();

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
}
