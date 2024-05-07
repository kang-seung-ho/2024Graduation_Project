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
	TakeItemAction.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ASagaPlayableCharacter::Acquire_Drink)));
	TakeItemAction.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ASagaPlayableCharacter::Acquire_Gum)));
	TakeItemAction.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ASagaPlayableCharacter::Acquire_smokebomb)));

	//Weapon
	//Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	//Weapon->SetupAttachment(GetMesh(), TEXT("c_middle1_r"));
}

void ASagaPlayableCharacter::RideNPC()
{
	UE_LOG(LogTemp, Warning, TEXT("RideNPC Called"))
	FOutputDeviceNull Ar;
	bool ret = CallFunctionByNameWithArguments(TEXT("RidingFunction"), Ar, nullptr, true);
	if (ret == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("RidingFunction Called"))
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("RidingFunction Not Found"))
	}
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
ASagaPlayableCharacter::Attack()
{
	Super::Attack();

	if (mWeaponType == EPlayerWeapon::LightSabor)
	{
		//공격과 충돌되는 물체 여부 판단
		FHitResult Result;

		FVector Start = GetActorLocation() + GetActorForwardVector() * 50.f;
		FVector End = Start + GetActorForwardVector() * 150.f;

		FCollisionQueryParams param = FCollisionQueryParams::DefaultQueryParam;
		param.AddIgnoredActor(this);

		bool Collision;
		if (myTEAM == EUserTeam::Red)
		{
			Collision = GetWorld()->SweepSingleByChannel(Result, Start, End, FQuat::Identity, ECC_GameTraceChannel4, FCollisionShape::MakeSphere(50.f), param);
		}
		else if (myTEAM == EUserTeam::Blue)
		{
			Collision = GetWorld()->SweepSingleByChannel(Result, Start, End, FQuat::Identity, ECC_GameTraceChannel5, FCollisionShape::MakeSphere(50.f), param);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Not Found Team"));
			return;
		}

#if ENABLE_DRAW_DEBUG

		//충돌시 빨강 아니면 녹색
		FColor Color = Collision ? FColor::Red : FColor::Green;

		DrawDebugCapsule(GetWorld(), (Start + End) / 2.f, 150.f / 2.f + 50.f / 2.f, 50.f, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), Color, false, 3.f);

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
	else if (mWeaponType == EPlayerWeapon::WaterGun)
	{
		FHitResult Hit;

		// We set up a line trace from our current location to a point 1000cm ahead of us
		FVector TraceStart = GetActorLocation();
		FVector TraceEnd = GetActorLocation() + GetActorForwardVector() * 1000.0f;

		// You can use FCollisionQueryParams to further configure the query
		// Here we add ourselves to the ignored list so we won't block the trace
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		// To run the query, you need a pointer to the current level, which you can get from an Actor with GetWorld()
		// UWorld()->LineTraceSingleByChannel runs a line trace and returns the first actor hit over the provided collision channel.
		GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_GameTraceChannel4, QueryParams);

		// You can use DrawDebug helpers and the log to help visualize and debug your trace queries.
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 10.0f);
		UE_LOG(LogTemp, Log, TEXT("Tracing line: %s to %s"), *TraceStart.ToCompactString(), *TraceEnd.ToCompactString());

		// If the trace hit something, bBlockingHit will be true,
		// and its fields will be filled with detailed info about what was hit
		if (Hit.bBlockingHit && IsValid(Hit.GetActor()))
		{
			UE_LOG(LogTemp, Log, TEXT("Trace hit actor: %s"), *Hit.GetActor()->GetName());
		}
		else {
			UE_LOG(LogTemp, Log, TEXT("No Actors were hit"));
		}
	}
	else if (mWeaponType == EPlayerWeapon::Hammer)
	{

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Found Weapon"));
	}

}

void
ASagaPlayableCharacter::Acquire_Drink(USagaWeaponData* ItemData)
{
	USagaWeaponData* AcquiredItemData = Cast<USagaWeaponData>(ItemData);
	if (AcquiredItemData)
	{
		//HP 회복 전송 코드 or 아이템 획득 전송 코드
		//로컬 인벤토리에 저장
	}

	mItemType = AcquiredItemData->ItemType;
}

void
ASagaPlayableCharacter::Acquire_Gum(USagaWeaponData* ItemData)
{
	USagaWeaponData* AcquiredItemData = Cast<USagaWeaponData>(ItemData);
	if (AcquiredItemData)
	{
		//아이템 획득 전송 코드
		//로컬 인벤토리에 저장
	}

	mItemType = AcquiredItemData->ItemType;
}

void
ASagaPlayableCharacter::Acquire_smokebomb(USagaWeaponData* ItemData)
{
	USagaWeaponData* AcquiredItemData = Cast<USagaWeaponData>(ItemData);
	if (AcquiredItemData)
	{
		//아이템 획득 전송 코드
		//로컬 인벤토리에 저장
	}
	mItemType = AcquiredItemData->ItemType;
}

void
ASagaPlayableCharacter::TakeItem(USagaWeaponData* WeaponData)
{
	if (WeaponData)
	{
		TakeItemAction[(uint8)WeaponData->ItemType].ItemDelegate.ExecuteIfBound(WeaponData);
	}
}
