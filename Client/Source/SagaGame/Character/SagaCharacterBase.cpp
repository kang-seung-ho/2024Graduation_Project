#include "Character/SagaCharacterBase.h"
#include <Engine/DamageEvents.h>
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Components/WidgetComponent.h>

#include "SagaGameInfo.h"
#include "SagaPlayerAnimInstance.h"
#include "SagaGummyBearAnimInstance.h"
#include "Item/SagaWeaponData.h"
#include "UI/SagaWidgetComponent.h"
#include "UI/SagaHpBarWidget.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaCharacterBase::ASagaCharacterBase()
	: Super()
	, myId(-1), myTeam(EUserTeam::Unknown)
	, straightMoveDirection(), strafeMoveDirection()
	, isRunning()
	, mAnimInst(nullptr), mBearAnimInst(nullptr)
	, animationMoveSpeed(), animationMoveAngle()
	, mCamera(), mArm()
	, myGameStat(), HpBar()
	, MyWeapon(), WeaponMeshes()
{
	PrimaryActorTick.bCanEverTick = true;

	// Creating UCharacterMovementComponent
	bReplicates = true;
	SetReplicateMovement(true);

	const auto mesh = GetMesh();
	ensure(mesh != nullptr);

	mesh->SetRelativeLocation(FVector(0.0, 0.0, -88.0));
	mesh->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));
	mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/PlayerAssets/Player.Player'"));
	if (MeshAsset.Succeeded())
	{
		mesh->SetSkeletalMesh(MeshAsset.Object);
	}

	mArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	mArm->SetupAttachment(mesh);
	mArm->SetRelativeLocation(FVector(0.0, 0.0, 150.0));
	mArm->SetRelativeRotation(FRotator(-15.0, 90.0, 0.0));
	mArm->TargetArmLength = 150.f;

	mArm->bEnableCameraLag = true;
	mArm->bEnableCameraRotationLag = true;

	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	mCamera->SetupAttachment(mArm);

	myGameStat = CreateDefaultSubobject<USagaCharacterStatComponent>(TEXT("Stat"));
	myGameStat->SetMaxHp(100.0f);

	HpBar = CreateDefaultSubobject<USagaWidgetComponent>(TEXT("HpBar"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.0, 0.0, 150.0));

	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_HpBar.UI_HpBar_C'"));
	if (HpBarWidgetRef.Succeeded())
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(150, 20));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	//Weapon
	MyWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	MyWeapon->SetupAttachment(GetMesh(), TEXT("c_middle1_r"));
	// WeaponMesh Collision Disable
	MyWeapon->SetCollisionProfileName(TEXT("Weapon"));
	//Weapon->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	//Load Weapon Meshes
	static ConstructorHelpers::FObjectFinder<UStaticMesh> LightSaborMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/PlayerAssets/Weapons/Lightsaber_prop.Lightsaber_prop'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WaterGunMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/PlayerAssets/Weapons/Watergun_prop.Watergun_prop'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> HammerMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/PlayerAssets/Weapons/Hammer_prop.Hammer_prop'"));

	if (LightSaborMeshRef.Succeeded())
	{
		WeaponMeshes.Add(EPlayerWeapon::LightSabor, LightSaborMeshRef.Object);
	}
	if (WaterGunMeshRef.Succeeded())
	{
		WeaponMeshes.Add(EPlayerWeapon::WaterGun, WaterGunMeshRef.Object);
	}
	if (HammerMeshRef.Succeeded())
	{
		WeaponMeshes.Add(EPlayerWeapon::Hammer, HammerMeshRef.Object);
	}
}

void ASagaCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	myGameStat->OnHpZero.AddDynamic(this, &ASagaCharacterBase::SetDead);
}

void
ASagaCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// BeginPlay 호출되기 전에 SkeletalMeshComponent에 지정된
	// AnimInstance 클래스를 사용하기 위한 객체 만들어놨음.
	mAnimInst = Cast<USagaPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	mBearAnimInst = Cast<USagaGummyBearAnimInstance>(GetMesh()->GetAnimInstance());

	if (not HasValidOwnerId())
	{
		SetTeamColorAndCollision(EUserTeam::Blue);
	}
}

void
ASagaCharacterBase::Tick(float delta_time)
{
	Super::Tick(delta_time);

	ProcessMovement();
	ProcessAnimation(delta_time);
}

void
ASagaCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float
ASagaCharacterBase::TakeDamage(float dmg, FDamageEvent const& event, AController* instigator, AActor* causer)
{
	const float actual_dmg = Super::TakeDamage(dmg, event, instigator, causer);
	const float current_hp = ExecuteHurt(actual_dmg);

#if WITH_EDITOR
	const auto name = GetName();
	UE_LOG(LogSagaGame, Warning, TEXT("[ASagaCharacterBase] '%s''s TakeDamage: %f, hp: %f"), *name, actual_dmg, current_hp);
#endif

	return actual_dmg;
}

void
ASagaCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void
ASagaCharacterBase::TranslateProperties(ASagaCharacterBase* other)
const
{
	if (other == this)
	{
		UE_LOG(LogSagaGame, Error, TEXT("Invalid Character Transitioning."));
		return;
	}
}

void
ASagaCharacterBase::ProcessMovement()
{
	const FRotator rotation = K2_GetActorRotation();
	const FRotator yaw = FRotator(0.0, rotation.Yaw, 0.0);
	const FVector forward_dir = yaw.Vector();
	const FVector right_dir = FRotationMatrix(yaw).GetScaledAxis(EAxis::Y);

	AddMovementInput(forward_dir, straightMoveDirection * GetVerticalMoveAcceleration());
	AddMovementInput(right_dir, strafeMoveDirection * GetHorizontalMoveAcceleration());
}

void
ASagaCharacterBase::ProcessAnimation(const float& delta_time)
{
	UE::Math::TVector2<float> move_direction{ float(strafeMoveDirection), float(straightMoveDirection) };
	move_direction.Normalize();

	// Stores animation-integrated values
	const float target_animation_speed{ move_direction.Length() * GetMaxMoveSpeed(isRunning) };

	// Accelerate toward to target direction
	if (target_animation_speed != animationMoveSpeed)
	{
		float speed_delta = 500.0f * delta_time;
		if (straightMoveDirection == 0 and strafeMoveDirection == 0)
		{
			speed_delta *= 10;
		}

		const float speed_gap = target_animation_speed - animationMoveSpeed;

		if (FMath::Abs(speed_gap) < speed_delta)
		{
			animationMoveSpeed = target_animation_speed;
		}
		else
		{
			animationMoveSpeed += FMath::Sign(speed_gap) * speed_delta;
		}
	}

	float target_animation_angle{};
	if (0 < straightMoveDirection) // Going forward
	{
		if (strafeMoveDirection < 0) // To left
		{
			target_animation_angle = -45.f;
		}
		else if (0 < strafeMoveDirection) // To right
		{
			target_animation_angle = 45.f;
		}
		else // To straight
		{
			target_animation_angle = 0.0f;
		}
	}
	else if (straightMoveDirection < 0) // Going backward
	{
		if (strafeMoveDirection < 0) // To left
		{
			target_animation_angle = -135.f;
		}
		else if (0 < strafeMoveDirection) // To right
		{
			target_animation_angle = 135.f;
		}
		else // To straight
		{
			target_animation_angle = 180.f;
		}
	}
	else
	{
		target_animation_angle = strafeMoveDirection * 90.f;
	}

	if (target_animation_angle != animationMoveAngle)
	{
		const auto delta = GetMoveAnimationDirectionDelta() * delta_time;
		const auto gap = target_animation_angle - animationMoveAngle;
		auto angle = animationMoveAngle;

		if (gap > 180)
		{
			angle += 360;
		}
		else if (gap < -180)
		{
			angle -= 360;
		}

		const auto interpolated_gap = target_animation_angle - angle;
		if (FMath::Abs(gap) < delta)
		{
			animationMoveAngle = target_animation_angle;
		}
		else
		{
			animationMoveAngle += FMath::Sign(interpolated_gap) * delta;
		}
	}
}

void
ASagaCharacterBase::AttachWeapon()
{
	const auto name = UEnum::GetValueAsString(myWeaponType);

	UE_LOG(LogSagaGame, Log, TEXT("[AttachWeapon] WeaponType: %s"), *name);

	// Set Weapon Mesh with Weapon Type
	if (WeaponMeshes.Contains(myWeaponType))
	{
		MyWeapon->SetStaticMesh(WeaponMeshes[myWeaponType]);

		UE_LOG(LogSagaGame, Log, TEXT("[AttachWeapon] Found weapon '%s''s mesh."), *name);

		if (myWeaponType == EPlayerWeapon::LightSabor)
		{
			MyWeapon->SetRelativeLocation(FVector(0.0, 0.0, 0.0));
			MyWeapon->SetRelativeRotation(FRotator(0.0, 70.0, 0.0));
			MyWeapon->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
		}
		else if (myWeaponType == EPlayerWeapon::WaterGun)
		{
			MyWeapon->SetRelativeLocation(FVector(-0.585, -4.04, 0.09));
			MyWeapon->SetRelativeRotation(FRotator(-74.24, 51.12, -86.08));
			MyWeapon->SetRelativeScale3D(FVector(0.7, 0.7, 0.7));
		}
		else if (myWeaponType == EPlayerWeapon::Hammer)
		{
			MyWeapon->SetRelativeLocation(FVector(0.34, -2.57, -2.66));
			MyWeapon->SetRelativeRotation(FRotator(-79.2, -24.29, -102.96));
			MyWeapon->SetRelativeScale3D(FVector(0.7, 0.7, 0.7));
		}
	}
	else
	{
		UE_LOG(LogSagaGame, Error, TEXT("[AttachWeapon] No weapon mesh has found for the selected weapon."));
	}
}

void
ASagaCharacterBase::SetupCharacterWidget(USagaUserWidget* InUserWidget)
{
	USagaHpBarWidget* HpBarWidget = Cast<USagaHpBarWidget>(InUserWidget);
	if (HpBarWidget)
	{
		HpBarWidget->SetMaxHp(myGameStat->GetMaxHp());
		HpBarWidget->UpdateHpBar(myGameStat->GetCurrentHp());

		myGameStat->OnHpChanged.AddUniqueDynamic(HpBarWidget, &USagaHpBarWidget::UpdateHpBar);
	}
}

void
ASagaCharacterBase::PlayAttackAnimation()
{
	UE_LOG(LogSagaGame, Warning, TEXT("Entered PlayAttackAnimation"));

	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	const int32 CharacterMode = system->GetCurrentMode();
	UE_LOG(LogSagaGame, Warning, TEXT("Character Mode : %d"), CharacterMode);

	if (CharacterMode == 1)
	{
		if (mAnimInst != nullptr)
		{
			mAnimInst->PlayAttackMontage();
			UE_LOG(LogSagaGame, Warning, TEXT("This character is a SagaPlayableCharacter"));
		}
		else
		{
			UE_LOG(LogSagaGame, Error, TEXT("mAnimInst is null"));
		}
	}
	else if (CharacterMode == 2)
	{
		if (mBearAnimInst != nullptr)
		{
			mBearAnimInst->PlayAttackMontage();
			UE_LOG(LogSagaGame, Warning, TEXT("This character is a SagaBear"));
		}
		else
		{
			UE_LOG(LogSagaGame, Error, TEXT("mBearAnimInst is null"));
		}
	}
	else
	{
		UE_LOG(LogSagaGame, Error, TEXT("Invalid Character Mode."));
	}
}

void
ASagaCharacterBase::SetDead()
{
	HpBar->SetHiddenInGame(true);
}

void
ASagaCharacterBase::SetUserId(const int32& id)
noexcept
{
	myId = id;
}

void
ASagaCharacterBase::SetTeam(const EUserTeam& team) noexcept
{
	myTeam = team;
}

void
ASagaCharacterBase::SetTeamColorAndCollision(const EUserTeam& team)
noexcept
{
	myTeam = team;

	if (team == EUserTeam::Red)
	{
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("Red"));
	}
	else
	{
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("Blue"));
	}
}

void
ASagaCharacterBase::SetWeapon(const EPlayerWeapon& weapon)
noexcept
{
	myWeaponType = weapon;
}

void
ASagaCharacterBase::SetHealth(const float hp)
noexcept
{
	myGameStat->SetCurrentHp(hp);
}

int32
ASagaCharacterBase::GetUserId()
const noexcept
{
	return myId;
}

EUserTeam
ASagaCharacterBase::GetTeamColorAndCollision()
const noexcept
{
	return myTeam;
}

EPlayerWeapon
ASagaCharacterBase::GetWeapon() const noexcept
{
	return myWeaponType;
}

float
ASagaCharacterBase::GetHealth()
const noexcept
{
	return myGameStat->GetCurrentHp();
}

bool
ASagaCharacterBase::IsAlive()
const noexcept
{
	return 0 < myGameStat->GetCurrentHp();
}

bool
ASagaCharacterBase::HasValidOwnerId()
const noexcept
{
	return -1 != myId;
}
