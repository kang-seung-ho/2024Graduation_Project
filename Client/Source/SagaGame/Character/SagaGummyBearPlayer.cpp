#include "SagaGummyBearPlayer.h"
#include <Templates/Casts.h>
#include <Engine/TimerHandle.h>
#include <Components/ArrowComponent.h>
#include <GeometryCollection/ManagedArray.h>
#include <GeometryCollection/GeometryCollectionComponent.h>
#include <GeometryCollection/GeometryCollectionObject.h>
#include <GeometryCollection/GeometryCollection.h>
#include <GeometryCollection/GeometryCollectionAlgo.h>
#include <NiagaraComponent.h>
#include <NiagaraFunctionLibrary.h>

#include "SagaGameSubsystem.h"
#include "Player/SagaPlayerTeam.h"
#include "Character/SagaPlayableCharacter.h"
#include "Character/SagaGummyBearAnimInstance.h"
#include "Character/SagaGummyBearSmall.h"
#include "Effect/SagaSwordEffect.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

float
ASagaGummyBearPlayer::TakeDamage(float dmg, FDamageEvent const& event, AController* instigator, AActor* causer)
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (net->IsOfflineMode())
	{
#if WITH_EDITOR

		const auto name = GetName();
		UE_LOG(LogSagaGame, Log, TEXT("[TakeDamage] Guardian '%s' would take %f damages. (Offline Mode)"), *name, dmg);
#endif

		return Super::TakeDamage(dmg, event, instigator, causer);
	}
	else if (IsAlive())
	{
#if WITH_EDITOR

		UE_LOG(LogSagaGame, Log, TEXT("[TakeDamage] Handling gummy bear's damage by %f."), dmg);
#endif

		// 서버의 RPC_DMG_PLYER 처리 부분의 주석 참조
		// arg0: 플레이어가 준 피해량 (4바이트 부동소수점) | 파괴 부위 (4바이트)
		int64 arg0{};
		// arg1: 현재 수호자의 식별자
		int32 arg1{ GetBearId() };

		std::memcpy(&arg0, reinterpret_cast<const char*>(&dmg), 4);

		net->SendRpcPacket(ESagaRpcProtocol::RPC_DMG_GUARDIAN, arg0, arg1);
	}

	return dmg;
}

void
ASagaGummyBearPlayer::ExecuteGuardianAction(ASagaCharacterBase* target)
{
	Super::ExecuteGuardianAction(target);

	if (target->GetTeam() == ESagaPlayerTeam::Red)
	{
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("Red"));
	}
	else if (target->GetTeam() == ESagaPlayerTeam::Blue)
	{
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("Blue"));
	}
	else
	{
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("Enemy"));
	}

	ownerData.SetCharacterHandle(target);
}

void
ASagaGummyBearPlayer::TerminateGuardianAction()
{
	Super::TerminateGuardianAction();

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Enemy"));

	const auto character = ownerData.GetCharacterHandle();

	if (IsValid(character))
	{
		const auto loc = playerUnridePosition->GetComponentLocation();
		const auto rot = playerUnridePosition->GetComponentRotation();
		character->SetActorLocationAndRotation(loc, rot);

		ownerData = {};
	}
	else
	{
#if WITH_EDITOR

		UE_LOG(LogSagaGame, Error, TEXT("[TerminateGuardianAction] Has no character handle!"));
#endif
	}
}

void
ASagaGummyBearPlayer::ExecuteAttackAnimation()
{
	if (IsValid(mBearAnimInst))
	{
		mBearAnimInst->PlayAttackMontage();
	}
	else
	{
#if WITH_EDITOR

		UE_LOG(LogSagaGame, Error, TEXT("[ASagaGummyBearPlayer] mBearAnimInst is null."));
#endif
	}
}

void
ASagaGummyBearPlayer::ExecuteAttack()
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());
	if (net->GetLocalUserId() != GetUserId()) return;

	//공격과 충돌되는 물체 여부 판단
	FHitResult hit_result{};

	FVector Start = GetActorLocation() + GetActorForwardVector() * 50.f;
	FVector End = Start + GetActorForwardVector() * 150.f;

	FCollisionQueryParams param = FCollisionQueryParams::DefaultQueryParam;
	param.AddIgnoredActor(this);

#if WITH_EDITOR

	const auto name = GetName();
#endif

	///NOTICE: ECC_GameTraceChannel2: Enemy
	///NOTICE: ECC_GameTraceChannel4: red
	///NOTICE: ECC_GameTraceChannel7: blue
	ECollisionChannel channel = ECC_GameTraceChannel2;

	if (GetTeam() == ESagaPlayerTeam::Red)
	{
		channel = ECC_GameTraceChannel4;
	}
	else if (GetTeam() == ESagaPlayerTeam::Blue)
	{
		channel = ECC_GameTraceChannel7;
	}
	else
	{
#if WITH_EDITOR

		UE_LOG(LogSagaGame, Error, TEXT("[ASagaGummyBearPlayer] '%s' has invalid team."), *name);
#endif
	}

	const bool collide = GetWorld()->SweepSingleByChannel(hit_result, Start, End, FQuat::Identity, channel, FCollisionShape::MakeSphere(50.f), param);

	//#if ENABLE_DRAW_DEBUG
	//	//충돌시 빨강 아니면 녹색
	//	FColor Color = collide ? FColor::Red : FColor::Green;
	//
	//	DrawDebugCapsule(GetWorld(), (Start + End) / 2.f, 150.f / 2.f + 50.f / 2.f, 50.f, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), Color, false, 3.f);
	//#endif

	constexpr float bearDamage = 50.0f;

	if (collide)
	{
		const auto hit_actor = hit_result.GetActor();

		FDamageEvent event{};
		FActorSpawnParameters params{};
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		const auto character = Cast<ASagaCharacterBase>(hit_actor);

		if (IsValid(character))
		{
			hit_actor->TakeDamage(bearDamage, event, GetController(), this);

			const auto hit_effect = GetWorld()->SpawnActor<ASagaSwordEffect>(hit_result.ImpactPoint, hit_result.ImpactNormal.Rotation(), params);

			// 이곳에 레퍼런스 복사
			//hit_effect->SetParticle(TEXT(""));
			//hit_effect->SetSound(TEXT(""));
		}
		else
		{
			const auto ai_pawn = Cast<ASagaGummyBearSmall>(hit_actor);

			if (IsValid(ai_pawn))
			{
				hit_actor->TakeDamage(bearDamage, event, GetController(), this);

				const auto hit_effect = GetWorld()->SpawnActor<ASagaSwordEffect>(hit_result.ImpactPoint, hit_result.ImpactNormal.Rotation(), params);

				// 이곳에 레퍼런스 복사
				//hit_effect->SetParticle(TEXT(""));
				//hit_effect->SetSound(TEXT(""));
			}
			else
			{
				// 아이템 혹은 다른 액터 공격
				hit_actor->TakeDamage(bearDamage, event, GetController(), this);

#if WITH_EDITOR

				UE_LOG(LogSagaGame, Log, TEXT("[ASagaGummyBearPlayer] '%s' attacked other actors."), *name);
#endif
			}
		}
	}
}

float
ASagaGummyBearPlayer::ExecuteHurt(const float dmg)
{
	const auto current_hp = Super::ExecuteHurt(dmg);

#if WITH_EDITOR

	UE_LOG(LogSagaGame, Log, TEXT("[ASagaGummyBearPlayer] ExecuteHurt (dmg: %f, hp : %f)"), dmg, current_hp);
#endif

	// 
	if (HitEffect)
	{
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = GetActorRotation();
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitEffect, SpawnLocation, SpawnRotation);

		if (NiagaraComponent)
		{
			FTimerHandle NiagaraTimerHandle{};

			GetWorldTimerManager().SetTimer(NiagaraTimerHandle
				, [NiagaraComponent]()
				{
					NiagaraComponent->Deactivate();
					NiagaraComponent->DestroyComponent();
				}, 3.0f, false
			);
		}
	}

	return current_hp;
}

void
ASagaGummyBearPlayer::ExecuteDeath()
{
	// 사망 처리
	UE_LOG(LogSagaGame, Log, TEXT("[ASagaGummyBearPlayer] ExecuteDeath"));

	// 사망 애니메이션 실행
	mBearAnimInst->Death();

	const auto world = GetWorld();
	const auto net = USagaNetworkSubSystem::GetSubSystem(world);
	const auto sys = USagaGameSubsystem::GetSubSystem(world);

	if (net->IsOfflineMode())
	{
		Super::ExecuteDeath();

		// 상대 팀 점수 증가 실행
		sys->AddScore(GetTeam() == ESagaPlayerTeam::Red ? ESagaPlayerTeam::Blue : ESagaPlayerTeam::Red, 1);
	}
	else
	{
		Super::ExecuteDeath();

		// TODO: 점수 동기화 작업 중
		net->SendRpcPacket(ESagaRpcProtocol::RPC_GET_SCORE);
	}
}

float
ASagaGummyBearPlayer::OnBodyPartGetDamaged(FVector Location, FVector Normal)
{
	if (IsAlive())
	{
		for (int32 i = 0; i < chaosParts.Num(); i++)
		{
			if (IsPointInsideBox(chaosParts[i], Location))
			{
				if (0 == --chaosPartHealthPoints[i])
				{
					FVector Impulse = -Normal * 250.0f; // Example impulse calculation
					CheckValidBone(Impulse, i);

					break;
				}
			}

			UE_LOG(LogSagaGame, Warning, TEXT("[ASagaGummyBearPlayer] HitBox: %d, hp: %d"), i, chaosPartHealthPoints[i]);
		}

		if (chaosPartHealthPoints[1] <= 0 && chaosPartHealthPoints[3] <= 0)
		{
			return 9999;
		}
		else
		{
			return 0;
		}
	}

	return 0;
}


void
ASagaGummyBearPlayer::CheckValidBone(const FVector& Impulse, int32 Index)
{
	USkinnedMeshComponent* SkinnedMesh = FindComponentByClass<USkinnedMeshComponent>();
	UBoxComponent* boxes = chaosParts[Index];
	FName BoneName = boxes->GetAttachSocketName();

	if (!SkinnedMesh->IsBoneHiddenByName(BoneName))
	{
		UE_LOG(LogSagaGame, Warning, TEXT("TriggerExplosion: %d"), Index);
		ExplodeBodyParts(BoneName, Impulse, Index);
	}
}

void
ASagaGummyBearPlayer::ExplodeBodyParts(FName BoneName, const FVector& Impulse, int32 Index)
{
	USkinnedMeshComponent* SkinnedMesh = FindComponentByClass<USkinnedMeshComponent>();
	SkinnedMesh->HideBoneByName(BoneName, PBO_None);
	UGeometryCollectionComponent* TargetGC = geometryCollections[Index];

	TargetGC->SetVisibility(true);
	TargetGC->SetSimulatePhysics(true);
	TargetGC->CrumbleActiveClusters();


	TargetGC->AddRadialImpulse(TargetGC->GetComponentLocation(), 100, 150, RIF_Constant, true);
	TargetGC->AddImpulse(Impulse, NAME_None, true);

	//DismCollisionBox[Index]->DestroyComponent();
	//SpawnMorphSystem(TargetGC, Index);
	//*/

	if (UFunction* Function = FindFunction(TEXT("TriggerMorphEvent")))
	{
		struct Params
		{
			UGeometryCollectionComponent* Target;
			int32 Ind;
		};

		Params Param;
		Param.Target = TargetGC;
		Param.Ind = Index;

		ProcessEvent(Function, &Param);
	}

}

FTransform
ASagaGummyBearPlayer::SpawnMorphSystem(UGeometryCollectionComponent* TargetGC, int32 Index)
{
	TargetGC->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	int32 PieceIndex = 0;

	switch (Index)
	{
	case 0:
		PieceIndex = 13;
		break;
	case 1:
		PieceIndex = 10;
		break;
	case 2:
		PieceIndex = 6;
		break;
	case 3:
		PieceIndex = 4;
		break;
	}

	FVector Location;
	FQuat Rotation;

	auto& transforms = TargetGC->GetTransformArray();
	auto& gc_transform = TargetGC->GetComponentTransform();

	if (transforms.IsValidIndex(PieceIndex))
	{
		FTransform WorldTransform = transforms[PieceIndex] * gc_transform;

		Location = WorldTransform.GetLocation();
		Rotation = WorldTransform.GetRotation();
	}
	else
	{
		UE_LOG(LogSagaGame, Fatal, TEXT("[SpawnMorphSystem] There is no transform of the piece %d."), Index);
	}

	if (NiagaraSystemTemplate)
	{
		FHitResult hitresult{};
		FVector Starttrace = Location + FVector(0, 0, 100);
		FVector Endtrace = Location + FVector(0, 0, -1000);

		const auto world = GetWorld();

		if (world->LineTraceSingleByChannel(hitresult, Starttrace, Endtrace, ECollisionChannel::ECC_Visibility))
		{
			NiagaraComponentTemplate->SetAsset(NiagaraSystemTemplate);
			Location = hitresult.Location + FVector(0, 0, 45.0f);

			UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				world,
				NiagaraSystemTemplate,
				Location,
				FRotator::ZeroRotator,
				FVector(1.f)
			);

			UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMesh(NiagaraComponent, TEXT("User.SourceMesh"), nullptr);
			UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMesh(NiagaraComponent, TEXT("User.SourceMesh"), GetTargetMesh(Index));

			NiagaraComponent->SetVariableQuat(TEXT("InitRotator"), Rotation);
			NiagaraComponent->ActivateSystem(true);

			HideBodyPartPiece(TargetGC, PieceIndex);

			FTransform output{};
			output.SetLocation(Location);
			output.SetRotation(FQuat::Identity);

			return output;
		}

	}

	return FTransform::Identity;
}

void
ASagaGummyBearPlayer::InitTargetMeshes()
{
	myChaosPartMeshes.Reserve(chaosPartsNumber);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh1Asset(TEXT("StaticMesh'/Game/NPCAssets/meshes/rarm_13.rarm_13'"));
	if (Mesh1Asset.Succeeded())
	{
		myChaosPartMeshes.Add(Mesh1Asset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh2Asset(TEXT("StaticMesh'/Game/NPCAssets/meshes/rleg_10.rleg_10'"));
	if (Mesh2Asset.Succeeded())
	{
		myChaosPartMeshes.Add(Mesh2Asset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh3Asset(TEXT("StaticMesh'/Game/NPCAssets/meshes/larm_6.larm_6'"));
	if (Mesh3Asset.Succeeded())
	{
		myChaosPartMeshes.Add(Mesh3Asset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh4Asset(TEXT("StaticMesh'/Game/NPCAssets/meshes/lleg_4.lleg_4'"));
	if (Mesh3Asset.Succeeded())
	{
		myChaosPartMeshes.Add(Mesh4Asset.Object);
	}

	//UE_LOG(LogSagaGame, Warning, TEXT("targetmesheslen: %d"), myChaosPartMeshes.Num());
}

UStaticMesh*
ASagaGummyBearPlayer::GetTargetMesh(int32 Index)
const
{
	return myChaosPartMeshes[Index];
}

int32
ASagaGummyBearPlayer::GetBearId()
const noexcept
{
	return bearUniqueId;
}

ASagaGummyBearPlayer::ASagaGummyBearPlayer()
	: Super()
	, bearUniqueId(0)
	, playerUnridePosition()
	, myInteractionBox()
{
	myGameStat->SetMaxHp(500.0f);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/NPCAssets/Modeling/Bear.Bear'"));
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}

	GetCapsuleComponent()->SetCapsuleSize(90.0f, 96.0f);
	SetRootComponent(GetCapsuleComponent());
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Enemy"));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetRelativeLocation(FVector(0.0, 0.0, -88.0));
	GetMesh()->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimAsset(TEXT("AnimBlueprint'/Game/NPCAssets/Animation/AB_GummyBear.AB_GummyBear_C'"));
	if (AnimAsset.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimAsset.Class);
	}

	playerUnridePosition = CreateDefaultSubobject<UArrowComponent>(TEXT("PlayerUnridePosition"));
	playerUnridePosition->SetupAttachment(RootComponent);
	playerUnridePosition->SetRelativeLocation(FVector(-170, 0, 110.0));
	playerUnridePosition->SetRelativeRotation(FRotator(0, 0, 0));

	myCameraSpringArmComponent->SetRelativeLocation(FVector(0.0, 0.0, 250.0));
	myCameraSpringArmComponent->SetRelativeRotation(FRotator(-30.0, 90.0, 0.0));
	myCameraSpringArmComponent->TargetArmLength = 400.f;

	myInteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	myInteractionBox->SetupAttachment(RootComponent);
	myInteractionBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	myInteractionBox->SetWorldScale3D(FVector(4.5f, 5.75f, 2.25f));
	myInteractionBox->SetRelativeLocation(FVector(70.0f, 0.0f, 40.0f));

	//*/
	chaosPartRightArm = CreateDefaultSubobject<UBoxComponent>(TEXT("chaosPartRightArm"));
	chaosPartRightArm->SetupAttachment(GetMesh(), TEXT("arm_stretch_r"));
	InitializeTransform(chaosPartRightArm, FVector(-50, -65, -7), FRotator(6, -50, 82), FVector(1.25, 1.25, 2.25));
	chaosPartRightArm->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	chaosParts.Add(chaosPartRightArm);

	chaosPartRightLeg = CreateDefaultSubobject<UBoxComponent>(TEXT("chaosPartRightLeg"));
	chaosPartRightLeg->SetupAttachment(GetMesh(), TEXT("thigh_stretch_r"));
	InitializeTransform(chaosPartRightLeg, FVector(2, -25, 10), FRotator(-30, 57, 72), FVector(1.5, 1.5, 2.0));
	chaosPartRightLeg->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	chaosParts.Add(chaosPartRightLeg);

	chaosPartLeftArm = CreateDefaultSubobject<UBoxComponent>(TEXT("chaosPartLeftArm"));
	chaosPartLeftArm->SetupAttachment(GetMesh(), TEXT("arm_stretch_l"));
	InitializeTransform(chaosPartLeftArm, FVector(40, -80, -15), FRotator(6, 40, 76), FVector(1.25, 1.25, 2.0));
	chaosPartLeftArm->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	chaosParts.Add(chaosPartLeftArm);

	chaosPartLeftLeg = CreateDefaultSubobject<UBoxComponent>(TEXT("chaosPartLeftLeg"));
	chaosPartLeftLeg->SetupAttachment(GetMesh(), TEXT("thigh_stretch_l"));
	InitializeTransform(chaosPartLeftLeg, FVector(-8, -15, 8), FRotator(-50, 100, -45), FVector(1.25, 1.25, 2.0));
	chaosPartLeftLeg->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	chaosParts.Add(chaosPartLeftLeg);

	//*/
	geometryRightArm = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("geometryRightArm"));
	geometryRightArm->SetupAttachment(chaosPartRightArm);
	static ConstructorHelpers::FObjectFinder<UGeometryCollection> r_armAsset(TEXT("GeometryCollection'/Game/NPCAssets/meshes/GC_r_arm.GC_r_arm'"));
	if (r_armAsset.Succeeded())
	{
		geometryRightArm->RestCollection = r_armAsset.Object;
	}
	InitializeTransform(geometryRightArm, FVector(2, 2, 6), FRotator(80, 68, -50), FVector(0.8, 0.8, 0.44));
	geometryCollections.Add(geometryRightArm);

	geometryRightLeg = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("geometryRightLeg"));
	geometryRightLeg->SetupAttachment(chaosPartRightLeg);
	static ConstructorHelpers::FObjectFinder<UGeometryCollection> r_legAsset(TEXT("GeometryCollection'/Game/NPCAssets/meshes/GC_r_leg.GC_r_leg'"));
	if (r_legAsset.Succeeded())
	{
		geometryRightLeg->RestCollection = r_legAsset.Object;
	}
	InitializeTransform(geometryRightLeg, FVector(3.8, -3, -10), FRotator(-5, 41, 0), FVector(0.66, 0.66, 0.5));
	geometryCollections.Add(geometryRightLeg);

	geometryLeftArm = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("geometryLeftArm"));
	geometryLeftArm->SetupAttachment(chaosPartLeftArm);
	static ConstructorHelpers::FObjectFinder<UGeometryCollection> l_armAsset(TEXT("GeometryCollection'/Game/NPCAssets/meshes/GC_l_arm.GC_l_arm'"));
	if (l_armAsset.Succeeded())
	{
		geometryLeftArm->RestCollection = l_armAsset.Object;
	}
	InitializeTransform(geometryLeftArm, FVector(1, 3, 8), FRotator(-82, -141, 54), FVector(0.8, 0.8, 0.5));
	geometryCollections.Add(geometryLeftArm);

	geometryLeftLeg = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("geometryLeftLeg"));
	geometryLeftLeg->SetupAttachment(chaosPartLeftLeg);

	static ConstructorHelpers::FObjectFinder<UGeometryCollection> l_legAsset(TEXT("GeometryCollection'/Game/NPCAssets/meshes/GC_l_leg.GC_l_leg'"));
	if (l_legAsset.Succeeded())
	{
		geometryLeftLeg->RestCollection = l_legAsset.Object;
	}

	InitializeTransform(geometryLeftLeg, FVector(0, 2, -12), FRotator(2, 154, 2), FVector(0.8, 0.66, 0.5));
	geometryCollections.Add(geometryLeftLeg);

	for (int i = 0; i < geometryCollections.Num(); i++)
	{
		geometryCollections[i]->SetVisibility(false);
		geometryCollections[i]->SetSimulatePhysics(false);
		geometryCollections[i]->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}

	NiagaraComponentTemplate = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MorphNiagaraComponent"));

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NSAsset{ TEXT("NiagaraSystem'/Game/NPCAssets/NS_MeshMorph.NS_MeshMorph'") };
	if (NSAsset.Succeeded())
	{
		NiagaraSystemTemplate = NSAsset.Object;
	}

	NiagaraComponentTemplate->SetupAttachment(RootComponent);
	NiagaraComponentTemplate->SetAutoActivate(false);

	// 오버랩 이벤트 바인드
	//myInteractionBox->OnComponentBeginOverlap.AddDynamic(this, &ASagaGummyBearPlayer::OnOverlapBegin);
	//myInteractionBox->OnComponentEndOverlap.AddDynamic(this, &ASagaGummyBearPlayer::OnOverlapEnd);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraEffect{ TEXT("/Script/{Niagara.NiagaraSystem'/Game/VFX/VFX_Hit/NS_Hit.NS_Hit'") };

	if (NiagaraEffect.Succeeded())
	{
		HitEffect = NiagaraEffect.Object;
		UE_LOG(LogSagaGame, Warning, TEXT("Bear Niagara Hit Effect Loaded"));
	}
	else
	{
		UE_LOG(LogSagaGame, Error, TEXT("Bear Item Niagara Hit Effect Not Loaded"));
	}

	InitTargetMeshes();
}

void
ASagaGummyBearPlayer::BeginPlay()
{
	chaosPartHealthPoints.Init(defaultChaosPartDestructiveHealthPoint, chaosPartsNumber);

	myGameStat->OnHpZero.AddUniqueDynamic(this, &ASagaGummyBearPlayer::ExecuteDeath);

	Super::BeginPlay();
}

void
ASagaGummyBearPlayer::InitializeTransform(USceneComponent* const component, const FVector& loc, const FRotator& rot, const FVector& scale)
{
	component->SetRelativeLocation(loc);
	component->SetRelativeRotation(rot);
	component->SetRelativeScale3D(scale);
}

bool
ASagaGummyBearPlayer::IsPointInsideBox(const UBoxComponent* const Box, const FVector& Point)
{
	if (!Box) return false;

	FVector BoxOrigin = Box->GetComponentLocation();
	FVector BoxExtent = Box->GetScaledBoxExtent();
	FRotator BoxRotation = Box->GetComponentRotation();
	FTransform BoxTransform(BoxRotation, BoxOrigin);

	FVector LocalPoint = BoxTransform.InverseTransformPosition(Point);

	return FMath::Abs(LocalPoint.X) <= BoxExtent.X &&
		FMath::Abs(LocalPoint.Y) <= BoxExtent.Y &&
		FMath::Abs(LocalPoint.Z) <= BoxExtent.Z;
}

void
ASagaGummyBearPlayer::HideBodyPartPiece(UGeometryCollectionComponent* const TargetGC, int32 PieceIndex)
{
	TargetGC->SetVisibility(false, true);
}

UGeometryCollectionComponent*
ASagaGummyBearPlayer::FindGeometryCollectionByName(const AActor* const actor, const FString& name)
{
	auto& owner = actor->Owner;

	if (IsValid(owner))
	{
		TArray<UActorComponent*> Components;
		owner->GetComponents(Components);

		for (const auto Component : Components)
		{
			const auto GeometryCollectionComponent = Cast<UGeometryCollectionComponent>(Component);

			if (GeometryCollectionComponent && GeometryCollectionComponent->GetName() == name)
			{
				return GeometryCollectionComponent;
			}
		}
	}

	return nullptr;
}

FVector
ASagaGummyBearPlayer::GetPieceWorldLocation(const UGeometryCollectionComponent* const target, int32 index)
{
	if (target)
	{
		const TManagedArray<FTransform>& Transforms = target->GetTransformArray();

		if (Transforms.IsValidIndex(index))
		{
			FTransform WorldTransform = Transforms[index] * target->GetComponentTransform();

			return WorldTransform.GetLocation();
		}
	}

	return FVector::ZeroVector;
}

FQuat
ASagaGummyBearPlayer::GetPieceWorldRotation(const UGeometryCollectionComponent* const target, int32 index)
{
	if (target)
	{
		const UGeometryCollection* GeometryCollection = target->GetRestCollection();
		FGeometryCollection* Collection = GeometryCollection->GetGeometryCollection().Get();
		FTransform Transform = Collection->Transform[index];

		const TManagedArray<FTransform>& Transforms = target->GetTransformArray();

		if (Transforms.IsValidIndex(index))
		{
			FTransform WorldTransform = Transform * target->GetComponentTransform();

			return WorldTransform.GetRotation();
		}
	}

	return FQuat::Identity;
}
