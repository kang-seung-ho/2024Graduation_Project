#include "SagaInGamePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "Input/SagaInputSystem.h"
#include "SagaCharacterPlayer.h"
#include "SagaGummyBearPlayer.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaInGamePlayerController::ASagaInGamePlayerController(const FObjectInitializer& ObjectInitializer)
	: APlayerController(ObjectInitializer)
	, wasMoved(), wasTilted()
	, isRiding()
	, walkDirection()
	, tranformUpdateTimer()
	, mMoveDir()
	, OnRideNPC()
{}

void ASagaInGamePlayerController::TriggerRideNPC(const FInputActionValue& Value)
{
	ASagaPlayableCharacter* ControlledCharacter = Cast<ASagaPlayableCharacter>(GetPawn());
	UE_LOG(LogSagaGame, Warning, TEXT("TriggerRideNPC"));

	if (ControlledCharacter)
	{
		ControlledCharacter->RideNPC();
	}
	else
	{
		UE_LOG(LogSagaGame, Warning, TEXT("This Character is not ASagaPlayableCharacter Type."));
	}
}

void ASagaInGamePlayerController::RideNPCCallFunction()
{
	ASagaPlayableCharacter* ControlledCharacter = Cast<ASagaPlayableCharacter>(GetPawn());

	UE_LOG(LogSagaGame, Warning, TEXT("TriggerRideNPC"));
	if (ControlledCharacter)
	{
		ControlledCharacter->RideNPC();
	}
	else
	{
		UE_LOG(LogSagaGame, Warning, TEXT("This Character is not ASagaPlayableCharacter Type."));
	}
}

void
ASagaInGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

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

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (nullptr != system and system->GetLocalUserId() != -1)
	{
		system->OnStartGame.AddDynamic(this, &ASagaInGamePlayerController::OnGameStarted);
		system->OnRpc.AddDynamic(this, &ASagaInGamePlayerController::OnRpc);

		system->SendGameIsLoadedPacket();
	}
	else
	{
		UE_LOG(LogSagaGame, Error, TEXT("Network subsystem is not ready."));
	}
}

void
ASagaInGamePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(tranformUpdateTimer);
}

void
ASagaInGamePlayerController::Tick(float delta_time)
{
	Super::Tick(delta_time);

	// Update animation's factor value
	auto pawn = GetPawn();
	if (nullptr == pawn)
	{
		return;
	}

	auto character = Cast<ASagaCharacterPlayer>(pawn);
	if (not IsValid(character))
	{
		return;
	}

	const auto& walk_dir = character->walkDirection;

	mMoveDir = walk_dir.X * 90.f;
	if (walk_dir.Y > 0.f)
	{
		if (walk_dir.X < 0.f)
		{
			mMoveDir = -45.f;
		}
		else if (walk_dir.X > 0.f)
		{
			mMoveDir = 45.f;
		}
	}
	else if (walk_dir.Y < 0.f)
	{
		if (walk_dir.X < 0.f)
		{
			mMoveDir = -135.f;
		}
		else if (walk_dir.X > 0.f)
		{
			mMoveDir = 135.f;
		}
		else
		{
			mMoveDir = 180.f;
		}
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
	Input->BindAction(InputSystem->ForwardBackMove, ETriggerEvent::Completed, this, &ASagaInGamePlayerController::EndForwardWalk);
	Input->BindAction(InputSystem->StrafeMove, ETriggerEvent::Started, this, &ASagaInGamePlayerController::BeginStrafeWalk);
	Input->BindAction(InputSystem->StrafeMove, ETriggerEvent::Completed, this, &ASagaInGamePlayerController::EndStrafeWalk);

	Input->BindAction(InputSystem->Sprint, ETriggerEvent::Started, this, &ASagaInGamePlayerController::BeginRun);
	Input->BindAction(InputSystem->Sprint, ETriggerEvent::Completed, this, &ASagaInGamePlayerController::EndRun);

	Input->BindAction(InputSystem->Jump, ETriggerEvent::Started, this, &ASagaInGamePlayerController::BeginJump);

	Input->BindAction(InputSystem->Rotate, ETriggerEvent::Triggered, this, &ASagaInGamePlayerController::BeginRotate);
	Input->BindAction(InputSystem->Rotate, ETriggerEvent::Completed, this, &ASagaInGamePlayerController::EndRotate);

	Input->BindAction(InputSystem->Attack, ETriggerEvent::Started, this, &ASagaInGamePlayerController::OnAttack);

	Input->BindAction(InputSystem->Interact, ETriggerEvent::Started, this, &ASagaInGamePlayerController::TriggerRideNPC);

	// �̺�Ʈ�� ��������Ʈ ���ε�
	OnRideNPC.AddDynamic(this, &ASagaInGamePlayerController::RideNPCCallFunction);
}
