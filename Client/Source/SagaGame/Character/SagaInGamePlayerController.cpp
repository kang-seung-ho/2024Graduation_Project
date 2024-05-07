#include "SagaInGamePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "Input/SagaInputSystem.h"
#include "SagaCharacterPlayer.h"
#include "SagaGummyBearPlayer.h"
#include "Saga/Network/SagaNetworkSubSystem.h"
#include "Saga/Network/SagaRpcProtocol.h"
#include "Saga/Network/SagaNetworkSettings.h"

#include "SagaPlayableCharacter.h"

ASagaInGamePlayerController::ASagaInGamePlayerController(const FObjectInitializer& ObjectInitializer)
	: APlayerController(ObjectInitializer)
	, isForwardWalking(), isStrafeWalking(), isRunning(), isRiding()
	, wasMoved(), wasTilted()
	, walkDirection(), preferedDirection()
	, tranformUpdateTimer()
{}



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

	Input->BindAction(InputSystem->Attack, ETriggerEvent::Started, this, &ASagaInGamePlayerController::OnAttack);
	Input->BindAction(InputSystem->Jump, ETriggerEvent::Started, this, &ASagaInGamePlayerController::ExecuteJump);
	Input->BindAction(InputSystem->Rotate, ETriggerEvent::Triggered, this, &ASagaInGamePlayerController::ExecuteRotation);
	Input->BindAction(InputSystem->Sprint, ETriggerEvent::Started, this, &ASagaInGamePlayerController::ExecuteRun);
	Input->BindAction(InputSystem->Sprint, ETriggerEvent::Completed, this, &ASagaInGamePlayerController::TerminateRun);
	Input->BindAction(InputSystem->Interact, ETriggerEvent::Started, this, &ASagaInGamePlayerController::TriggerRideNPC);


	// 이벤트에 델리게이트 바인딩
	OnRideNPC.AddDynamic(this, &ASagaInGamePlayerController::RideNPCCallFunction);
}

void ASagaInGamePlayerController::TriggerRideNPC(const FInputActionValue& Value)
{
	ASagaPlayableCharacter* ControlledCharacter = Cast<ASagaPlayableCharacter>(GetPawn());
	UE_LOG(LogTemp, Warning, TEXT("TriggerRideNPC"));
	if (ControlledCharacter)
	{
		ControlledCharacter->RideNPC();  // 실제 캐릭터의 함수 호출
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("This Character is not ASagaPlayableCharacter Type."));
	}
}

void ASagaInGamePlayerController::RideNPCCallFunction()
{
	ASagaPlayableCharacter* ControlledCharacter = Cast<ASagaPlayableCharacter>(GetPawn());
	UE_LOG(LogTemp, Warning, TEXT("TriggerRideNPC"));
	if (ControlledCharacter)
	{
		ControlledCharacter->RideNPC();  // 실제 캐릭터의 함수 호출
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("This Character is not ASagaPlayableCharacter Type."));
	}
}

void
ASagaInGamePlayerController::BeginRotation()
{}

void
ASagaInGamePlayerController::EndRotation()
{}

void
ASagaInGamePlayerController::ExecuteRotation(const FInputActionValue& Value)
{
	wasTilted = true;

	const FVector InputValue = Value.Get<FVector>();

	AddYawInput(InputValue.X);

	ASagaCharacterPlayer* ControlledPawn = GetPawn<ASagaCharacterPlayer>();
	if (ControlledPawn != nullptr)
	{
		ControlledPawn->RotationCameraArm(InputValue.Y);
	}
}

void
ASagaInGamePlayerController::TerminateRotation()
{

}

void
ASagaInGamePlayerController::BeginRide()
{
	if constexpr (not saga::IsOfflineMode)
	{
		auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		if (nullptr != system and system->GetLocalUserId() != -1)
		{
			system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_RIDE);
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
		}
	}
}

void
ASagaInGamePlayerController::EndRide()
{
	if constexpr (not saga::IsOfflineMode)
	{
		auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		if (nullptr != system and system->GetLocalUserId() != -1)
		{
			system->SendRpcPacket(ESagaRpcProtocol::RPC_END_RIDE);
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
		}
	}
}

void ASagaInGamePlayerController::TerminateRide_Implementation()
{

}

void
ASagaInGamePlayerController::ExecuteRide_Implementation()
{
	
}

void
ASagaInGamePlayerController::Tick(float delta_time)
{
	Super::Tick(delta_time);

	UpdateMovement(delta_time);

	if (isRunning)
	{
		ExecuteRun();
	}

	if (isForwardWalking || isStrafeWalking)
	{
		ExecuteWalk(delta_time);
	}
	else
	{
		TerminateWalk(delta_time);
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
	}
	else
	{
		UE_LOG(LogSagaGame, Warning, TEXT("Network subsystem is not ready."));
	}
}

void
ASagaInGamePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(tranformUpdateTimer);
}

void
ASagaInGamePlayerController::OnGameStarted()
{
	GetWorldTimerManager().SetTimer(tranformUpdateTimer, this, &ASagaInGamePlayerController::OnUpdateTransform, 1.00f, true, 5.0f);
}
