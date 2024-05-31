#include "SagaPlayableCharacter.h"
#include "SagaPlayerAnimInstance.h"
#include "../Effect/SagaSwordEffect.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


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
		humanCharacterAnimation = AnimAsset.Class;
	}

	/*mArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	mArm->SetupAttachment(GetMesh());

	mCamera->SetupAttachment(mArm);

	mArm->SetRelativeLocation(FVector(0.0, 0.0, 150.0));
	mArm->SetRelativeRotation(FRotator(-15.0, 90.0, 0.0));

	mArm->TargetArmLength = 150.f;*/

	// ASagaPlayableCharacter 클래스 내 생성자에 추가
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraEffect(TEXT("Niagara.NiagaraSystem'/Game/PartyFX/Niagara/NS_Fireworks_Star.NS_Fireworks_Star'"));
	if (NiagaraEffect.Succeeded())
	{
		HitEffect = NiagaraEffect.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> CascadeEffect(TEXT("ParticleSystem'/Game/Hit_VFX/VFX/Hard_Hit/P_Hit_5.P_Hit_5'"));
	if (CascadeEffect.Succeeded())
	{
		HitCascadeEffect = CascadeEffect.Object;
	}


	//Item get Action
	TakeItemAction.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ASagaPlayableCharacter::Acquire_Drink)));
	TakeItemAction.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ASagaPlayableCharacter::Acquire_Gum)));
	TakeItemAction.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ASagaPlayableCharacter::Acquire_smokebomb)));
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
	//myWeaponType = EPlayerWeapon::LightSabor;
	UE_LOG(LogSagaGame, Warning, TEXT("Playable Character BeginPlay"));
}

void
ASagaPlayableCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

float
ASagaPlayableCharacter::ExecuteHurt(const float dmg)
{
	UE_LOG(LogSagaGame, Log, TEXT("[Character] ExecuteHurt (%f)"), dmg);

	Stat->ApplyDamage(dmg);
	myHealth -= dmg;

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	FVector NiagaraSpawnLocation = GetActorLocation();
	FRotator NiagaraSpawnRotation = GetActorRotation();

	FVector EffectSpawnLocation = GetActorLocation();
	FRotator EffectSpawnRotation = GetActorRotation();


	if (HitCascadeEffect)
	{
		UParticleSystemComponent* CascadeComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitCascadeEffect,
			EffectSpawnLocation,
			EffectSpawnRotation,
			true
		);

		if (CascadeComponent)
		{
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &ASagaPlayableCharacter::DeactivateCascadeEffect, CascadeComponent), 3.0f, false);
		}
	}
	

	if (myHealth <= 0.0f)
	{
		// 사망 애니메이션 실행
		mAnimInst->Death();

		// 사망 처리 (이동 정리, 충돌 해제)
		ExecuteDeath();

		// 리스폰 함수 실행
		// RespawnCharacter 함수 3초 뒤	실행
		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ASagaPlayableCharacter::RespawnCharacter, 3.0f, false);

		// 상대 팀 점수 증가 실행
		system->AddScore(myTeam == EUserTeam::Red ? EUserTeam::Blue : EUserTeam::Red, 1);

		// arg1이 0이면 사람 캐릭터
		system->SendRpcPacket(ESagaRpcProtocol::RPC_DMG_PLYER, myHealth, 0);
	}
	else
	{
		//mAnimInst->Hit();

		if (not system->IsOfflineMode())
		{
			// arg1이 0이면 사람 캐릭터
			system->SendRpcPacket(ESagaRpcProtocol::RPC_DMG_PLYER, myHealth, 0);
		}
		else
		{

		}
	}

	return dmg;
}

void ASagaPlayableCharacter::DeactivateCascadeEffect(UParticleSystemComponent* ParticleComponent)
{
	if (ParticleComponent)
	{
		ParticleComponent->Deactivate();
	}
}


void
ASagaPlayableCharacter::ExecuteDeath()
{
	UE_LOG(LogSagaGame, Log, TEXT("[Character] ExecuteDeath"));

	Super::ExecuteDeath();
}

float
ASagaPlayableCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void
ASagaPlayableCharacter::RespawnCharacter()
{
	//
	Super::RespawnCharacter();

	// Animate
	mAnimInst->Revive();

	// Retrive collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// re-set collision profile name. NOT just Enable

	//GetCapsuleComponent()->SetCollisionProfileName(TEXT("Red"));
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

	if (myWeaponType == EPlayerWeapon::LightSabor)
	{
		FHitResult Result;

		// 플레이어 컨트롤러와 카메라를 가져옵니다.
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (!PlayerController) return;

		FVector WorldLocation, WorldDirection;
		PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

		// 공격 시작 및 종료 위치 계산
		FVector Start = GetActorLocation() + WorldDirection * 50.f;
		FVector End = Start + WorldDirection * 150.f;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		bool Collision = GetWorld()->SweepSingleByChannel(Result, Start, End, FQuat::Identity, ECC_GameTraceChannel4, FCollisionShape::MakeSphere(50.f), Params);

#if ENABLE_DRAW_DEBUG
		FColor Color = Collision ? FColor::Red : FColor::Green;
		DrawDebugCapsule(GetWorld(), (Start + End) / 2.f, 75.f, 25.f, FRotationMatrix::MakeFromZ(WorldDirection).ToQuat(), Color, false, 3.f);
#endif

		if (Collision)
		{
			FDamageEvent DamageEvent;
			Result.GetActor()->TakeDamage(30.f, DamageEvent, GetController(), this);
		}
	}
	else if (myWeaponType == EPlayerWeapon::WaterGun)
	{
		FHitResult Result;

		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (!PlayerController) return;

		FVector WorldLocation, WorldDirection;
		PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

		FVector TraceStart = GetActorLocation();
		FVector TraceEnd = TraceStart + WorldDirection * 1500.0f;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		bool Collision{};

		if (myTeam == EUserTeam::Red)
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Using Red Team Collision - WaterGun"));
			Collision = GetWorld()->LineTraceSingleByChannel(Result, TraceStart, TraceEnd, ECC_GameTraceChannel4, QueryParams);
		}
		else if (myTeam == EUserTeam::Blue)
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
	else if (myWeaponType == EPlayerWeapon::Hammer)
	{
		FHitResult Result;

		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (!PlayerController) return;

		FVector WorldLocation, WorldDirection;
		PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

		FVector Start = GetActorLocation() + WorldDirection * 50.f;
		FVector End = Start + WorldDirection * 150.f;

		FCollisionQueryParams param = FCollisionQueryParams::DefaultQueryParam;
		param.AddIgnoredActor(this);

		bool Collision;
		if (myTeam == EUserTeam::Red)
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Using Red Team Collision - Hammer"))
				Collision = GetWorld()->SweepSingleByChannel(Result, Start, End, FQuat::Identity, ECC_GameTraceChannel4, FCollisionShape::MakeSphere(50.f), param);
		}
		else if (myTeam == EUserTeam::Blue)
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Using Blue Team Collision - Hammer"))
				Collision = GetWorld()->SweepSingleByChannel(Result, Start, End, FQuat::Identity, ECC_GameTraceChannel7, FCollisionShape::MakeSphere(50.f), param);
		}
		else
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Not Found Team"));
			return;
		}

		if (Collision)
		{
			FDamageEvent DamageEvent;
			Result.GetActor()->TakeDamage(30.f, DamageEvent, GetController(), this);

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		}
	}
	else
	{
		UE_LOG(LogSagaGame, Warning, TEXT("Not Found Weapon"));
	}
}

void ASagaPlayableCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetMesh()->SetAnimInstanceClass(humanCharacterAnimation.LoadSynchronous());

	MyWeapon->SetCollisionProfileName(TEXT("Weapon"));
}

void
ASagaPlayableCharacter::Acquire_Drink(USagaWeaponData* ItemData)
{
	USagaWeaponData* AcquiredItemData = Cast<USagaWeaponData>(ItemData);
	if (AcquiredItemData)
	{
		//Send HP adding code or Item Acquiring code to server
		//And Save to Local Inventory
	}

	mItemType = AcquiredItemData->ItemType;
}

void
ASagaPlayableCharacter::Acquire_Gum(USagaWeaponData* ItemData)
{
	USagaWeaponData* AcquiredItemData = Cast<USagaWeaponData>(ItemData);
	if (AcquiredItemData)
	{
		//Item Acquiring code to server
		//And Save to Local Inventory
	}

	mItemType = AcquiredItemData->ItemType;
}

void
ASagaPlayableCharacter::Acquire_smokebomb(USagaWeaponData* ItemData)
{
	USagaWeaponData* AcquiredItemData = Cast<USagaWeaponData>(ItemData);
	if (AcquiredItemData)
	{
		//Item Acquiring code to server
		//And Save to Local Inventory
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
