#include "SagaPlayableCharacter.h"
#include "SagaPlayerAnimInstance.h"
#include "../Effect/SagaSwordEffect.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

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

void
ASagaPlayableCharacter::RideNPC()
{
	UE_LOG(LogSagaGame, Warning, TEXT("RideNPC Called"))
		FOutputDeviceNull Ar;
	bool ret = CallFunctionByNameWithArguments(TEXT("RidingFunction"), Ar, nullptr, true);
	if (ret == true)
	{
		UE_LOG(LogSagaGame, Warning, TEXT("RidingFunction Called"))
	}
	else
	{
		UE_LOG(LogSagaGame, Warning, TEXT("RidingFunction Not Found"))
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

float
ASagaPlayableCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	myClientHP -= DamageAmount;

	if (myClientHP <= 0.0f)
	{
		//잠시 Collision 해제
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//사망애니메이션 실행

		
		//리스폰 함수 실행
		// RespawnCharacter 함수 3초 뒤	실행
		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ASagaPlayableCharacter::RespawnCharacter, 3.0f, false);

		//상대 팀 점수 증가 실행
		auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());
		if (system)
		{
			system->AddScore(myTEAM == EUserTeam::Red ? EUserTeam::Blue : EUserTeam::Red, 1);
		}
	}
	return DamageAmount;
}

void ASagaPlayableCharacter::RespawnCharacter()
{
	
	myClientHP = 100.0f;

	FVector SpawnLocation = FVector(-760.f, 3930.0f, 330.0f);
	FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);

	isForwardWalking = false;
	isStrafeWalking = false;
	isRunning = false;


	SetActorLocationAndRotation(SpawnLocation, SpawnRotation);

	//Collision Enable Codes

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());
	system->GetLocalUserTeam(myTEAM); //re-save team color from system
	SetTeamColorAndCollision();	//re-set team collision
	GetCharacterMovement()->StopActiveMovement();

	

	UE_LOG(LogTemp, Warning, TEXT("Character respawned at Location: %s"), *SpawnLocation.ToString());
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
	myTEAM = EUserTeam::Red; //Code For Client Test
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
			UE_LOG(LogSagaGame, Warning, TEXT("Using Red Team Collision - LightSaber"))
				Collision = GetWorld()->SweepSingleByChannel(Result, Start, End, FQuat::Identity, ECC_GameTraceChannel4, FCollisionShape::MakeSphere(50.f), param);
		}
		else if (myTEAM == EUserTeam::Blue)
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Using Blue Team Collision - LightSaber"))
				Collision = GetWorld()->SweepSingleByChannel(Result, Start, End, FQuat::Identity, ECC_GameTraceChannel7, FCollisionShape::MakeSphere(50.f), param);
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Not Found Team"));
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
		FHitResult Result;

		FVector TraceStart = GetActorLocation();
		FVector TraceEnd = GetActorLocation() + GetActorForwardVector() * 1500.0f;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		bool Collision{};

		if (myTEAM == EUserTeam::Red)
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Using Red Team Collision - WaterGun"));

			Collision = GetWorld()->LineTraceSingleByChannel(Result, TraceStart, TraceEnd, ECC_GameTraceChannel4, QueryParams);
		}
		else if (myTEAM == EUserTeam::Blue)
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Using Blue Team Collision - WaterGun"));

			Collision = GetWorld()->LineTraceSingleByChannel(Result, TraceStart, TraceEnd, ECC_GameTraceChannel7, QueryParams);
		}


		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Result.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 10.0f);
		UE_LOG(LogSagaGame, Log, TEXT("Tracing line: %s to %s"), *TraceStart.ToCompactString(), *TraceEnd.ToCompactString());

		if (Result.bBlockingHit && IsValid(Result.GetActor()))
		{
			UE_LOG(LogSagaGame, Log, TEXT("Trace hit actor: %s"), *Result.GetActor()->GetName());
		}
		else
		{
			UE_LOG(LogSagaGame, Log, TEXT("No Actors were hit"));
		}

		if (Collision)
		{
			FDamageEvent DamageEvent;
			Result.GetActor()->TakeDamage(20.f, DamageEvent, GetController(), this);

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


			ASagaSwordEffect* Effect = GetWorld()->SpawnActor<ASagaSwordEffect>(Result.ImpactPoint, Result.ImpactNormal.Rotation());
			if (Effect)
			{
				Effect->SetParticle(TEXT("")); //이곳에 레퍼런스 복사
				Effect->SetSound(TEXT("")); //이곳에 레퍼런스 복사
			}

		}

	}
	else if (mWeaponType == EPlayerWeapon::Hammer)
	{

	}
	else
	{
		UE_LOG(LogSagaGame, Warning, TEXT("Not Found Weapon"));
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
