#include "SagaPlayableCharacter.h"
#include "SagaPlayerAnimInstance.h"
#include "../Effect/SagaSwordEffect.h"

ASagaPlayableCharacter::ASagaPlayableCharacter()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/PlayerAssets/Player.Player'"));
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}

	GetMesh()->SetRelativeLocation(FVector(0.0, 0.0, -88.0));
	GetMesh()->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimAsset(TEXT("AnimBlueprint'/Game/PlayerAssets/Animation/AB_SagaPlayer.AB_SagaPlayer_C'"));
	if (AnimAsset.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimAsset.Class);
	}
	
	/*mArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	mArm->SetupAttachment(GetMesh());

	mCamera->SetupAttachment(mArm);

	mArm->SetRelativeLocation(FVector(0.0, 0.0, 150.0));
	mArm->SetRelativeRotation(FRotator(-15.0, 90.0, 0.0));

	mArm->TargetArmLength = 150.f;*/

	//Weapon Add Action
	TakeWeaponAction.Add(FTakeWeaponDelegateWrapper(FOnTakeWeaponDelegate::CreateUObject(this, &ASagaPlayableCharacter::EquipHammer)));
	TakeWeaponAction.Add(FTakeWeaponDelegateWrapper(FOnTakeWeaponDelegate::CreateUObject(this, &ASagaPlayableCharacter::EquipLightSaber)));
	TakeWeaponAction.Add(FTakeWeaponDelegateWrapper(FOnTakeWeaponDelegate::CreateUObject(this, &ASagaPlayableCharacter::EquipWaterGun)));

	//Weapon
	Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("c_middle1_r"));
}

void
ASagaPlayableCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void
ASagaPlayableCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


void
ASagaPlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void
ASagaPlayableCharacter::SwordAttack()
{
	Super::SwordAttack();

	//공격과 충돌되는 물체 여부 판단
	FHitResult Result;

	FVector Start = GetActorLocation() + GetActorForwardVector() * 50.f;
	FVector End = Start+ GetActorForwardVector() * 150.f;

	FCollisionQueryParams param = FCollisionQueryParams::DefaultQueryParam;
	param.AddIgnoredActor(this);

	bool Collision = GetWorld()->SweepSingleByChannel(Result, Start, End, FQuat::Identity, ECC_GameTraceChannel4, FCollisionShape::MakeSphere(50.f), param);

#if ENABLE_DRAW_DEBUG

	//충돌시 빨강 아니면 녹색
	FColor Color = Collision ? FColor::Red : FColor::Green;

	DrawDebugCapsule(GetWorld(), (Start+End) / 2.f, 150.f / 2.f + 50.f / 2.f, 50.f, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), Color, false, 3.f);

#endif
	if (Collision)
	{
		FDamageEvent DamageEvent;
		Result.GetActor()->TakeDamage(30.f, DamageEvent, GetController(), this);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


		ASagaSwordEffect* Effect = GetWorld()->SpawnActor<ASagaSwordEffect>(Result.ImpactPoint, Result.ImpactNormal.Rotation());

		Effect->SetParticle(TEXT("")); //이곳에 레퍼런스 복사
		Effect->SetSound(TEXT("")); //이곳에 레퍼런스 복사
	}

}

void
ASagaPlayableCharacter::EquipHammer(USagaWeaponData* WeaponData)
{
	USagaWeaponData* WeaponDataItem = Cast<USagaWeaponData>(WeaponData);
	if (WeaponDataItem)
	{
		Weapon->SetStaticMesh(WeaponDataItem->WeaponMesh);
	}

	mWeaponType = WeaponDataItem->WeaponType;
}

void
ASagaPlayableCharacter::EquipLightSaber(USagaWeaponData* WeaponData)
{
	USagaWeaponData* WeaponDataItem = Cast<USagaWeaponData>(WeaponData);
	if (WeaponDataItem)
	{
		Weapon->SetStaticMesh(WeaponDataItem->WeaponMesh);
	}

	mWeaponType = WeaponDataItem->WeaponType;
}

void
ASagaPlayableCharacter::EquipWaterGun(USagaWeaponData* WeaponData)
{
	USagaWeaponData* WeaponDataItem = Cast<USagaWeaponData>(WeaponData);
	if (WeaponDataItem)
	{
		Weapon->SetStaticMesh(WeaponDataItem->WeaponMesh);
	}
	mWeaponType = WeaponDataItem->WeaponType;
}

void
ASagaPlayableCharacter::TakeItem(USagaWeaponData* WeaponData)
{
	if (WeaponData)
	{
		TakeWeaponAction[(uint8)WeaponData->WeaponType].WeaponDelegate.ExecuteIfBound(WeaponData);
	}
}
