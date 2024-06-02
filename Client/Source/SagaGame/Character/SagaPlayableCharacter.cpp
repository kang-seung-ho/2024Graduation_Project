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
	: Super()
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

	static ConstructorHelpers::FObjectFinder<UParticleSystem> CascadeEffect(TEXT("ParticleSystem'/Game/Hit_VFX/VFX/Hard_Hit/P_Hit_5.P_Hit_5'"));
	if (CascadeEffect.Succeeded())
	{
		HitCascadeEffect = CascadeEffect.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> GunCascadeEffect(TEXT("ParticleSystem'/Game/Hit_VFX/VFX/Hard_Hit/P_Hit_3.P_Hit_3'"));
	if (GunCascadeEffect.Succeeded())
	{
		GunHitCascadeEffect = GunCascadeEffect.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> HitSoundEffectObject(TEXT("SoundWave'/Game/PlayerAssets/Sounds/Damage.Damage'"));
	if (HitSoundEffectObject.Succeeded())
	{
		HitSoundEffect = HitSoundEffectObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> DeadSoundEffectObject(TEXT("SoundWave'/Game/PlayerAssets/Sounds/Death.Death'"));
	if (DeadSoundEffectObject.Succeeded())
	{
		DeadSoundEffect = DeadSoundEffectObject.Object;
	}
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
	Super::ExecuteHurt(dmg);

	FVector NiagaraSpawnLocation = GetActorLocation();
	FRotator NiagaraSpawnRotation = GetActorRotation();

	FVector EffectSpawnLocation = GetActorLocation();
	FRotator EffectSpawnRotation = GetActorRotation();

	if (dmg == 30.f)
	{
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
	}
	else if (dmg == 20.f)
	{
		if (GunHitCascadeEffect)
		{
			UParticleSystemComponent* CascadeComponent = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				GunHitCascadeEffect,
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
	}

	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (myHealth <= 0.0f)
	{
		// 사망 애니메이션 실행
		mAnimInst->Death();

		if (DeadSoundEffect)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DeadSoundEffect, GetActorLocation());
		}

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
		mAnimInst->Hit();

		if (HitSoundEffect)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSoundEffect, GetActorLocation());
		}

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

void ASagaPlayableCharacter::Attack()
{
	Super::Attack();

	// TODO: ASagaPlayableCharacter::Attack()
	// myTeam = EUserTeam::Red; // Code For Client Test

	if (myWeaponType == EPlayerWeapon::LightSabor)
	{
		FHitResult Result;

		// 캐릭터의 앞 방향을 기준으로 공격 시작 및 종료 위치 계산
		FVector Start = GetActorLocation() + GetActorForwardVector() * 50.f;
		FVector End = Start + GetActorForwardVector() * 150.f;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		bool Collision = false;
		if (myTeam == EUserTeam::Red)
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Using Red Team Collision - LightSabor"));

			Collision = GetWorld()->SweepSingleByChannel(Result, Start, End, FQuat::Identity, ECC_GameTraceChannel4, FCollisionShape::MakeSphere(50.f), Params);
		}
		else if (myTeam == EUserTeam::Blue)
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Using Blue Team Collision - LightSabor"));
			Collision = GetWorld()->SweepSingleByChannel(Result, Start, End, FQuat::Identity, ECC_GameTraceChannel7, FCollisionShape::MakeSphere(50.f), Params);
		}

		FVector Hitlocation = Result.ImpactPoint;
		FVector HitNormal = Result.Normal;

		if (Collision)
		{
			FDamageEvent DamageEvent;
			Result.GetActor()->TakeDamage(30.f, DamageEvent, GetController(), this);

			/*if (Result.GetActor()->IsA<ASagaGummyBearPlayer>())
			{
				Cast<ASagaGummyBearPlayer>(Result.GetActor())->TryDismemberment(Hitlocation, HitNormal);
			}*/
			}
		}
	else if (myWeaponType == EPlayerWeapon::WaterGun)
	{
		FHitResult Result;

		// 캐릭터의 앞 방향을 기준으로 공격 시작 및 종료 위치 계산
		FVector TraceStart = GetActorLocation();
		FVector TraceEnd = GetActorLocation() + GetActorForwardVector() * 1500.0f;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		bool Collision = false;

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

			FVector Hitlocation = Result.ImpactPoint;
			FVector HitNormal = Result.Normal;

			/*if (Result.GetActor()->IsA<ASagaGummyBearPlayer>())
			{
				Cast<ASagaGummyBearPlayer>(Result.GetActor())->TryDismemberment(Hitlocation, HitNormal);
			}*/
			}
		}
	else if (myWeaponType == EPlayerWeapon::Hammer)
	{
		FHitResult Result;

		// 캐릭터의 앞 방향을 기준으로 공격 시작 및 종료 위치 계산
		FVector Start = GetActorLocation() + GetActorForwardVector() * 50.f;
		FVector End = Start + GetActorForwardVector() * 150.f;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		bool Collision = false;
		if (myTeam == EUserTeam::Red)
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Using Red Team Collision - Hammer"));

			Collision = GetWorld()->SweepSingleByChannel(Result, Start, End, FQuat::Identity, ECC_GameTraceChannel4, FCollisionShape::MakeSphere(50.f), Params);
		}
		else if (myTeam == EUserTeam::Blue)
		{
			UE_LOG(LogSagaGame, Warning, TEXT("Using Blue Team Collision - Hammer"));

			Collision = GetWorld()->SweepSingleByChannel(Result, Start, End, FQuat::Identity, ECC_GameTraceChannel7, FCollisionShape::MakeSphere(50.f), Params);
		}

		if (Collision)
		{
			FDamageEvent DamageEvent;
			Result.GetActor()->TakeDamage(30.f, DamageEvent, GetController(), this);

			FVector Hitlocation = Result.ImpactPoint;
			FVector HitNormal = Result.Normal;

			/*if (Result.GetActor()->IsA<ASagaGummyBearPlayer>())
			{
				Cast<ASagaGummyBearPlayer>(Result.GetActor())->TryDismemberment(Hitlocation, HitNormal);
			}*/
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
