#include "SagaInGamePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "Input/SagaInputSystem.h"
#include "SagaCharacterPlayer.h"
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
}

void
ASagaInGamePlayerController::ExecuteWalk(const float& delta_time)
{
	APawn* ControlledPawn = GetPawn();

	const FRotator Rotation = K2_GetActorRotation();
	const FRotator YawRotation = FRotator(0.0, Rotation.Yaw, 0.0);
	const FVector ForwardVector = YawRotation.Vector();
	const FVector RightVector = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);

	walkDirection = (preferedDirection + walkDirection) / 2;

	ControlledPawn->AddMovementInput(ForwardVector, walkDirection.Y);
	ControlledPawn->AddMovementInput(RightVector, walkDirection.X);

	auto move_dir = walkDirection.X * 90.f;
	if (walkDirection.Y > 0.f)
	{
		if (walkDirection.X < 0.f)
		{
			move_dir = -45.f;
		}
		else if (walkDirection.X > 0.f)
		{
			move_dir = 45.f;
		}
	}
	else if (walkDirection.Y < 0.f)
	{
		if (walkDirection.X < 0.f)
		{
			move_dir = -135.f;
		}
		else if (walkDirection.X > 0.f)
		{
			move_dir = 135.f;
		}
		else
		{
			move_dir = 180.f;
		}
	}
}

void
ASagaInGamePlayerController::OnAttack(const FInputActionValue& Value)
{
	ASagaCharacterPlayer* ControlledPawn = GetPawn<ASagaCharacterPlayer>();

	ControlledPawn->PlayAttackAnimation();
}

void
PrintVector(const FVector& vector)
{
	const FString str = vector.ToString();
	UE_LOG(LogTemp, Warning, TEXT("[SagaGame][Character] Movement Vector: (%s)"), *str);
}

void
ASagaInGamePlayerController::BeginForwardWalk(const FInputActionValue& Value)
{
	isForwardWalking = true;

	const auto TempDirection = Value.Get<FVector>();
	preferedDirection.Y = preferedDirection.Y;

	PrintVector(preferedDirection);

	if constexpr (not saga::IsOfflineMode)
	{
		auto singleton = GetGameInstance();
		auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

		if (system->GetLocalUserId() != -1)
		{
			system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_WALK, GetNormalizedMoveDir());
		}
	}
}

void
ASagaInGamePlayerController::EndForwardWalk(const FInputActionValue& Value)
{
	isForwardWalking = false;

	FVector TempDirection = Value.Get<FVector>();
	preferedDirection.Y = preferedDirection.Y;

	PrintVector(preferedDirection);

	if constexpr (not saga::IsOfflineMode)
	{
		auto singleton = GetGameInstance();
		auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

		if (system->GetLocalUserId() != -1)
		{
			system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_WALK, GetNormalizedMoveDir());
		}
	}
}

void
ASagaInGamePlayerController::BeginStrafeWalk(const FInputActionValue& Value)
{
	isStrafeWalking = true;

	FVector TempDirection = Value.Get<FVector>();
	preferedDirection.X = preferedDirection.X;

	PrintVector(preferedDirection);

	FString KeyAsString = Value.ToString();

	if constexpr (not saga::IsOfflineMode)
	{
		auto singleton = GetGameInstance();
		auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

		if (system->GetLocalUserId() != -1)
		{
			system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_WALK, GetNormalizedMoveDir());
		}
	}
}

void
ASagaInGamePlayerController::EndStrafeWalk(const FInputActionValue& Value)
{
	isStrafeWalking = false;

	FVector TempDirection = Value.Get<FVector>();
	preferedDirection.X = preferedDirection.X;

	PrintVector(preferedDirection);

	if constexpr (not saga::IsOfflineMode)
	{
		auto singleton = GetGameInstance();
		auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

		if (system->GetLocalUserId() != -1)
		{
			system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_WALK, GetNormalizedMoveDir());
		}
	}
}

void
ASagaInGamePlayerController::BeginRun()
{
	isRunning = true;

	if constexpr (not saga::IsOfflineMode)
	{
		auto singleton = GetGameInstance();
		auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

		if (system->GetLocalUserId() != -1)
		{
			system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_RUN);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[SagaGame][Character] Run"));
}

void
ASagaInGamePlayerController::EndRun()
{
	isRunning = false;

	if constexpr (not saga::IsOfflineMode)
	{
		auto singleton = GetGameInstance();
		auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

		if (system->GetLocalUserId() != -1)
		{
			system->SendRpcPacket(ESagaRpcProtocol::RPC_END_RUN);
		}
	}
}

void ASagaInGamePlayerController::ExecuteRun()
{
	ACharacter* MyCharacter = GetCharacter();
	if (MyCharacter)
	{
		// 달리기 시작할 때 속도를 높임
		MyCharacter->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
}

void
ASagaInGamePlayerController::TerminateRun()
{
	ACharacter* MyCharacter = GetCharacter();
	if (MyCharacter)
	{
		// 달리기를 멈췄을 때 속도를 원래대로 복원
		MyCharacter->GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	}

	UE_LOG(LogTemp, Warning, TEXT("End Run"));
}

void
ASagaInGamePlayerController::BeginJump()
{
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (system->GetLocalUserId() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_JUMP);
	}
}

void
ASagaInGamePlayerController::EndJump()
{
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (system->GetLocalUserId() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_JUMP);
	}
}

void
ASagaInGamePlayerController::ExecuteJump()
{
	ASagaCharacterPlayer* ControlledPawn = GetPawn<ASagaCharacterPlayer>();
	if (ControlledPawn != nullptr)
	{
		if (ControlledPawn->CanJump())
		{
			ControlledPawn->Jump();
		}
	}
}

void ASagaInGamePlayerController::TerminateJump()
{}

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

void ASagaInGamePlayerController::TerminateRotation()
{

}

void
ASagaInGamePlayerController::BeginAttack()
{
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (system->GetLocalUserId() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_ATTACK_0);
	}
}

void
ASagaInGamePlayerController::EndAttack()
{
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (system->GetLocalUserId() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_ATTACK_0);
	}
}

void
ASagaInGamePlayerController::BeginRide()
{
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (system->GetLocalUserId() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_RIDE);
	}
}

void
ASagaInGamePlayerController::EndRide()
{
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (system->GetLocalUserId() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_RIDE);
	}
}

double
ASagaInGamePlayerController::GetNormalizedMoveDir()
const noexcept
{
	const auto angle = FMath::RadiansToDegrees(FMath::Atan2(walkDirection.Y, walkDirection.X));

	return floor(angle / 45);
}

void ASagaInGamePlayerController::Tick(float delta_time)
{
	Super::Tick(delta_time);

	if (isRunning)
	{
		ExecuteRun();
	}

	if (isForwardWalking || isStrafeWalking)
	{
		ExecuteWalk(delta_time);
	}
}

void ASagaInGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	//�������ؽ�Ʈ�� �Է½ý��ۿ� ����
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

void ASagaInGamePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
