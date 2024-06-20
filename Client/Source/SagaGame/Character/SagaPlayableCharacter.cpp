#include "SagaPlayableCharacter.h"
#include <DrawDebugHelpers.h>
#include <Containers/Array.h>
#include <Misc/OutputDeviceNull.h>
#include <UObject/Object.h>
#include <Templates/Casts.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/PlayerController.h>
#include <Animation/AnimInstance.h>
#include <NiagaraFunctionLibrary.h>
#include <NiagaraComponent.h>
#include <Particles/ParticleSystem.h>
#include <Particles/ParticleSystemComponent.h>

#include "SagaGameSubsystem.h"
#include "Character/SagaGummyBearPlayer.h"
#include "Character/SagaPlayerAnimInstance.h"
#include "Effect/SagaSwordEffect.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

float
ASagaPlayableCharacter::TakeDamage(float dmg, FDamageEvent const& event, AController* instigator, AActor* causer)
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (net->IsOfflineMode() or not HasValidOwnerId())
	{
		return Super::TakeDamage(dmg, event, instigator, causer);
	}
	else
	{
		// 자기 자신의 피해량만 송신함
		if (GetUserId() == net->GetLocalUserId())
		{
			// 서버의 RPC_DMG_PLYER 처리 부분의 주석 참조
			// arg0: 플레이어가 준 피해량 (4바이트 부동소수점)
			int64 arg0{};
			// arg1: 플레이어에게 피해를 준 개체의 식별자
			//     예시: 플레이어의 ID, 수호자의 순번
			int32 arg1{ -1 };

			std::memcpy(&arg0, reinterpret_cast<const char*>(&dmg), 4);

			const auto other = Cast<ASagaCharacterBase>(causer);

			if (IsValid(other) and other->HasValidOwnerId())
			{
				arg1 = other->GetUserId();
			}

			net->SendRpcPacket(ESagaRpcProtocol::RPC_DMG_PLYER, arg0, arg1);
		}

		return dmg;
	}
}

void
ASagaPlayableCharacter::ExecuteGuardianAction(ASagaCharacterBase* target)
{
	Super::ExecuteGuardianAction(target);

	TranslateProperties(target);

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void
ASagaPlayableCharacter::TerminateGuardianAction()
{
	Super::TerminateGuardianAction();

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}

void
ASagaPlayableCharacter::ExecuteAttackAnimation()
{
	if (IsValid(mAnimInst))
	{
		mAnimInst->PlayAttackMontage();
	}
	else
	{
		UE_LOG(LogSagaGame, Error, TEXT("[ASagaPlayableCharacter] mAnimInst is null."));
	}
}

void
ASagaPlayableCharacter::ExecuteAttack()
{
	const auto name = GetName();
	const auto weapon = GetWeapon();

	float damage{};

	FCollisionQueryParams query{};
	query.AddIgnoredActor(this);

	bool collide = false;
	FHitResult hit_result{};
	FDamageEvent hit_event{};
	FVector Hitlocation, HitNormal;

	ECollisionChannel channel;
	if (GetTeam() == ESagaPlayerTeam::Red)
	{
		channel = ECC_GameTraceChannel4;
	}
	else // blue team
	{
		channel = ECC_GameTraceChannel7;
	}

	switch (weapon)
	{
	case EPlayerWeapon::LightSabor:
	{
		// 캐릭터의 앞 방향을 기준으로 공격 시작 및 종료 위치 계산
		FVector Start = GetActorLocation() + GetActorForwardVector() * 50.f;
		FVector End = Start + GetActorForwardVector() * 150.f;

		collide = GetWorld()->SweepSingleByChannel(hit_result, Start, End, FQuat::Identity, channel, FCollisionShape::MakeSphere(50.f), query);

		damage = 30.f;
	}
	break;

	case EPlayerWeapon::WaterGun:
	{
		// 캐릭터의 앞 방향을 기준으로 공격 시작 및 종료 위치 계산
		FVector TraceStart = GetActorLocation();
		FVector TraceEnd = GetActorLocation() + GetActorForwardVector() * 1500.0f;
		collide = GetWorld()->LineTraceSingleByChannel(hit_result, TraceStart, TraceEnd, channel, query);

		if (hit_result.bBlockingHit && IsValid(hit_result.GetActor()))
		{
			UE_LOG(LogSagaGame, Log, TEXT("[ASagaPlayableCharacter][Attack] Trace hit actor: %s"), *hit_result.GetActor()->GetName());
		}
		else
		{
			UE_LOG(LogSagaGame, Log, TEXT("[ASagaPlayableCharacter][Attack] '%s': No Actors were hit"), *name);
		}

		damage = 20.f;
	}
	break;

	case EPlayerWeapon::Hammer:
	{
		// 캐릭터의 앞 방향을 기준으로 공격 시작 및 종료 위치 계산
		FVector Start = GetActorLocation() + GetActorForwardVector() * 50.f;
		FVector End = Start + GetActorForwardVector() * 150.f;
		collide = GetWorld()->SweepSingleByChannel(hit_result, Start, End, FQuat::Identity, channel, FCollisionShape::MakeSphere(50.f), query);

		damage = 30.f;
	}
	break;

	default:
	{
		UE_LOG(LogSagaGame, Error, TEXT("[ASagaPlayableCharacter][Attack] '%s' has no Weapon."), *name);
		return;
	}
	break;
	}

	if (collide)
	{
		const auto hit_actor = hit_result.GetActor();

		hit_result.GetActor()->TakeDamage(damage, hit_event, GetController(), this);

		// TODO: 이 코드를 구미 베어의 TakeDamage로 옮겨야 함.
		if (hit_actor->IsA<ASagaGummyBearPlayer>())
		{
			UE_LOG(LogSagaGame, Log, TEXT("[ASagaPlayableCharacter][Attack] '%s' hits a gummy bear."), *name);

			Hitlocation = hit_result.ImpactPoint;
			HitNormal = hit_result.Normal;

			Cast<ASagaGummyBearPlayer>(hit_actor)->TryDismemberment(Hitlocation, HitNormal);
		}
	}
}

float
ASagaPlayableCharacter::ExecuteHurt(const float dmg)
{
	// TODO: ASagaPlayableCharacter의 ExecuteHurt 구조 갈아엎기
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (net->IsOfflineMode() or not HasValidOwnerId())
	{
	}

	const auto current_health = Super::ExecuteHurt(dmg);

	FVector NiagaraSpawnLocation = GetActorLocation();
	FRotator NiagaraSpawnRotation = GetActorRotation();

	FVector EffectSpawnLocation = GetActorLocation();
	FRotator EffectSpawnRotation = GetActorRotation();

	// TODO: 데미지 마다 효과를 달리하지 말고 맞은 무기 종류따라 처리를 해줘야 함 --> change each weapon's damage amount different by 20 30 40
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
				CascadeComponent->bAutoDestroy = true;
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
				CascadeComponent->bAutoDestroy = true;
			}
		}
	}

	if (0 < current_health)
	{
		// NOTICE: 여기서 RPC 또 보내면 안됨
		// 중복되서 데미지 여러번 처리됨

		mAnimInst->Hit();

		if (HitSoundEffect)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSoundEffect, GetActorLocation());
		}
	}

	return current_health;
}

void
ASagaPlayableCharacter::ExecuteDeath()
{
	// 사망 처리
	UE_LOG(LogSagaGame, Log, TEXT("[ASagaPlayableCharacter] ExecuteDeath"));

	Super::ExecuteDeath();

	// 사망 애니메이션 실행
	mAnimInst->Death();

	if (DeadSoundEffect)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeadSoundEffect, GetActorLocation());
	}

	if (HasValidOwnerId())
	{
		const auto world = GetWorld();
		const auto net = USagaNetworkSubSystem::GetSubSystem(world);
		const auto sys = USagaGameSubsystem::GetSubSystem(world);

		if (net->IsOfflineMode())
		{
			// 상대 팀 점수 증가 실행
			sys->AddScore(GetTeam() == ESagaPlayerTeam::Red ? ESagaPlayerTeam::Blue : ESagaPlayerTeam::Red, 1);

			// 리스폰 함수 실행
			// ExecuteRespawn 함수 3초 뒤	실행
			GetWorldTimerManager().SetTimer(respawnTimerHandle, this, &ASagaPlayableCharacter::ExecuteRespawn, 3.0f, false);
		}
		else if (GetUserId() == net->GetLocalUserId())
		{
			// NOTICE: 서버에서 RPC_DEAD 전송해주므로 하면 안됨
			//net->SendRpcPacket(ESagaRpcProtocol::RPC_DEAD, 0, GetUserId());

			sys->AddScore(GetTeam() == ESagaPlayerTeam::Red ? ESagaPlayerTeam::Blue : ESagaPlayerTeam::Red, 1);

			GetWorldTimerManager().SetTimer(respawnTimerHandle, this, &ASagaPlayableCharacter::HandleRespawnCountdown, 0.5f, true);
			//GetWorldTimerManager().SetTimer(respawnTimerHandle, this, &ASagaPlayableCharacter::ExecuteRespawn, 3.0f, false);
			//GetWorldTimerManager().SetTimer(respawnTimerHandle, this, &ASagaPlayableCharacter::ExecuteRespawnViaRpc, 3.0f, false);
		}
	}
}

void
ASagaPlayableCharacter::ExecuteRespawnViaRpc()
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not net->IsOfflineMode())
	{
		net->SendRpcPacket(ESagaRpcProtocol::RPC_RESPAWN, 0, GetUserId());
	}
}

void
ASagaPlayableCharacter::ExecuteRespawn()
{
	// 리스폰 타이머 해제
	GetWorldTimerManager().ClearTimer(respawnTimerHandle);

	Super::ExecuteRespawn();

	// Animate
	mAnimInst->Revive();

	// Retrive collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

ASagaGummyBearPlayer*
ASagaPlayableCharacter::GetNeareastCollidedBear()
const
{
	if (0 == collideBears.Num())
	{
		return nullptr;
	}

	if (1 == collideBears.Num())
	{
		if (collideBears[0]->IsAlive())
		{
			return collideBears[0];
		}
		else
		{
			return nullptr;
		}
	}

	const auto pos = GetActorLocation();
	double mindist{ INFINITY };
	ASagaGummyBearPlayer* result{};

	for (auto& bear : collideBears)
	{
		double dist = FVector::Distance(pos, bear->GetActorLocation());
		if (dist < mindist and bear->IsAlive())
		{
			result = bear;
			mindist = dist;
		}
	}

	return result;
}

void
ASagaPlayableCharacter::HandleBeginCollision(AActor* self, AActor* other_actor)
{
	const auto bear = Cast<ASagaGummyBearPlayer>(other_actor);

	if (IsValid(bear) and bear->IsAlive())
	{
#if WITH_EDITOR

		const auto name = GetName();
		const auto other_name = other_actor->GetName();
		UE_LOG(LogSagaGame, Log, TEXT("[HandleBeginCollision] '%s' is collide with bear '%s'."), *name, *other_name);
#endif

		collideBears.Add(bear);
	}
}

void
ASagaPlayableCharacter::HandleEndCollision(AActor* self, AActor* other_actor)
{
	const auto bear = Cast<ASagaGummyBearPlayer>(other_actor);

	if (IsValid(bear))
	{
#if WITH_EDITOR

		const auto name = GetName();
		const auto other_name = other_actor->GetName();
		UE_LOG(LogSagaGame, Log, TEXT("[HandleBeginCollision] '%s' would not be collide with bear '%s' anymore."), *name, *other_name);
#endif

		collideBears.RemoveSwap(bear);
	}
}

void
ASagaPlayableCharacter::HandleRespawnCountdown()
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not net->IsOfflineMode() and net->IsConnected())
	{
		net->SendRpcPacket(ESagaRpcProtocol::RPC_RESPAWN_TIMER);
	}
}

ASagaPlayableCharacter::ASagaPlayableCharacter()
	: Super()
	, humanCharacterAnimation()
	, collideBears()
	, HitCascadeEffect(), GunHitCascadeEffect()
	, HitSoundEffect(), DeadSoundEffect()
	, respawnTimerHandle()
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

	collideBears.Reserve(3);
}

void
ASagaPlayableCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetMesh()->SetAnimInstanceClass(humanCharacterAnimation.LoadSynchronous());

	if (IsValid(myWeapon))
	{
		myWeapon->SetCollisionProfileName(TEXT("Weapon"));
	}
}

void
ASagaPlayableCharacter::BeginPlay()
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not net->IsOfflineMode())
	{
		// NOTICE: 오프라인 모드일 때만 체력이 닳았을 때 ExecuteDeath를 실행함
		myGameStat->OnHpZero.AddDynamic(this, &ASagaPlayableCharacter::ExecuteDeath);
	}

	OnActorBeginOverlap.AddDynamic(this, &ASagaPlayableCharacter::HandleBeginCollision);
	OnActorEndOverlap.AddDynamic(this, &ASagaPlayableCharacter::HandleEndCollision);

	Super::BeginPlay();
}
