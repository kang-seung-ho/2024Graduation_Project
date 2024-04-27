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

	
}

void ASagaInGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	//매핑컨텍스트를 입력시스템에 지정
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
	
}

//Begin, End -> RPC 호출
//Execute, Terminate -> 클라로직 실행
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

	//우: InputValue.X가 1
	//왼: InputValue.Y가 -1
	//둘다 X : 0
	mMoveDir = WalkDirection.X * 90.f;

	//앞으로 이동
	if (WalkDirection.Y > 0.f)
	{
		//앞으로, + 왼쪽
		if (WalkDirection.X < 0.f)
		{
			mMoveDir = -45.f;
		}
		//앞으로, + 오른쪽
		else if (WalkDirection.X > 0.f)
		{
			mMoveDir = 45.f;
		}
	}
	else if (WalkDirection.Y < 0.f)
	{
		//뒤로, + 왼쪽
		if (WalkDirection.X < 0.f)
		{
			mMoveDir = -135.f;
		}
		//뒤로, + 오른쪽
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
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>(); //에러발생지점


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
	auto singleton = GEngine->GetWorld()->GetGameInstance();
	auto system = singleton->GetSubsystem<USagaNetworkSubSystem>();

	if (system->GetLocalUserId() != -1)
	{
		system->SendRpcPacket(ESagaRpcProtocol::RPC_END_RUN);
	}
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
