#include "SagaInGamePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "Input/SagaInputSystem.h"
#include "SagaCharacterPlayer.h"
#include "SagaGummyBearPlayer.h"

#include "Saga/Network/SagaNetworkSubSystem.h"
#include "Blueprint/UserWidget.h"

ASagaInGamePlayerController::ASagaInGamePlayerController(const FObjectInitializer& ObjectInitializer)
	: APlayerController(ObjectInitializer)
	, walkDirection()
	, isRiding(), OnRideNPC()
	, mTeamScoreBoardClass(), mTeamScoreBoard()
	, lastCharacterPosition(), lastCharacterRotation(), tranformUpdateTimer()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_ScoreBoard.UI_ScoreBoard_C'"));

	if (WidgetClass.Succeeded())
	{
		mTeamScoreBoardClass = WidgetClass.Class;
	}
}

AActor*
ASagaInGamePlayerController::CreatePlayableCharacter(UClass* type, const FTransform& transform)
const
{
	if (type != nullptr)
	{
		FActorSpawnParameters setting{};
		setting.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		UE_LOG(LogSagaGame, Log, TEXT("[SagaGame] Creating a playable character"));
		return GetWorld()->SpawnActor(type, &transform, MoveTempIfPossible(setting));
	}
	else
	{
		return nullptr;
	}
}

void
ASagaInGamePlayerController::TriggerRideNPC(const FInputActionValue& Value)
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
ASagaInGamePlayerController::RideNPCCallFunction()
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
	//

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (nullptr != system)
	{
		system->OnStartGame.AddDynamic(this, &ASagaInGamePlayerController::OnGameStarted);

		system->OnCreatingCharacter.AddDynamic(this, &ASagaInGamePlayerController::OnCreatingCharacter);

		system->OnUpdatePosition.AddDynamic(this, &ASagaInGamePlayerController::OnUpdatePosition);
		system->OnUpdateRotation.AddDynamic(this, &ASagaInGamePlayerController::OnUpdateRotation);

		system->OnRpc.AddDynamic(this, &ASagaInGamePlayerController::OnRpc);
	}
	else
	{
		UE_LOG(LogSagaGame, Error, TEXT("Network subsystem is not ready."));
	}

	FTimerHandle CountdownTimerHandle{};
	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ASagaInGamePlayerController::CountDown, 1.0f, true, 0.0);

	if (IsValid(mTeamScoreBoardClass))
	{
		mTeamScoreBoard = CreateWidget<UUserWidget>(GetWorld(), mTeamScoreBoardClass);

		if (IsValid(mTeamScoreBoard))
		{
			mTeamScoreBoard->AddToViewport();
		}
	}

	OnLevelReady();
}

void ASagaInGamePlayerController::CountDown()
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

	Input->BindAction(InputSystem->Interact, ETriggerEvent::Started, this, &ASagaInGamePlayerController::TriggerRideNPC);

	// �̺�Ʈ�� ��������Ʈ ���ε�
	OnRideNPC.AddDynamic(this, &ASagaInGamePlayerController::RideNPCCallFunction);
}
