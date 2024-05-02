#include "SagaInGamePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "Input/SagaInputSystem.h"
#include "SagaCharacterPlayer.h"
#include "SagaGummyBearPlayer.h"
#include "Saga/Network/SagaNetworkSubSystem.h"
#include "Saga/Network/SagaRpcProtocol.h"
#include "Saga/Network/SagaNetworkSettings.h"

ASagaInGamePlayerController::ASagaInGamePlayerController(const FObjectInitializer& ObjectInitializer)
	: APlayerController(ObjectInitializer)
	, isForwardWalking(), isStrafeWalking(), isRunning()
	, walkDirection(), preferedDirection()
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
	//Input->BindAction(InputSystem->Interact, ETriggerEvent::Started, this, &ASagaInGamePlayerController::ExecuteRide);
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

void
ASagaInGamePlayerController::ExecuteRide()
{
	//if (isRiding) // 탑승 중일 경우 내리는 로직
	//{
	//	isRiding = false;
	//	GetOwner()->SetActorHiddenInGame(false);  // 플레이어를 다시 보이게 함
	//	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	//	if (PlayerController)
	//	{
	//		PlayerController->UnPossess();  // 이전에 빙의했던 NPC에서 벗어남
	//		PlayerController->Possess(GetPawn());  // 원래 플레이어 캐릭터로 다시 빙의
	//	}
	//}
	//else // 탑승하는 로직
	//{
	//	TArray<AActor*> OverlappingActors;
	//	GetOverlappingActors(OverlappingActors, ASagaGummyBearPlayer::StaticClass());  // GummyBear NPC 클래스로 필터링

	//	for (AActor* Actor : OverlappingActors)
	//	{
	//		ASagaGummyBearPlayer* NPCCharacter = Cast<ASagaGummyBearPlayer>(Actor);
	//		if (NPCCharacter)
	//		{
	//			UE_LOG(LogSagaGame, Log, TEXT("Interacted With NPC"));
	//			// 탑승한 경우 플레이어를 숨김
	//			GetPawn()->SetActorHiddenInGame(true);
	//			isRiding = true;

	//			// 현재 컨트롤러로 NPC 빙의
	//			Possess(NPCCharacter);
	//			break;  // 빙의에 성공하면 루프 종료
	//		}
	//		else {
	//			UE_LOG(LogSagaGame, Log, TEXT("No NPC to ride"));
	//		}
	//	}
	//}
}


double
ASagaInGamePlayerController::GetNormalizedMoveDir()
const noexcept
{
	const auto angle = FMath::RadiansToDegrees(FMath::Atan2(walkDirection.Y, walkDirection.X));

	return floor(angle / 45);
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
}

void
ASagaInGamePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
