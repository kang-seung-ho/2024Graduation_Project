// Fill out your copyright notice in the Description page of Project Settings.


#include "SagaInGamePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "../Input/SagaInputSystem.h"
#include "SagaCharacterPlayer.h"

ASagaInGamePlayerController::ASagaInGamePlayerController(const FObjectInitializer& ObjectInitializer) : APlayerController(ObjectInitializer)
{
	mMoveDir = 0.f;
}

void ASagaInGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	//매핑컨텍스트를 입력시스템에 지정
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	const USagaInputSystem* InputSystem = GetDefault<USagaInputSystem>();

	Subsystem->AddMappingContext(InputSystem->DefaultContext, 0);
}

void ASagaInGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);
	ensure(Input);

	const USagaInputSystem* InputSystem = GetDefault<USagaInputSystem>();

	Input->BindAction(InputSystem->Move, ETriggerEvent::Triggered, this, &ASagaInGamePlayerController::OnMove);
	Input->BindAction(InputSystem->Attack, ETriggerEvent::Started, this, &ASagaInGamePlayerController::OnAttack);
}


void ASagaInGamePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


void ASagaInGamePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASagaInGamePlayerController::OnMove(const FInputActionValue& Value)
{
	const FVector InputValue = Value.Get<FVector>();
	APawn* ControlledPawn = GetPawn();

	const FRotator Rotation = K2_GetActorRotation();
	const FRotator YawRotation = FRotator(0.0, Rotation.Yaw, 0.0);
	const FVector ForwardVector = YawRotation.Vector();
	const FVector RightVector = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);

	ControlledPawn->AddMovementInput(ForwardVector, InputValue.Y);
	ControlledPawn->AddMovementInput(RightVector, InputValue.X);

	//우: InputValue.X가 1
	//왼: InputValue.Y가 -1
	//둘다 X : 0
	mMoveDir = InputValue.X * 90.f;

	//앞으로 이동
	if (InputValue.Y > 0.f)
	{
		//앞으로, + 왼쪽
		if (InputValue.X < 0.f)
		{
			mMoveDir = -45.f;
		}
		//앞으로, + 오른쪽
		else if (InputValue.X > 0.f)
		{
			mMoveDir = 45.f;
		}
	}
	else if (InputValue.Y < 0.f)
	{
		//뒤로, + 왼쪽
		if (InputValue.X < 0.f)
		{
			mMoveDir = -135.f;
		}
		//뒤로, + 오른쪽
		else if (InputValue.X > 0.f)
		{
			mMoveDir = 135.f;
		}
		else
		{
			mMoveDir = 180.f;
		}
	}
}

void ASagaInGamePlayerController::OnAttack(const FInputActionValue& Value)
{
	ASagaCharacterPlayer* ControlledPawn = GetPawn<ASagaCharacterPlayer>();

	ControlledPawn->PlayAttackAnimation();
}
