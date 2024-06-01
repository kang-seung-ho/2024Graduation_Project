#include "SagaInGamePlayerController.h"
#include <UObject/Object.h>
#include <Kismet/GameplayStatics.h>
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include <Blueprint/UserWidget.h>

#include "SagaGameInfo.h"
#include "Input/SagaInputSystem.h"
#include "SagaCharacterPlayer.h"
#include "SagaGummyBearPlayer.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaInGamePlayerController::ASagaInGamePlayerController(const FObjectInitializer& ObjectInitializer)
	: APlayerController(ObjectInitializer)
	, isRiding(), OnRideNPC()
	, walkDirection()
	, isAttacking()
	, lastCharacterPosition(), lastCharacterRotation()
	, readyTimerHandle(), countdownTimerHandle()
	, transformUpdateTimer()
{}

void
ASagaInGamePlayerController::TriggerRideNPC(const FInputActionValue& Value)
{
	ASagaPlayableCharacter* ControlledCharacter = GetPawn<ASagaPlayableCharacter>();
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
ASagaInGamePlayerController::RideNPCCallFunction()
{
	ASagaPlayableCharacter* ControlledCharacter = GetPawn<ASagaPlayableCharacter>();

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

	FInputModeGameOnly mode{};
	SetInputMode(mode);

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

	const auto world = GetWorld();
	const auto system = USagaNetworkSubSystem::GetSubSystem(world);

	if (nullptr != system)
	{
		system->OnLeftRoom.AddDynamic(this, &ASagaInGamePlayerController::OnLeftRoom);

		system->OnStartGame.AddDynamic(this, &ASagaInGamePlayerController::OnGameStarted);

		system->OnRpc.AddDynamic(this, &ASagaInGamePlayerController::OnRpc);
	}
	else
	{
		UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
	}

	GetWorldTimerManager().SetTimer(readyTimerHandle, this, &ASagaInGamePlayerController::OnLevelReady, 1.0f, false);

	GetWorldTimerManager().SetTimer(countdownTimerHandle, this, &ASagaInGamePlayerController::CountDown, 1.0f, true);
}

void
ASagaInGamePlayerController::CountDown()
{
	if (Seconds != 0)
	{
		Seconds = Seconds - 1;
	}
	else
	{
		if (Minutes == 0 && Seconds == 0)
		{
			//UGameplayStatics::OpenLevel(GetWorld(), TEXT("SagaGameLevel"));
			UE_LOG(LogTemp, Warning, TEXT("Game End"));
		}
		else
		{
			Minutes = Minutes - 1;
			Seconds = 59;
		}
	}
}

void
ASagaInGamePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(transformUpdateTimer);
}

void
ASagaInGamePlayerController::Tick(float delta_time)
{
	Super::Tick(delta_time);

	const auto pawn = GetPawn<ASagaCharacterPlayer>();
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

	// �̺�Ʈ�� ��������Ʈ ���ε�
	OnRideNPC.AddDynamic(this, &ASagaInGamePlayerController::RideNPCCallFunction);
}
