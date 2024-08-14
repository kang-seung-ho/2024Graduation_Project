#include "Character/SagaCharacterBase.h"
#include <UObject/Linker.h>
#include <UObject/Object.h>
#include <UObject/ObjectPtr.h>
#include <UObject/ConstructorHelpers.h>
#include <Engine/StaticMesh.h>
#include <Engine/DamageEvents.h>
#include <Containers/Map.h>
#include <Templates/UnrealTemplate.h>
#include <Templates/Casts.h>
#include <Components/ProgressBar.h>
#include <Components/StaticMeshComponent.h>
#include <Components/WidgetComponent.h>
#include <Animation/AnimInstance.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/Character.h>
#include <GameFramework/SpringArmComponent.h>
#include <NiagaraSystem.h>
#include <NiagaraComponent.h>
#include <NiagaraFunctionLibrary.h>

#include "Player/SagaPlayerWeaponTypes.h"
#include "Character/SagaPlayerAnimInstance.h"
#include "Character/SagaGummyBearAnimInstance.h"
#include "Item/SagaItemTypes.h"
#include "Item/SagaWeaponData.h"
#include "UI/SagaWidgetComponent.h"
#include "UI/SagaHpBarWidget.h"
#include "PlayerControllers/SagaInGamePlayerController.h"

TMap<EPlayerWeapon, TObjectPtr<class UStaticMesh>> ASagaCharacterBase::WeaponMeshes{};

void
ASagaCharacterBase::SetUserId(const int32& id)
noexcept
{
	ownerData.myID = id;
}

void
ASagaCharacterBase::SetTeam(const ESagaPlayerTeam& team)
{
	ownerData.myTeam = team;

	if (team == ESagaPlayerTeam::Red)
	{
#if WITH_EDITOR

		const auto name = GetName();

		UE_LOG(LogSagaGame, Log, TEXT("[ASagaCharacterBase][SetTeam] '%s''s team is red."), *name);
#endif

		ChangeColliderProfileToRedTeam();
	}
	else if (team == ESagaPlayerTeam::Blue)
	{
#if WITH_EDITOR

		const auto name = GetName();

		UE_LOG(LogSagaGame, Log, TEXT("[ASagaCharacterBase][SetTeam] '%s''s team is blue."), *name);
#endif

		ChangeColliderProfileToBluTeam();
	}
	else
	{
#if WITH_EDITOR

		const auto name = GetName();
		const auto team_name = UEnum::GetValueAsString(team);

		UE_LOG(LogSagaGame, Warning, TEXT("[ASagaCharacterBase][SetTeam] '%s''s team is '%s'."), *name, *team_name);
#endif
	}
}

void
ASagaCharacterBase::SetWeapon(const EPlayerWeapon& weapon, bool attach_now)
noexcept
{
	ownerData.myWeapon = weapon;

	if (attach_now)
	{
#if WITH_EDITOR

		const auto name = GetName();
		const auto weapon_name = UEnum::GetValueAsString(weapon);

		UE_LOG(LogSagaGame, Log, TEXT("[SetWeapon] '%s''s weapon: %s"), *name, *weapon_name);
#endif

		// Set the weapon mesh from its handled weapon
		if (WeaponMeshes.Contains(weapon))
		{
#if WITH_EDITOR

			UE_LOG(LogSagaGame, Log, TEXT("[SetWeapon] '%s' has found its weapon mesh (%s)."), *name, *weapon_name);
#endif

			myWeapon->SetStaticMesh(WeaponMeshes[weapon]);

			switch (weapon)
			{
			case EPlayerWeapon::LightSabor:
			{
				myWeapon->SetRelativeLocation(FVector(0.0, 0.0, 0.0));
				myWeapon->SetRelativeRotation(FRotator(0.0, 70.0, 0.0));
				myWeapon->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
			}
			break;

			case EPlayerWeapon::WaterGun:
			{
				myWeapon->SetRelativeLocation(FVector(-0.585, -4.04, 0.09));
				myWeapon->SetRelativeRotation(FRotator(-74.24, 51.12, -86.08));
				myWeapon->SetRelativeScale3D(FVector(0.7, 0.7, 0.7));
			}
			break;

			case EPlayerWeapon::Hammer:
			{
				myWeapon->SetRelativeLocation(FVector(0.34, -2.57, -2.66));
				myWeapon->SetRelativeRotation(FRotator(-79.2, -24.29, -102.96));
				myWeapon->SetRelativeScale3D(FVector(0.7, 0.7, 0.7));
			}
			break;

			default:
			{
#if WITH_EDITOR
				UE_LOG(LogSagaGame, Error, TEXT("[SetWeapon] '%s' has a invalid weapon '%d'"), *name, static_cast<int32>(weapon));
#endif
			}
			break;
			}
		}
		else
		{
#if WITH_EDITOR
			UE_LOG(LogSagaGame, Error, TEXT("[SetWeapon] No weapon mesh has found for the selected weapon."));
#endif
		}
	}
}

void
ASagaCharacterBase::SetHealth(const float hp)
noexcept
{
	myGameStat->SetCurrentHp(hp);
}

void
ASagaCharacterBase::TranslateProperties(ASagaCharacterBase* other)
const
{
	if (not IsValid(other))
	{
#if WITH_EDITOR
		UE_LOG(LogSagaGame, Error, TEXT("[TranslateProperties] Invalid character handle."));
#endif

		return;
	}

	if (other == this)
	{
#if WITH_EDITOR
		UE_LOG(LogSagaGame, Warning, TEXT("[TranslateProperties] Invalid property transitioning."));
#endif

		return;
	}

	other->ownerData = ownerData;
	//other->ownerData.myID = ownerData.myID;
}

int32
ASagaCharacterBase::GetUserId()
const noexcept
{
	return ownerData.myID;
}

ESagaPlayerTeam
ASagaCharacterBase::GetTeam()
const noexcept
{
	return ownerData.myTeam;
}

EPlayerWeapon
ASagaCharacterBase::GetWeapon()
const noexcept
{
	return ownerData.myWeapon;
}

float
ASagaCharacterBase::GetHealth()
const noexcept
{
	return myGameStat->GetCurrentHp();
}

bool
ASagaCharacterBase::HasValidOwnerId()
const noexcept
{
	return -1 != ownerData.myID;
}

bool
ASagaCharacterBase::IsAlive()
const noexcept
{
	return 0 < myGameStat->GetCurrentHp();
}

ASagaCharacterBase::ASagaCharacterBase()
	: Super()
	, ownerData()
	, myGameStat()
	, myWeapon()
	, myAnimationInst(), mAnimInst(nullptr), mBearAnimInst(nullptr)
	, animationMoveSpeed(), animationMoveAngle()
	, myHealthIndicatorBarWidget()
	, OnCharacterDeath(), OnCharacterRespawned()
	, myCameraComponent(), myCameraSpringArmComponent()
	, straightMoveDirection(), strafeMoveDirection()
	, isRunning()
	, healthbarWidgetClass()
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

	myCameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	myCameraSpringArmComponent->SetupAttachment(mesh);
	myCameraSpringArmComponent->SetRelativeLocation(FVector(0.0, 0.0, 150.0));
	myCameraSpringArmComponent->SetRelativeRotation(FRotator(-15.0, 90.0, 0.0));
	myCameraSpringArmComponent->TargetArmLength = 150.f;

	myCameraSpringArmComponent->bEnableCameraLag = true;
	myCameraSpringArmComponent->bEnableCameraRotationLag = true;

	myCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	myCameraComponent->SetupAttachment(myCameraSpringArmComponent);

	myGameStat = CreateDefaultSubobject<USagaCharacterStatComponent>(TEXT("Stat"));
	myGameStat->SetMaxHp(100.0f);

	myHealthIndicatorBarWidget = CreateDefaultSubobject<USagaWidgetComponent>(TEXT("HpBar"));
	myHealthIndicatorBarWidget->SetupAttachment(GetMesh());
	myHealthIndicatorBarWidget->SetRelativeLocation(FVector(0.0, 0.0, 150.0));

	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_HpBar.UI_HpBar_C'"));
	if (HpBarWidgetRef.Succeeded())
	{
		healthbarWidgetClass = HpBarWidgetRef.Class;

		myHealthIndicatorBarWidget->SetWidgetClass(HpBarWidgetRef.Class);
		myHealthIndicatorBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
		myHealthIndicatorBarWidget->SetDrawSize(FVector2D(150, 20));
		myHealthIndicatorBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Weapon
	myWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	myWeapon->SetupAttachment(GetMesh(), TEXT("c_middle1_r"));
	// WeaponMesh Collision Disable
	myWeapon->SetCollisionProfileName(TEXT("Weapon"));
	//Weapon->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	// Load Weapon Meshes
	WeaponMeshes.Reserve(4);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> LightSaborMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/PlayerAssets/Weapons/Lightsaber_prop.Lightsaber_prop'"));
	if (LightSaborMeshRef.Succeeded())
	{
		WeaponMeshes.Add(EPlayerWeapon::LightSabor, LightSaborMeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> WaterGunMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/PlayerAssets/Weapons/Watergun_prop.Watergun_prop'"));
	if (WaterGunMeshRef.Succeeded())
	{
		WeaponMeshes.Add(EPlayerWeapon::WaterGun, WaterGunMeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> HammerMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/PlayerAssets/Weapons/Hammer_prop.Hammer_prop'"));
	if (HammerMeshRef.Succeeded())
	{
		WeaponMeshes.Add(EPlayerWeapon::Hammer, HammerMeshRef.Object);
	}

	//To make the AI pawn detect the player
	mAISource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AISource")); //UAIPerceptionStimuliSourceComponent is derived from UActorComponent
	mAISource->RegisterForSense(TSubclassOf<UAISense_Sight>());
	mAISource->RegisterWithPerceptionSystem();


	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraEffect(TEXT("/Script/Niagara.NiagaraSystem'/Game/VFX/VFX_Get/NS_Heal.NS_Heal'"));
	if (NiagaraEffect.Succeeded())
	{
		HealItemEffect = NiagaraEffect.Object;
		UE_LOG(LogSagaGame, Log, TEXT("[ASagaCharacterBase] Heal Niagara effect is loaded."));
	}
	else
	{
		UE_LOG(LogSagaGame, Error, TEXT("[ASagaCharacterBase] Heal Niagara effect is not loaded."));
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SmokeNiagaraEffect(TEXT("/Script/Niagara.NiagaraSystem'/Game/Item/VFX/NS_Smoke.NS_Smoke'"));
	if (SmokeNiagaraEffect.Succeeded())
	{
		SmokeItemEffect = SmokeNiagaraEffect.Object;
		UE_LOG(LogSagaGame, Log, TEXT("[ASagaCharacterBase] Smoke Niagara effect is loaded."));
	}
	else
	{
		UE_LOG(LogSagaGame, Error, TEXT("[ASagaCharacterBase] Smoke Niagara effect is not loaded."));
	}
}

void
ASagaCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// 호출되기 전에 SkeletalMeshComponent에 지정된
	// AnimInstance 클래스를 사용하기 위한 객체 만들어놨음.
	mAnimInst = Cast<USagaPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	mBearAnimInst = Cast<USagaGummyBearAnimInstance>(GetMesh()->GetAnimInstance());

	if (IsValid(myHealthIndicatorBarWidget))
	{
		const auto healthbar = Cast<USagaHpBarWidget>(myHealthIndicatorBarWidget->GetWidget());

		if (IsValid(healthbar))
		{
#if WITH_EDITOR

			const auto name = GetName();

			UE_LOG(LogSagaGame, Log, TEXT("[ASagaCharacterBase][BeginPlay] '%s' is setting up the hp bar widget..."), *name);
#endif

			healthbar->UpdateHpBar(myGameStat->GetCurrentHp() / myGameStat->GetMaxHp());

			myGameStat->OnHpChanged.AddDynamic(healthbar, &USagaHpBarWidget::UpdateHpBar);
		}
		else
		{
#if WITH_EDITOR

			const auto name = GetName();

			UE_LOG(LogSagaGame, Error, TEXT("[ASagaCharacterBase][BeginPlay] '%s' has an invalid hp bar widget."), *name);
#endif
		}
	}
	else
	{
#if WITH_EDITOR

		const auto name = GetName();

		UE_LOG(LogSagaGame, Error, TEXT("[ASagaCharacterBase][BeginPlay] '%s' has an invalid hp bar ui component."), *name);
#endif
	}

	SetCollisionEnable(true);

	myGameStat->ResetHp(true);
	myGameStat->OnHpZero.AddUniqueDynamic(this, &ASagaCharacterBase::ExecuteDeath);
}

void
ASagaCharacterBase::TakeItem(ESagaItemTypes ItemType)
{
	switch (ItemType)
	{
		// Add Drink to Player's Inventory
	case ESagaItemTypes::Drink:
	{
		UE_LOG(LogSagaGame, Log, TEXT("[ASagaCharacterBase][TakeItem] Item: Drink"));
		AddItemToInventory(ItemType);
	}
	break;

	// Add Gum to Player's Inventory
	case ESagaItemTypes::Gum:
	{
		UE_LOG(LogSagaGame, Log, TEXT("[ASagaCharacterBase][TakeItem] Item: Gum"));
		AddItemToInventory(ItemType);
	}
	break;

	// Add SmokeBomb to Player's Inventory
	case ESagaItemTypes::SmokeBomb:
	{
		UE_LOG(LogSagaGame, Log, TEXT("[ASagaCharacterBase][TakeItem] Item: SmokeBomb"));
		AddItemToInventory(ItemType);
	}
	break;

	default:
	{}
	break;
	}
}

void
ASagaCharacterBase::AddItemToInventory(ESagaItemTypes ItemType)
const
{
	const auto pc = GetController<ASagaInGamePlayerController>();

	if (IsValid(pc))
	{
		pc->AddItemToInventory(ItemType);
	}
}

void
ASagaCharacterBase::Tick(float delta_time)
{
	Super::Tick(delta_time);

	/* Process Movement */

	const FRotator rotation = K2_GetActorRotation();
	const FRotator yaw = FRotator(0.0, rotation.Yaw, 0.0);
	const FVector forward_dir = yaw.Vector();
	const FVector right_dir = FRotationMatrix(yaw).GetScaledAxis(EAxis::Y);

	AddMovementInput(forward_dir, straightMoveDirection * GetVerticalMoveAcceleration());
	AddMovementInput(right_dir, strafeMoveDirection * GetHorizontalMoveAcceleration());

	/* Process Animation */

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

float
ASagaCharacterBase::TakeDamage(float dmg, FDamageEvent const& event, AController* instigator, AActor* causer)
{
#if WITH_EDITOR

	const float actual_dmg = Super::TakeDamage(dmg, event, instigator, causer);
	const float current_hp = ExecuteHurt(actual_dmg);

	const auto name = GetName();
	UE_LOG(LogSagaGame, Log, TEXT("[ASagaCharacterBase] '%s''s TakeDamage: %f, hp: %f"), *name, actual_dmg, current_hp);
	return actual_dmg;
#else

	return ExecuteHurt(Super::TakeDamage(dmg, event, instigator, causer));
#endif
}

void
ASagaCharacterBase::StopMovement()
{
	isRunning = false;
	TerminateStraightWalk();
	TerminateStrafeWalk();

	GetCharacterMovement()->StopActiveMovement();
}

void
ASagaCharacterBase::SetCollisionEnable(const bool flag)
{
	if (flag)
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	SetActorEnableCollision(flag);
}

void
ASagaCharacterBase::ChangeColliderProfile(const FName& name)
const
{
	GetCapsuleComponent()->SetCollisionProfileName(name);
}

void
ASagaCharacterBase::ChangeColliderProfileToRedTeam()
const
{
	static FName team_name{ TEXT("Red") };

	ChangeColliderProfile(team_name);
}

void
ASagaCharacterBase::ChangeColliderProfileToBluTeam()
const
{
	static FName team_name{ TEXT("Blue") };

	ChangeColliderProfile(team_name);
}

void
ASagaCharacterBase::ChangeColliderProfileToHostile()
const
{
	static FName team_name{ TEXT("Enemy") };

	ChangeColliderProfile(team_name);
}
