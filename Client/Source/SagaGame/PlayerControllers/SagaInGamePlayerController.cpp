#include "PlayerControllers/SagaInGamePlayerController.h"
#include <Math/MathFwd.h>
#include <UObject/Object.h>
#include <UObject/UObjectGlobals.h>
#include <Kismet/GameplayStatics.h>
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>

#include "Input/SagaInputSystem.h"
#include "Character/SagaCharacterBase.h"

ASagaInGamePlayerController::ASagaInGamePlayerController(const FObjectInitializer& initializer)
noexcept
	: APlayerController(initializer)
	, OnRideNPC()
	, ownerId(-1)
	, walkDirection()
	, isAttacking()
	, mySpawner()
{}

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

	//if (GetPawn())
	//{
	//	mArm->SetupAttachment(GetPawn()->GetRootComponent());
	//	mCamera->SetupAttachment(mArm);

	//	mArm->SetRelativeLocation(FVector(0.0, 0.0, 150.0));
	//	mArm->SetRelativeRotation(FRotator(-15.0, 90.0, 0.0));
	//	mArm->TargetArmLength = 150.f;
	//}

	//// Make sure the camera components are correctly initialized
	//if (GetPawn())
	//{
	//	mArm->AttachToComponent(GetPawn()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	//	mCamera->AttachToComponent(mArm, FAttachmentTransformRules::SnapToTargetIncludingScale);
	//}
	//
}

void
ASagaInGamePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void
ASagaInGamePlayerController::Tick(float delta_time)
{
	Super::Tick(delta_time);

	const auto pawn = GetPawn<ASagaCharacterBase>();
	if (not IsValid(pawn))
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

	//Input->BindAction(InputSystem->Interact, ETriggerEvent::Started, this, &ASagaInGamePlayerController::TriggerRideNPC);

	OnRideNPC.AddDynamic(this, &ASagaInGamePlayerController::RideNPCCallFunction);
}

void
ASagaInGamePlayerController::AssignPlayerSpawner(AActor* spawner)
noexcept
{
	mySpawner = spawner;
}

const AActor*
ASagaInGamePlayerController::GetLocalPlayerSpawner()
const noexcept
{
	return mySpawner;
}

int32
ASagaInGamePlayerController::GetOwnerId()
const noexcept
{
	return ownerId;
}
