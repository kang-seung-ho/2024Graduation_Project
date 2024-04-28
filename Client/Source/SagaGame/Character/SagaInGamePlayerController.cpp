// Fill out your copyright notice in the Description page of Project Settings.


#include "SagaInGamePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "../Input/SagaInputSystem.h"
#include "SagaCharacterPlayer.h"
#include "SagaNetwork/Public/Saga/Network/SagaNetworkSubSystem.h"
#include "SagaNetwork/Public/Saga/Network/SagaRpcProtocol.h"

ASagaInGamePlayerController::ASagaInGamePlayerController(const FObjectInitializer& ObjectInitializer) : APlayerController(ObjectInitializer)
{
	mMoveDir = 0.f;

	//GetGameInstance<class USagaNetworkSubSystem>();
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

void ASagaInGamePlayerController::SetupInputComponent()
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

//Begin, End -> RPC ȣ��
//Execute, Terminate -> Ŭ����� ����
void ASagaInGamePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


void ASagaInGamePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(isForwardWalking || isStrafeWalking)
	{
		ExecuteWalk();
	}
	if (isRunning)
	{
		ExecuteRun();
	}
}

void ASagaInGamePlayerController::ExecuteWalk()
{
	
	APawn* ControlledPawn = GetPawn();

	const FRotator Rotation = K2_GetActorRotation();
	const FRotator YawRotation = FRotator(0.0, Rotation.Yaw, 0.0);
	const FVector ForwardVector = YawRotation.Vector();
	const FVector RightVector = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);

	ControlledPawn->AddMovementInput(ForwardVector, WalkDirection.Y);
	ControlledPawn->AddMovementInput(RightVector, WalkDirection.X);

	//��: InputValue.X�� 1
	//��: InputValue.Y�� -1
	//�Ѵ� X : 0
	mMoveDir = WalkDirection.X * 90.f;

	//������ �̵�
	if (WalkDirection.Y > 0.f)
	{
		//������, + ����
		if (WalkDirection.X < 0.f)
		{
			mMoveDir = -45.f;
		}
		//������, + ������
		else if (WalkDirection.X > 0.f)
		{
			mMoveDir = 45.f;
		}
	}
	else if (WalkDirection.Y < 0.f)
	{
		//�ڷ�, + ����
		if (WalkDirection.X < 0.f)
		{
			mMoveDir = -135.f;
		}
		//�ڷ�, + ������
		else if (WalkDirection.X > 0.f)
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


void
ASagaInGamePlayerController::BeginForwardWalk(const FInputActionValue& Value)
{
	isForwardWalking = true;
	FVector TempDirection = Value.Get<FVector>();
	WalkDirection.Y = TempDirection.Y;
	float WalkAngle = FMath::RadiansToDegrees(FMath::Atan2(WalkDirection.Y, WalkDirection.X)) / 45;

	auto singleton = GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>(); //�����߻�����


	if (system->GetLocalUserId() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_WALK, floor(WalkAngle));
	}

	

	FString KeyAsString = Value.ToString();
	UE_LOG(LogTemp, Warning, TEXT("Y    KeyAsString : %s"), *KeyAsString);
}

void
ASagaInGamePlayerController::EndForwardWalk(const FInputActionValue& Value)
{
	isForwardWalking = false;

	FVector TempDirection = Value.Get<FVector>();
	WalkDirection.Y = TempDirection.Y;
	float WalkAngle = FMath::RadiansToDegrees(FMath::Atan2(WalkDirection.Y, WalkDirection.X)) / 45;
	auto singleton = GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (system->GetLocalUserId() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_WALK, floor(WalkAngle));
	}

}

void ASagaInGamePlayerController::BeginStrafeWalk(const FInputActionValue& Value)
{
	isStrafeWalking = true;

	FVector TempDirection = Value.Get<FVector>();
	WalkDirection.X = TempDirection.X;
	float WalkAngle = FMath::RadiansToDegrees(FMath::Atan2(WalkDirection.Y, WalkDirection.X)) / 45;

	FString KeyAsString = Value.ToString();

	auto singleton = GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (system->GetLocalUserId() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_WALK, floor(WalkAngle));
	}

	UE_LOG(LogTemp, Warning, TEXT("X     KeyAsString : %s"), *KeyAsString);
}

void ASagaInGamePlayerController::EndStrafeWalk(const FInputActionValue& Value)
{
	isStrafeWalking = false;

	FVector TempDirection = Value.Get<FVector>();
	WalkDirection.X = TempDirection.X;
	float WalkAngle = FMath::RadiansToDegrees(FMath::Atan2(WalkDirection.Y, WalkDirection.X)) / 45;

	auto singleton = GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (system->GetLocalUserId() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_WALK, floor(WalkAngle));
	}

}



void
ASagaInGamePlayerController::BeginRun()
{
	isRunning = true;
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (system->GetLocalUserId() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_BEG_RUN);
	}
}

void
ASagaInGamePlayerController::EndRun()
{
	isRunning = false;
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (system->GetLocalUserId() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_RUN);
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
	UE_LOG(LogTemp, Warning, TEXT("Run"));
}

void ASagaInGamePlayerController::TerminateRun()
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

void ASagaInGamePlayerController::ExecuteJump()
{
	ASagaCharacterPlayer* ControlledPawn = GetPawn<ASagaCharacterPlayer>();
	if (ControlledPawn != nullptr) {
		if (ControlledPawn->CanJump())
			ControlledPawn->Jump();
	}


}

void ASagaInGamePlayerController::TerminateJump()
{
}

void ASagaInGamePlayerController::BeginRotation()
{
}

void ASagaInGamePlayerController::EndRotation()
{
}

void ASagaInGamePlayerController::ExecuteRotation(const FInputActionValue& Value)
{
	const FVector	InputValue = Value.Get<FVector>();

	AddYawInput(InputValue.X);

	ASagaCharacterPlayer* ControlledPawn = GetPawn<ASagaCharacterPlayer>();
	if (ControlledPawn != nullptr) {
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
