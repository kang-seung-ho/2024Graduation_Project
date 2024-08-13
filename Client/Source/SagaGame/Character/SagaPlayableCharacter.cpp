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
#include "Character/SagaGummyBearSmall.h"
#include "Effect/SagaSwordEffect.h"
#include "UI/SagaWidgetComponent.h"
#include "UI/SagaHpBarWidget.h"

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

				net->SendRpcPacket(ESagaRpcProtocol::RPC_DMG_PLYER, arg0, arg1);
			}
			else
			{
				const auto ai_bear = Cast<ASagaGummyBearSmall>(causer);

				if (IsValid(ai_bear))
				{
					net->SendRpcPacket(ESagaRpcProtocol::RPC_DMG_PLYER, arg0, -2);

#if WITH_EDITOR

					UE_LOG(LogSagaGame, Log, TEXT("[ASagaPlayableCharacter] Attacked by a mini bear."));
#endif
				}
			}
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
	SetCollisionEnable(false);

	myHealthIndicatorBarWidget->SetHiddenInGame(true);
}

void
ASagaPlayableCharacter::TerminateGuardianAction()
{
	Super::TerminateGuardianAction();

	SetActorHiddenInGame(false);
	SetCollisionEnable(true);

	if (IsAlive())
	{
		myHealthIndicatorBarWidget->SetHiddenInGame(false);

		const auto healthbar = Cast<USagaHpBarWidget>(myHealthIndicatorBarWidget->GetWidget());

		if (IsValid(healthbar))
		{
#if WITH_EDITOR

			const auto ratio = myGameStat->GetCurrentHp() / myGameStat->GetMaxHp();
			UE_LOG(LogSagaGame, Log, TEXT("[ASagaPlayableCharacter][TerminateGuardianAction] Hp percentage: %f."), ratio);

			healthbar->UpdateHpBar(ratio);
#else

			healthbar->UpdateHpBar(myGameStat->GetCurrentHp() / myGameStat->GetMaxHp());

#endif

			myGameStat->RetryUpdateHealth();
		}
		else
		{
#if WITH_EDITOR

			UE_LOG(LogSagaGame, Error, TEXT("[ASagaPlayableCharacter][TerminateGuardianAction] The hp bar is null."));
#endif
		}
	}
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
#if WITH_EDITOR

		UE_LOG(LogSagaGame, Error, TEXT("[ASagaPlayableCharacter] mAnimInst is null."));
#endif
	}
}

void
ASagaPlayableCharacter::ExecuteAttack()
{
#if WITH_EDITOR

	const auto name = GetName();
#endif

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());
	if (net->GetLocalUserId() != GetUserId())
	{
#if WITH_EDITOR

		//UE_LOG(LogSagaGame, Warning, TEXT("[ASagaPlayableCharacter] '%s' is not local character. (net id: %d, entity id: %d)"), *name, net->GetLocalUserId(), GetUserId());
#endif

		// NOTICE: 이거 하면 데미지 처리 안됨
		//return;
	}

	const auto weapon = GetWeapon();

	FCollisionQueryParams query{};
	query.AddIgnoredActor(this);

	bool collide = false;
	FHitResult hit_result{};
	FDamageEvent hit_event{};
	float damage{};

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
#if WITH_EDITOR

			UE_LOG(LogSagaGame, Log, TEXT("[ASagaPlayableCharacter][Attack] Trace hit actor: %s"), *hit_result.GetActor()->GetName());
#endif
		}
		else
		{
#if WITH_EDITOR

			UE_LOG(LogSagaGame, Log, TEXT("[ASagaPlayableCharacter][Attack] '%s': No Actors were hit"), *name);
#endif
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
#if WITH_EDITOR

		UE_LOG(LogSagaGame, Error, TEXT("[ASagaPlayableCharacter][Attack] '%s' has no Weapon."), *name);
#endif

		return;
	}
	break;
	}

	if (collide)
	{
		const auto hit_actor = hit_result.GetActor();

		const auto bear = Cast<ASagaGummyBearPlayer>(hit_actor);

		if (IsValid(bear))
		{
			if (bear->IsAlive())
			{
				const auto Hitlocation = hit_result.ImpactPoint;
				const auto HitNormal = hit_result.Normal;

				const auto index = bear->OnBodyPartGetDamaged(Hitlocation, HitNormal);

				if (-1 != index and not net->IsOfflineMode())
				{
#if WITH_EDITOR

					const auto bear_name = bear->GetName();

					UE_LOG(LogSagaGame, Log, TEXT("[ASagaPlayableCharacter][Attack] A part %d of '%s' would be destructed."), index, *bear_name);
#endif

					net->SendRpcPacket(ESagaRpcProtocol::RPC_DMG_GUARDIANS_PART, 1 + index, bear->GetBearId());
				}

				hit_actor->TakeDamage(damage, hit_event, GetController(), this);
			}
		}
		else
		{
			hit_actor->TakeDamage(damage, hit_event, GetController(), this);
		}
	}
}

float
ASagaPlayableCharacter::ExecuteHurt(const float dmg)
{
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
	else if (dmg == 45.f) //if the player is attacked by skill(Q)
	{
		if (SkillQHitCascadeEffect)
		{
			UParticleSystemComponent* CascadeComponent = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				SkillQHitCascadeEffect,
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

	const auto world = GetWorld();
	const auto net = USagaNetworkSubSystem::GetSubSystem(world);
	const auto sys = USagaGameSubsystem::GetSubSystem(world);

	if (net->IsOfflineMode())
	{
		// 리스폰 함수 실행
		// ExecuteRespawn 함수 3초 뒤	실행
		GetWorldTimerManager().SetTimer(respawnTimerHandle, this, &ASagaPlayableCharacter::ExecuteRespawn, 3.0f, false);

		// 상대 팀 점수 증가
		sys->AddScore(GetTeam() == ESagaPlayerTeam::Red ? ESagaPlayerTeam::Blue : ESagaPlayerTeam::Red, 1);
	}
	else if (HasValidOwnerId())
	{
		if (GetUserId() == net->GetLocalUserId())
		{
			// NOTICE: 서버에서 RPC_DEAD 전송해주므로 하면 안됨
			//net->SendRpcPacket(ESagaRpcProtocol::RPC_DEAD, 0, GetUserId());

			GetWorldTimerManager().SetTimer(respawnTimerHandle, this, &ASagaPlayableCharacter::HandleRespawnCountdown, 0.5f, true);

			net->SendRpcPacket(ESagaRpcProtocol::RPC_GET_SCORE);
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
}

ASagaGummyBearPlayer* ASagaPlayableCharacter::GetNeareastCollidedBear()
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

void ASagaPlayableCharacter::UseSkill(int32 SlotNumber)
{
	mAnimInst->UseSkill(SlotNumber); //play animation first



	UE_LOG(LogSagaGame, Log, TEXT("[ASagaPlayableCharacter] UseSkill: %d"), SlotNumber);
}

void ASagaPlayableCharacter::ExecuteSkill(int32 SlotNumber)
{
	UE_LOG(LogTemp, Log, TEXT("[ASagaPlayableCharacter] ExecuteSkill: %d"), SlotNumber);
	const auto name = GetName();
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());
	if (net->GetLocalUserId() != GetUserId())
	{
	}

	FHitResult hit_result{};
	float damage{};
	FCollisionQueryParams query{};
	query.AddIgnoredActor(this);
	ECollisionChannel channel;
	if (GetTeam() == ESagaPlayerTeam::Red)
	{
		channel = ECC_GameTraceChannel4;
	}
	else // blue team
	{
		channel = ECC_GameTraceChannel7;
	}
	if (SlotNumber == 0) { // LightSabor's Q Skill
		FVector Start = GetActorLocation() + GetActorForwardVector() * 50.f;
		FVector End = Start + GetActorForwardVector() * 150.f;
		FDamageEvent hit_event{};
		bool collide = GetWorld()->SweepSingleByChannel(hit_result, Start, End, FQuat::Identity, channel, FCollisionShape::MakeSphere(50.f), query);


		damage = 45.f;

		if (collide)
		{
			const auto hit_actor = hit_result.GetActor();

			const auto bear = Cast<ASagaGummyBearPlayer>(hit_actor);

			if (IsValid(bear))
			{
				if (bear->IsAlive())
				{
					const auto Hitlocation = hit_result.ImpactPoint;
					const auto HitNormal = hit_result.Normal;

					const auto index = bear->OnBodyPartGetDamaged(Hitlocation, HitNormal);

					if (-1 != index and not net->IsOfflineMode())
					{
#if WITH_EDITOR

						const auto bear_name = bear->GetName();

						UE_LOG(LogSagaGame, Log, TEXT("[ASagaPlayableCharacter][Attack] A part %d of '%s' would be destructed."), index, *bear_name);
#endif

						net->SendRpcPacket(ESagaRpcProtocol::RPC_DMG_GUARDIANS_PART, 1 + index, bear->GetBearId());
					}

					hit_actor->TakeDamage(damage, hit_event, GetController(), this);
				}
			}
			else
			{
				hit_actor->TakeDamage(damage, hit_event, GetController(), this);
			}
		}
	}
	else if (SlotNumber == 1) // WaterGun's Q Skill
	{
		bool collide = false;
		FDamageEvent hit_event{};

		FVector TraceStart = GetActorLocation();
		FVector TraceEnd = GetActorLocation() + GetActorForwardVector() * 1500.0f;
		collide = GetWorld()->LineTraceSingleByChannel(hit_result, TraceStart, TraceEnd, channel, query);

		if (hit_result.bBlockingHit && IsValid(hit_result.GetActor()))
		{
#if WITH_EDITOR

			UE_LOG(LogTemp, Log, TEXT("[ASagaPlayableCharacter][Attack] Trace hit actor: %s"), *hit_result.GetActor()->GetName());
#endif
		}
		else
		{
#if WITH_EDITOR

			UE_LOG(LogTemp, Log, TEXT("[ASagaPlayableCharacter][Attack] '%s': No Actors were hit"), *name);
#endif
		}

		damage = 20.f;

		if (collide)
		{
			const auto hit_actor = hit_result.GetActor();

			const auto bear = Cast<ASagaGummyBearPlayer>(hit_actor);

			if (IsValid(bear))
			{
				if (bear->IsAlive())
				{
					const auto Hitlocation = hit_result.ImpactPoint;
					const auto HitNormal = hit_result.Normal;

					const auto index = bear->OnBodyPartGetDamaged(Hitlocation, HitNormal);

					if (-1 != index and not net->IsOfflineMode())
					{
#if WITH_EDITOR

						const auto bear_name = bear->GetName();

						UE_LOG(LogSagaGame, Log, TEXT("[ASagaPlayableCharacter][Attack] A part %d of '%s' would be destructed."), index, *bear_name);
#endif

						net->SendRpcPacket(ESagaRpcProtocol::RPC_DMG_GUARDIANS_PART, 1 + index, bear->GetBearId());
					}

					hit_actor->TakeDamage(damage, hit_event, GetController(), this);
				}
			}
			else
			{
				hit_actor->TakeDamage(damage, hit_event, GetController(), this);
			}
		}
	}
	else if (SlotNumber == 2) // Hammer's Q Skill
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Hammer's Q Skill"));
		FVector Start = GetActorLocation() + GetActorForwardVector() * 50.f;
		FVector End = Start + GetActorForwardVector() * 150.f;
		FDamageEvent hit_event{};
		bool collide = GetWorld()->SweepSingleByChannel(hit_result, Start, End, FQuat::Identity, channel, FCollisionShape::MakeSphere(50.f), query);


		damage = 45.f;

		if (collide)
		{
			const auto hit_actor = hit_result.GetActor();

			const auto bear = Cast<ASagaGummyBearPlayer>(hit_actor);

			if (IsValid(bear))
			{
				if (bear->IsAlive())
				{
					const auto Hitlocation = hit_result.ImpactPoint;
					const auto HitNormal = hit_result.Normal;

					const auto index = bear->OnBodyPartGetDamaged(Hitlocation, HitNormal);

					if (-1 != index and not net->IsOfflineMode())
					{
#if WITH_EDITOR

						const auto bear_name = bear->GetName();

						UE_LOG(LogSagaGame, Log, TEXT("[ASagaPlayableCharacter][Attack] A part %d of '%s' would be destructed."), index, *bear_name);
#endif

						net->SendRpcPacket(ESagaRpcProtocol::RPC_DMG_GUARDIANS_PART, 1 + index, bear->GetBearId());
					}

					hit_actor->TakeDamage(damage, hit_event, GetController(), this);
				}
			}
			else
			{
				hit_actor->TakeDamage(damage, hit_event, GetController(), this);
			}
		}
	}
	else if (SlotNumber == 3) //LightSaber's E Skill
	{

	}
	else if(SlotNumber == 4) //WaterGun's E Skill
	{

	}
	else if(SlotNumber == 5) //Hammer's E Skill
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Hammer's E Skill"));
		FVector Start = GetActorLocation() + GetActorForwardVector() * 50.f;
		FVector End = Start + GetActorForwardVector() * 150.f;
		FDamageEvent hit_event{};
		bool collide = GetWorld()->SweepSingleByChannel(hit_result, Start, End, FQuat::Identity, channel, FCollisionShape::MakeSphere(50.f), query);


		damage = 20.f; //25.f

		if (collide)
		{
			const auto hit_actor = hit_result.GetActor();

			const auto bear = Cast<ASagaGummyBearPlayer>(hit_actor);

			if (IsValid(bear))
			{
				if (bear->IsAlive())
				{
					const auto Hitlocation = hit_result.ImpactPoint;
					const auto HitNormal = hit_result.Normal;

					const auto index = bear->OnBodyPartGetDamaged(Hitlocation, HitNormal);

					if (-1 != index and not net->IsOfflineMode())
					{
#if WITH_EDITOR

						const auto bear_name = bear->GetName();

						UE_LOG(LogSagaGame, Log, TEXT("[ASagaPlayableCharacter][Attack] A part %d of '%s' would be destructed."), index, *bear_name);
#endif

						net->SendRpcPacket(ESagaRpcProtocol::RPC_DMG_GUARDIANS_PART, 1 + index, bear->GetBearId());
					}

					hit_actor->TakeDamage(damage, hit_event, GetController(), this);
				}
			}
			else
			{
				hit_actor->TakeDamage(damage, hit_event, GetController(), this);
			}
		}
	}
	else if(SlotNumber == 6) //LightSaber's R Skill
	{

	}
	else if(SlotNumber == 7) //WaterGun's R Skill
	{

	}
	else if(SlotNumber == 8) //Hammer's R Skill
	{

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[ASagaPlayableCharacter] ExecuteSkill: Invalid SlotNumber"));
	}
	

}

void ASagaPlayableCharacter::HandleBeginCollision(AActor* self, AActor* other_actor)
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

void ASagaPlayableCharacter::HandleEndCollision(AActor* self, AActor* other_actor)
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

void ASagaPlayableCharacter::HandleRespawnCountdown()
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
	, HitCascadeEffect(), GunHitCascadeEffect(), SkillQHitCascadeEffect()
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

	static ConstructorHelpers::FObjectFinder<UParticleSystem> SkillCascadeEffectRef(TEXT("ParticleSystem'/Game/Hit_VFX/VFX/Plasma_Hit/P_Hit_Plasma_4.P_Hit_Plasma_4'"));
	if (SkillCascadeEffectRef.Succeeded())
	{
		SkillQHitCascadeEffect = SkillCascadeEffectRef.Object;
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

	static ConstructorHelpers::FObjectFinder<UStaticMesh> RedHatMesh(TEXT("/Script/Engine.StaticMesh'/Game/PlayerAssets/RedHat.RedHat'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BlueHatMesh(TEXT("/Script/Engine.StaticMesh'/Game/PlayerAssets/BlueHat.BlueHat'"));



}

void ASagaPlayableCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetMesh()->SetAnimInstanceClass(humanCharacterAnimation.LoadSynchronous());

	if (IsValid(myWeapon))
	{
		myWeapon->SetCollisionProfileName(TEXT("Weapon"));
	}

	UStaticMesh* RedHatMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Script/Engine.StaticMesh'/Game/PlayerAssets/RedHat.RedHat'"));
	UStaticMesh* BlueHatMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Script/Engine.StaticMesh'/Game/PlayerAssets/BlueHat.BlueHat'"));

	UStaticMeshComponent* HatComponent = NewObject<UStaticMeshComponent>(this);

	if (ownerData.myTeam == ESagaPlayerTeam::Red)
	{
		if (RedHatMesh)
		{
			HatComponent->SetStaticMesh(RedHatMesh);
		}
	}
	else
	{
		if (BlueHatMesh)
		{
			HatComponent->SetStaticMesh(BlueHatMesh);
		}
	}

	HatComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	HatComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	// Head 소켓에 부착
	HatComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("head_x_PartyHat_Socket"));

	HatComponent->RegisterComponent();
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
