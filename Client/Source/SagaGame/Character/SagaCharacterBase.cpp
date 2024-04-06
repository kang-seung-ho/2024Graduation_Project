#include "Character/SagaCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Physics/SagaCollision.h"
#include "Engine/DamageEvents.h"
#include "CharacterStat/SagaCharacterStatComponent.h"
#include "UI/SagaWidgetComponent.h"
#include "UI/SagaHpBarWidget.h"

#include "SagaComboActionData.h"
#include "SagaCharacterControlData.h"
#include "Item/SagaWeaponItemData.h"

DEFINE_LOG_CATEGORY(LogSagaCharacter);

ASagaCharacterBase::ASagaCharacterBase()
	: ACharacter()
	, ISagaAttackAnimationInterface(), ISagaCharacterItemInterface()
	, ISagaCharacterWidgetInterface()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_SAGACAPSULE);

	//움직임
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	//메시
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/PlayerAnimBP/AB_SagaPlayer.AB_SagaPlayer_c"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	//생성한 두가지 컨트롤 데이터들을 Map에 추가
	static ConstructorHelpers::FObjectFinder<USagaCharacterControlData> SoulderDataRef(TEXT("/Script/SagaGame.SagaCharacterControlData'/Game/CharacterControl/SagaControl_Shoulder.SagaControl_Shoulder'"));
	if (SoulderDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Shoulder, SoulderDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<USagaCharacterControlData> QuaterDataRef(TEXT("/Script/SagaGame.SagaCharacterControlData'/Game/CharacterControl/SagaControl_Quater.SagaControl_Quater'"));
	if (QuaterDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Quater, QuaterDataRef.Object);
	}

	//Combo
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ComboActionMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Animation/AM_ComboAttack.AM_ComboAttack'"));
	if (ComboActionMontageRef.Object)
	{
		ComboActionMontage = ComboActionMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<USagaComboActionData> ComboActionDataRef(TEXT("/Script/SagaGame.SagaComboActionData'/Game/CharacterAction/Saga_ComboAttack.Saga_ComboAttack'"));
	if (ComboActionDataRef.Object)
	{
		ComboActionData = ComboActionDataRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT(""));
	if (DeadMontageRef.Object)
	{
		DeadMontage = DeadMontageRef.Object; //
	}

	//아이템 액션
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ASagaCharacterBase::DrinkEnergyDrink)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ASagaCharacterBase::InstallGumball)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ASagaCharacterBase::EquipWeapons)));

	//무기 컴포넌트(Weapon Component)
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	//무기부착
	Weapon->SetupAttachment(GetMesh(), TEXT("hand_rSocket")); //캐릭터의 특정 본에 항상 부착되어 돌아다닐 수 있도록 소켓이름을 지정해준다.
	//캐릭터 애셋에 소켓 이름이 지정되어 있어야 한다.


	//스탯 컴포넌트
	Stat = CreateDefaultSubobject<USagaCharacterStatComponent>(TEXT("Stat"));

	//위젯 컴포넌트
	HpBar = CreateDefaultSubobject<USagaWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget>HpBarWidgetRef(TEXT("/Game/UI/WBP_HpBar.WBP_HpBar_C"));
	if (HpBarWidgetRef.Class)
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(150.f, 15.0f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void
ASagaCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Stat->OnHpZero.AddUObject(this, &ASagaCharacterBase::SetDead);
}

void
ASagaCharacterBase::SetCharacterControlData(const USagaCharacterControlData* CharacterControlData)
{
	//Pawn
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

	// CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;
}

void
ASagaCharacterBase::ProcessComboCommand() //이것과 SagaCharacterPlayer의 Attack함수가 연동되도록 Attack함수에 작성
{
	if (CurrentCombo == 0)
	{
		ComboActionBegin();
		return;
	}

	//입력이 들어올 때
	if (!ComboTimerHandle.IsValid()) //타이머가 설정 안되어있을떄 입력이 들어올때에는 이미 타이머가 발동되어 시기를 놓쳤거나 더이상 진행할 필요가 없다는 뜻
	{
		HasNextComboCommand = false;
	}
	else
	{
		HasNextComboCommand = true;
	}
}

void
ASagaCharacterBase::ComboActionBegin()
{
	//현재 콤보 상태
	CurrentCombo = 1;

	//Movement 설정
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None); //None으로 설정 시--> 공격하는 동안 이동불가

	//애니메이션 설정
	const float AttackSpeedRate = 1.0f;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(ComboActionMontage, AttackSpeedRate);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ASagaCharacterBase::ComboActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ComboActionMontage);

	ComboTimerHandle.Invalidate();
	SetComboCheckTimer();
}

void
ASagaCharacterBase::ComboActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	ensure(CurrentCombo != 0);// current combo가 0이 아닌지를 검사
	CurrentCombo = 0;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void
ASagaCharacterBase::SetComboCheckTimer()
{
	int32 ComboIndex = CurrentCombo - 1;
	ensure(ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	const float AttackSpeedRate = 1.0f;
	float ComboEffectiveTime = (ComboActionData->EffectiveFrameCount[ComboIndex] / ComboActionData->FrameRate) / AttackSpeedRate;
	if (ComboEffectiveTime > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &ASagaCharacterBase::ComboCheck, ComboEffectiveTime, false);
	}
}

void
ASagaCharacterBase::ComboCheck()
{
	ComboTimerHandle.Invalidate();
	if (HasNextComboCommand) //타이머 발동 전 입력이 들어와 HasNextComboCommand가 true값이 되면 다음 섹션으로 넘어가기
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, ComboActionData->MaxComboCount); //지정한 콤보값 벗어나지 않도록.
		FName NextSection = *FString::Printf(TEXT("%s%d"), *ComboActionData->MontageSectionNamePrefix, CurrentCombo);
		AnimInstance->Montage_JumpToSection(NextSection, ComboActionMontage);
		SetComboCheckTimer();
		HasNextComboCommand = false;
	}
}

void
ASagaCharacterBase::AttackHitCheck() //trace 활용하여 물체가 서로 충돌되는지 검사하는 로직 구현
{
	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);//false-> 복잡한 충돌체크 사용안함, this-> 무시할 액터는 자기 자신뿐이라 this 사용

	const float AttackRange = 40.0f;
	const float AttackRadius = 50.0f;
	const float AttackDamage = 30.0f;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, CCHANNEL_SAGAACTION, FCollisionShape::MakeSphere(AttackRadius), Params);
	if (HitDetected)
	{
		FDamageEvent DamageEvent;
		OutHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
	}

#if ENABLE_DRAW_DEBUG

	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);

#endif
}

float
ASagaCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Stat->ApplyDamage(DamageAmount);

	return DamageAmount;
}

void
ASagaCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);
	HpBar->SetHiddenInGame(true);
}

void
ASagaCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
}

void
ASagaCharacterBase::TakeItem(USagaItemData* InItemData)
{
	if (InItemData)
	{
		TakeItemActions[(uint8)InItemData->Type].ItemDelegate.ExecuteIfBound(InItemData); //ExecuteIfBound함수로 InItemData를 넘겨준다.
	}
}

void
ASagaCharacterBase::DrinkEnergyDrink(USagaItemData* InItemData)
{
	UE_LOG(LogSagaCharacter, Log, TEXT("Drink EnergyDrink"));
}

void
ASagaCharacterBase::EquipWeapons(USagaItemData* InItemData)
{
	USagaWeaponItemData* WeaponItemData = Cast<USagaWeaponItemData>(InItemData);
	if (WeaponItemData)
	{
		if (WeaponItemData->WeaponMesh.IsPending()) //아직 로드되지 않았다면
		{
			WeaponItemData->WeaponMesh.LoadSynchronous();
		}
		Weapon->SetSkeletalMesh(WeaponItemData->WeaponMesh.Get());
	}
	/*if (InItemData)
	{
		Weapon->SetSkeletalMesh(WeaponItemData->WeaponMesh);
	}*/
}

void
ASagaCharacterBase::InstallGumball(USagaItemData* InItemData)
{
	UE_LOG(LogSagaCharacter, Log, TEXT("Install Gumball"));
}

void
ASagaCharacterBase::SetupCharacterWidget(USagaUserWidget* InUserWidget)
{
	USagaHpBarWidget* HpBarWidget = Cast<USagaHpBarWidget>(InUserWidget);
	if (HpBarWidget)
	{
		HpBarWidget->SetMaxHp(Stat->GetMaxHp());
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp());
		Stat->OnHpChanged.AddUObject(HpBarWidget, &USagaHpBarWidget::UpdateHpBar);
	}
}
