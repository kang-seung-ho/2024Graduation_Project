#include "Character/SagaCharacterPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "SagaCharacterControlData.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Item/Items.h"
#include "Item/InventoryComponent.h"
 
ASagaCharacterPlayer::ASagaCharacterPlayer()
{
	PlayerHP = 150.0f;
	bIsRiding = false;

	WalkSpeed = 500.0f; // 기본 걷기 속도
	SprintSpeed = 1000.0f; // 달리기 속도

	//카메라
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//인벤토리
	Inventory = CreateDefaultSubobject<UInventoryComponent>("Inventory");
	//Inventory->Capacity = 20;

	//상호작용
	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(RootComponent);
	InteractionBox->SetCollisionProfileName(TEXT("Trigger"));
	InteractionBox->OnComponentBeginOverlap.AddDynamic(this, &ASagaCharacterPlayer::OnOverlapBegin);
	InteractionBox->OnComponentEndOverlap.AddDynamic(this, &ASagaCharacterPlayer::OnOverlapEnd);

	//입력
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Jump.IA_Jump'"));
	if (nullptr != InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputChangeActionControlRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_ChangeControl.IA_ChangeControl'"));
	if (nullptr != InputChangeActionControlRef.Object)
	{
		ChangeControlAction = InputChangeActionControlRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Shoulder_Move.IA_Shoulder_Move'"));
	if (nullptr != InputActionShoulderMoveRef.Object)
	{
		ShoulderMoveAction = InputActionShoulderMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Soulder_Look.IA_Soulder_Look'"));
	if (nullptr != InputActionShoulderLookRef.Object)
	{
		ShoulderLookAction = InputActionShoulderLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionQuaterMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_QuaterMove.IA_QuaterMove'"));
	if (nullptr != InputActionQuaterMoveRef.Object)
	{
		QuaterMoveAction = InputActionQuaterMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionAttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Attack.IA_Attack'"));
	if (nullptr != InputActionAttackRef.Object)
	{
		AttackAction = InputActionAttackRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> SprintActionAttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Sprint.IA_Sprint'"));
	if (nullptr != SprintActionAttackRef.Object)
	{
		SprintAction = SprintActionAttackRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InteractWithNPCRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_InteractWithNPC.IA_InteractWithNPC'"));
	if (nullptr != InteractWithNPCRef.Object)
	{
		InteractionWithNPC = InteractWithNPCRef.Object;
	}

	CurrentCharacterControlType = ECharacterControlType::Shoulder;
}

void ASagaCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	SetCharacterControl(CurrentCharacterControlType);


}

void ASagaCharacterPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ChangeControlAction, ETriggerEvent::Triggered, this, &ASagaCharacterPlayer::ChangeCharacterControl);
	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &ASagaCharacterPlayer::StartShoulderMove);
	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Completed, this, &ASagaCharacterPlayer::EndShoulderMove);
	EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &ASagaCharacterPlayer::ShoulderLook);
	EnhancedInputComponent->BindAction(QuaterMoveAction, ETriggerEvent::Triggered, this, &ASagaCharacterPlayer::StartQuaterMove);
	EnhancedInputComponent->BindAction(QuaterMoveAction, ETriggerEvent::Triggered, this, &ASagaCharacterPlayer::EndQuaterMove);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASagaCharacterPlayer::StartAttack);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &ASagaCharacterPlayer::EndAttack);

	//달리기
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ASagaCharacterPlayer::StartSprinting);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ASagaCharacterPlayer::StopSprinting);
	
	//NPC와 상호작용
	EnhancedInputComponent->BindAction(InteractionWithNPC, ETriggerEvent::Triggered, this, &ASagaCharacterPlayer::GetOnNPC);
}

void ASagaCharacterPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASagaCharacterPlayer::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		//오버랩된게 NPC인지 확인
		ASagaCharacterNPC* OverlappedNPC = Cast<ASagaCharacterNPC>(OtherActor);
		if (OverlappedNPC)
		{
			//UE_LOG(LogTemp, Warning, TEXT("NPC is around"));
			NearbyNPC = OverlappedNPC;
			bIsNPCAround = 1;
		}
	}
}

void ASagaCharacterPlayer::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bIsNPCAround = 0;
}



void ASagaCharacterPlayer::ChangeCharacterControl()
{
	if (CurrentCharacterControlType == ECharacterControlType::Quater)
	{
		SetCharacterControl(ECharacterControlType::Shoulder);
	}
	else if (CurrentCharacterControlType == ECharacterControlType::Shoulder)
	{
		SetCharacterControl(ECharacterControlType::Quater);
	}
}

void ASagaCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	USagaCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
	}

	CurrentCharacterControlType = NewCharacterControlType;
}

void ASagaCharacterPlayer::UseItem(UItems* Item)
{
	if (Item)
	{
		Item->Use(this);
		Item->OnUse(this); //블루프린트 이벤트
	}
}

void ASagaCharacterPlayer::SetCharacterControlData(const USagaCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	//카메라 스프링암 설정
	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeRotation(CharacterControlData->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch = CharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw = CharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll = CharacterControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
}

void ASagaCharacterPlayer::StartShoulderMove(const FInputActionValue& Value)
{
	ExecuteMove();
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void ASagaCharacterPlayer::EndShoulderMove(const FInputActionValue& Value)
{
	TerminateMove();
}

void ASagaCharacterPlayer::ShoulderLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ASagaCharacterPlayer::StartQuaterMove(const FInputActionValue& Value)
{
	ExecuteMove();
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	//movement벡터를 받아 크기를 1로 조정한뒤 Forward방향을 사용해 지정하면 movement component에서 설정한 옵션에 의해 캐릭터가 자동으로 이동하는 방향으로 회전하게 된다.
	float InputSizeSquared = MovementVector.SquaredLength();
	float MovementVectorSize = 1.0f;
	float MovementVectorSizeSquared = MovementVector.SquaredLength();

	if (MovementVectorSizeSquared > 1.0f)
	{
		MovementVector.Normalize();
		MovementVectorSizeSquared = 1.0f;
	}
	else
	{
		MovementVectorSize = FMath::Sqrt(MovementVectorSizeSquared);
	}

	FVector MoveDirection = FVector(MovementVector.X, MovementVector.Y, 0.0f);
	GetController()->SetControlRotation(FRotationMatrix::MakeFromX(MoveDirection).Rotator());
	AddMovementInput(MoveDirection, MovementVectorSize);

}

void ASagaCharacterPlayer::EndQuaterMove(const FInputActionValue& Value)
{
	TerminateMove();
}

void ASagaCharacterPlayer::ExecuteMove()
{

}

void ASagaCharacterPlayer::TerminateMove()
{
}

void ASagaCharacterPlayer::StartAttack()
{
	ProcessComboCommand();
	ExecuteAttack();
}

void ASagaCharacterPlayer::EndAttack()
{
	TerminateAttack();
}

void ASagaCharacterPlayer::ExecuteAttack()
{

}

void ASagaCharacterPlayer::TerminateAttack()
{

}

//void ASagaCharacterPlayer::InteractWithNPC()
//{
//	// NPC와의 상호작용 로직
//	if (bIsRiding)
//	{
//		// 하차 로직
//		bIsRiding = false;
//		// 플레이어 컨트롤 복원 코드 작성할것
//	}
//	else
//	{
//		// 탑승 로직
//		bIsRiding = true;
//		// NPC 컨트롤 로직 / NPC를 플레이어의 자식 컴포넌트로 설정
//		
//	}
//}

void ASagaCharacterPlayer::StartSprinting() //Shift로 달리기
{
	//UE_LOG(LogTemp, Warning, TEXT("Started Sprinting"));
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	ExecuteSprinting();
}

void ASagaCharacterPlayer::StopSprinting() //Shift 안누를시 달리기 해제
{
	//UE_LOG(LogTemp, Warning, TEXT("Stopped Sprinting"));
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	TerminateSprinting();
}

void ASagaCharacterPlayer::ExecuteSprinting()
{

}

void ASagaCharacterPlayer::TerminateSprinting()
{

}

void ASagaCharacterPlayer::GetOnNPC()
{
	//오버랩 된 NPC를 가져온 뒤 해당 NPC로 플레이어의 입력을 전달해야 함!!! 구현할 것.
	if (bIsRiding)
	{
		// Detach 로직작성하기
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		UE_LOG(LogTemp, Warning, TEXT("Getting off from NPC"));

		// NPC 클래스에 구현된 DetachPlayer 함수를 호출
		if (NearbyNPC)
		{
			NearbyNPC->DetachPlayer(Cast<APlayerController>(GetController()), this);
		}

		bIsRiding = false;
	}
	else {
		if (NearbyNPC && bIsNPCAround == 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("GettingOnNPC"));
			//AttachToActor(NearbyNPC, FAttachmentTransformRules::KeepWorldTransform);
			// SnapToTargetIncludingScale을 사용하여 대상의 위치, 회전, 스케일을 그대로 따르도록 함
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
			AttachToActor(NearbyNPC, AttachmentRules, TEXT("clavicle_r"));


			APlayerController* PlayerController = Cast<APlayerController>(GetController());
			if (PlayerController)
			{
				PlayerController->Possess(NearbyNPC);
			}
			bIsRiding = true;
		}
	}
	
	

	UE_LOG(LogTemp, Warning, TEXT("OnNPC"));
}
