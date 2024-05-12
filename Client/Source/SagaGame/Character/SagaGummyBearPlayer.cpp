#include "SagaGummyBearPlayer.h"
#include "SagaGummyBearAnimInstance.h"
#include "../Effect/SagaSwordEffect.h"

#include "SagaGame/Player/SagaUserTeam.h"

#include "Saga/Network/SagaNetworkSettings.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaGummyBearPlayer::ASagaGummyBearPlayer()
{
	myHealth = 500.0f;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/NPCAssets/Modeling/Bear1.Bear1'"));
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

	mArm->SetRelativeLocation(FVector(0.0, 0.0, 250.0));
	mArm->SetRelativeRotation(FRotator(-30.0, 90.0, 0.0));
	mArm->TargetArmLength = 400.f;

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(RootComponent);
	InteractionBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	InteractionBox->SetWorldScale3D(FVector(4.5f, 5.75f, 2.25f));
	InteractionBox->SetRelativeLocation(FVector(70.0f, 0.0f, 40.0f));

	// 오버랩 이벤트 바인드
	InteractionBox->OnComponentBeginOverlap.AddDynamic(this, &ASagaGummyBearPlayer::OnOverlapBegin);
	InteractionBox->OnComponentEndOverlap.AddDynamic(this, &ASagaGummyBearPlayer::OnOverlapEnd);
}

void
ASagaGummyBearPlayer::Attack()
{
	//공격과 충돌되는 물체 여부 판단
	FHitResult Result{};

	FVector Start = GetActorLocation() + GetActorForwardVector() * 50.f;
	FVector End = Start + GetActorForwardVector() * 150.f;

	FCollisionQueryParams param = FCollisionQueryParams::DefaultQueryParam;
	param.AddIgnoredActor(this);

	bool Collision = GetWorld()->SweepSingleByChannel(Result, Start, End, FQuat::Identity, ECC_GameTraceChannel4, FCollisionShape::MakeSphere(50.f), param);

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

void
ASagaGummyBearPlayer::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void
ASagaGummyBearPlayer::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void
ASagaGummyBearPlayer::BeginPlay()
{
	Super::BeginPlay();
}

float
ASagaGummyBearPlayer::ExecuteHurt(const float dmg)
{
	UE_LOG(LogSagaGame, Log, TEXT("[Character][Bear] ExecuteHurt (%f)"), dmg);

	BearHp -= dmg;
	Stat->ApplyDamage(dmg);

	UE_LOG(LogTemp, Log, TEXT("Bear Hp : %f"), BearHp);

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());
	if (BearHp <= 0.0f)
	{
		// 사망 애니메이션 실행
		mBearAnimInst->Death();

		// 사망 처리 (이동 정리, 충돌 해제)
		ExecuteDeath();

		// 파티클 이펙트 실행

		//상대 팀 점수 증가 실행
		if (system)
		{
			system->AddScore(myTeam == EUserTeam::Red ? EUserTeam::Blue : EUserTeam::Red, 3);

			if constexpr (not saga::IsOfflineMode)
			{
				system->SendRpcPacket(ESagaRpcProtocol::RPC_DMG_PLYER, BearHp, 1);
			}
			else
			{

			}
		}

		//Destroy();
	}
	else
	{
		if (system)
		{
			if constexpr (not saga::IsOfflineMode)
			{
				system->SendRpcPacket(ESagaRpcProtocol::RPC_DMG_PLYER, BearHp, 1);
			}
			else
			{

			}
		}
	}

	return dmg;
}

void
ASagaGummyBearPlayer::ExecuteDeath()
{
	Super::ExecuteDeath();
}
