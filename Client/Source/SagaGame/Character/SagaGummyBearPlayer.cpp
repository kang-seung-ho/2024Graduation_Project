#include "SagaGummyBearPlayer.h"
#include <GeometryCollection/GeometryCollectionComponent.h>
#include <GeometryCollection/GeometryCollectionObject.h>
#include <GeometryCollection/GeometryCollection.h>
#include <GeometryCollection/GeometryCollectionAlgo.h>
#include <NiagaraComponent.h>
#include <NiagaraFunctionLibrary.h>

#include "Player/SagaPlayerTeam.h"
#include "Character/SagaPlayableCharacter.h"
#include "Character/SagaGummyBearAnimInstance.h"
#include "Effect/SagaSwordEffect.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

class USagaCharacterDismComponent;

ASagaGummyBearPlayer::ASagaGummyBearPlayer()
	: Super()
	, BearNum(0)
	, isCanRide(false)
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

	myCameraSpringArmComponent->SetRelativeLocation(FVector(0.0, 0.0, 250.0));
	myCameraSpringArmComponent->SetRelativeRotation(FRotator(-30.0, 90.0, 0.0));
	myCameraSpringArmComponent->TargetArmLength = 400.f;

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(RootComponent);
	InteractionBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	InteractionBox->SetWorldScale3D(FVector(4.5f, 5.75f, 2.25f));
	InteractionBox->SetRelativeLocation(FVector(70.0f, 0.0f, 40.0f));

	//*/
	Dbox_Rarm = CreateDefaultSubobject<UBoxComponent>(TEXT("Dbox_Rarm"));
	Dbox_Rarm->SetupAttachment(GetMesh(), TEXT("arm_stretch_r"));
	InitializeTransform(Dbox_Rarm, FVector(-50, -65, -7), FRotator(6, -50, 82), FVector(1.25, 1.25, 2.25));
	Dbox_Rarm->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DismCollisionBox.Add(Dbox_Rarm);

	Dbox_Rleg = CreateDefaultSubobject<UBoxComponent>(TEXT("Dbox_Rleg"));
	Dbox_Rleg->SetupAttachment(GetMesh(), TEXT("thigh_stretch_r"));
	InitializeTransform(Dbox_Rleg, FVector(2, -25, 10), FRotator(-30, 57, 72), FVector(1.5, 1.5, 2.0));
	Dbox_Rleg->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DismCollisionBox.Add(Dbox_Rleg);

	Dbox_Larm = CreateDefaultSubobject<UBoxComponent>(TEXT("Dbox_Larm"));
	Dbox_Larm->SetupAttachment(GetMesh(), TEXT("arm_stretch_l"));
	InitializeTransform(Dbox_Larm, FVector(40, -80, -15), FRotator(6, 40, 76), FVector(1.25, 1.25, 2.0));
	Dbox_Larm->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DismCollisionBox.Add(Dbox_Larm);

	Dbox_Lleg = CreateDefaultSubobject<UBoxComponent>(TEXT("Dbox_Lleg"));
	Dbox_Lleg->SetupAttachment(GetMesh(), TEXT("thigh_stretch_l"));
	InitializeTransform(Dbox_Lleg, FVector(-8, -15, 8), FRotator(-50, 100, -45), FVector(1.25, 1.25, 2.0));
	Dbox_Lleg->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DismCollisionBox.Add(Dbox_Lleg);

	//*/
	r_arm = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("r_arm"));
	r_arm->SetupAttachment(Dbox_Rarm);
	static ConstructorHelpers::FObjectFinder<UGeometryCollection> r_armAsset(TEXT("GeometryCollection'/Game/works/meshes/GC_r_arm.GC_r_arm'"));
	if (r_armAsset.Succeeded())
	{
		r_arm->RestCollection = r_armAsset.Object;
	}
	InitializeTransform(r_arm, FVector(2, 2, 6), FRotator(80, 68, -50), FVector(0.8, 0.8, 0.44));
	GeometryCollections.Add(r_arm);

	r_leg = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("r_leg"));
	r_leg->SetupAttachment(Dbox_Rleg);
	static ConstructorHelpers::FObjectFinder<UGeometryCollection> r_legAsset(TEXT("GeometryCollection'/Game/works/meshes/GC_r_leg.GC_r_leg'"));
	if (r_legAsset.Succeeded())
	{
		r_leg->RestCollection = r_legAsset.Object;
	}
	InitializeTransform(r_leg, FVector(3.8, -3, -10), FRotator(-5, 41, 0), FVector(0.66, 0.66, 0.5));
	GeometryCollections.Add(r_leg);

	l_arm = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("l_arm"));
	l_arm->SetupAttachment(Dbox_Larm);
	static ConstructorHelpers::FObjectFinder<UGeometryCollection> l_armAsset(TEXT("GeometryCollection'/Game/works/meshes/GC_l_arm.GC_l_arm'"));
	if (l_armAsset.Succeeded())
	{
		l_arm->RestCollection = l_armAsset.Object;
	}
	InitializeTransform(l_arm, FVector(1, 3, 8), FRotator(-82, -141, 54), FVector(0.8, 0.8, 0.5));
	GeometryCollections.Add(l_arm);

	l_leg = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("l_leg"));
	l_leg->SetupAttachment(Dbox_Lleg);
	static ConstructorHelpers::FObjectFinder<UGeometryCollection> l_legAsset(TEXT("GeometryCollection'/Game/works/meshes/GC_l_leg.GC_l_leg'"));
	if (l_legAsset.Succeeded())
	{
		l_leg->RestCollection = l_legAsset.Object;
	}
	InitializeTransform(l_leg, FVector(0, 2, -12), FRotator(2, 154, 2), FVector(0.8, 0.66, 0.5));
	GeometryCollections.Add(l_leg);

	for (int i = 0; i < GeometryCollections.Num(); i++)
	{
		GeometryCollections[i]->SetVisibility(false);
		GeometryCollections[i]->SetSimulatePhysics(false);
		GeometryCollections[i]->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
	InitTargetMeshes();
	//*/
	DismPartID = -1;
	DismThreshold = 1;

	NiagaraComponentTemplate = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MorphNiagaraComponent"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NSAsset(TEXT("NiagaraSystem'/Game/works/NS_MeshMorph.NS_MeshMorph'"));
	if (NSAsset.Succeeded())
	{
		NiagaraSystemTemplate = NSAsset.Object;
	}
	NiagaraComponentTemplate->SetupAttachment(RootComponent);
	NiagaraComponentTemplate->SetAutoActivate(false);

	// 오버랩 이벤트 바인드
	InteractionBox->OnComponentBeginOverlap.AddDynamic(this, &ASagaGummyBearPlayer::OnOverlapBegin);
	InteractionBox->OnComponentEndOverlap.AddDynamic(this, &ASagaGummyBearPlayer::OnOverlapEnd);
}

void
ASagaGummyBearPlayer::BeginPlay()
{
	Super::BeginPlay();
	ActiveIndx.Init(DismThreshold, 4);
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
ASagaGummyBearPlayer::PlayAttackAnimation()
{
	if (IsValid(mBearAnimInst))
	{
#if WITH_EDITOR
		const auto name = GetName();
		UE_LOG(LogSagaGame, Log, TEXT("[ASagaGummyBearPlayer][PlayAttackAnimation] '%s' is a bear character."), *name);
#endif

		mBearAnimInst->PlayAttackMontage();
	}
	else
	{
		UE_LOG(LogSagaGame, Error, TEXT("[ASagaGummyBearPlayer] mBearAnimInst is null."));
	}
}


template<typename T>
void ASagaGummyBearPlayer::InitializeTransform(T component, FVector Location, FRotator Rotation, FVector Scale)
{
	component->SetRelativeLocation(Location);
	component->SetRelativeRotation(Rotation);
	component->SetRelativeScale3D(Scale);
}

void ASagaGummyBearPlayer::TryDismemberment(FVector Hitlocation, FVector HitNormal)
{

	OnTakeDamage(Hitlocation, HitNormal);

}

float
ASagaGummyBearPlayer::ExecuteHurt(const float dmg)
{
	UE_LOG(LogSagaGame, Log, TEXT("[ASagaGummyBearPlayer] ExecuteHurt (%f)"), dmg);

	const auto current_hp = Super::ExecuteHurt(dmg);
	UE_LOG(LogSagaGame, Log, TEXT("Bear Hp : %f"), current_hp);

	if (0 < current_hp)
	{
		// 피격 이펙트

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

	Super::ExecuteDeath();

	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (system->IsOfflineMode())
	{
		// 상대 팀 점수 증가 실행
		system->AddScore(GetTeam() == ESagaPlayerTeam::Red ? ESagaPlayerTeam::Blue : ESagaPlayerTeam::Red, 1);
	}
	else
	{
		// 상대 팀 점수 증가 실행
		system->AddScore(GetTeam() == ESagaPlayerTeam::Red ? ESagaPlayerTeam::Blue : ESagaPlayerTeam::Red, 1);
	}
}

void
ASagaGummyBearPlayer::OnTakeDamage(FVector Location, FVector Normal)
{
	if (IsAlive())
	{
		for (int32 i = 0; i < DismCollisionBox.Num(); i++)
		{
			if (IsPointInsideBox(DismCollisionBox[i], Location))
			{
				ActiveIndx[i]--;
				if (ActiveIndx[i] == 0)
				{
					//UE_LOG(LogTemp, Warning, TEXT("HitBox: %d, boxHP: %d"), i, ActiveIndx[i]);
					DismPartID = i;

					FVector Impulse = -Normal * 250.0f; // Example impulse calculation
					CheckValidBone(Impulse, DismPartID);

					break;
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("HitBox: %d, boxHP: %d"), i, ActiveIndx[i]);
		}

		if (ActiveIndx[1] <= 0 && ActiveIndx[3] <= 0)
		{
			ExecuteHurt(GetHealth());
		}
	}
}


UGeometryCollectionComponent* ASagaGummyBearPlayer::GetGeometryCollectionByName(const FString& CollectionName)
{
	if (Owner)
	{
		TArray<UActorComponent*> Components;
		Owner->GetComponents(Components);
		for (UActorComponent* Component : Components)
		{
			UGeometryCollectionComponent* GeometryCollectionComponent = Cast<UGeometryCollectionComponent>(Component);
			if (GeometryCollectionComponent && GeometryCollectionComponent->GetName() == CollectionName)
			{
				return GeometryCollectionComponent;
			}
		}
	}
	return nullptr;
}


FVector ASagaGummyBearPlayer::GetPieceWorldPosition(UGeometryCollectionComponent* TargetGC, int32 PieceIndex)
{
	if (TargetGC)
	{
		const TManagedArray<FTransform>& Transforms = TargetGC->GetTransformArray();
		if (Transforms.IsValidIndex(PieceIndex))
		{
			FTransform WorldTransform = Transforms[PieceIndex] * TargetGC->GetComponentTransform();
			return WorldTransform.GetLocation();
		}
	}
	return FVector::ZeroVector;
}

FQuat ASagaGummyBearPlayer::GetPieceWorldRotation(UGeometryCollectionComponent* TargetGC, int32 PieceIndex)
{

	if (TargetGC)
	{
		const UGeometryCollection* GeometryCollection = TargetGC->GetRestCollection();
		FGeometryCollection* Collection = GeometryCollection->GetGeometryCollection().Get();
		FTransform Transform = Collection->Transform[PieceIndex];

		const TManagedArray<FTransform>& Transforms = TargetGC->GetTransformArray();
		if (Transforms.IsValidIndex(PieceIndex))
		{
			FTransform WorldTransform = Transform * TargetGC->GetComponentTransform();
			return WorldTransform.GetRotation();
			//return WorldTransform.GetRotation();
		}
	}
	return FQuat::Identity;
}

bool ASagaGummyBearPlayer::IsPointInsideBox(UBoxComponent* Box, const FVector& Point)
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

void ASagaGummyBearPlayer::CheckValidBone(const FVector& Impulse, int32 Index)
{
	USkinnedMeshComponent* SkinnedMesh = FindComponentByClass<USkinnedMeshComponent>();
	UBoxComponent* BOX = DismCollisionBox[Index];
	FName BoneName = BOX->GetAttachSocketName();
	if (!SkinnedMesh->IsBoneHiddenByName(BoneName))
	{
		UE_LOG(LogTemp, Warning, TEXT("TriggerExplosion: %d"), Index);
		ExplodeBodyParts(BoneName, Impulse, Index);
	}
}

void ASagaGummyBearPlayer::ExplodeBodyParts(FName BoneName, const FVector& Impulse, int32 Index)
{
	USkinnedMeshComponent* SkinnedMesh = FindComponentByClass<USkinnedMeshComponent>();
	SkinnedMesh->HideBoneByName(BoneName, PBO_None);
	UGeometryCollectionComponent* TargetGC = GeometryCollections[Index];

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

void ASagaGummyBearPlayer::HideTargetPiece(UGeometryCollectionComponent* TargetGC, int32 PieceIndex)
{

	TargetGC->SetVisibility(false, true);

}

FTransform ASagaGummyBearPlayer::SpawnMorphSystem(UGeometryCollectionComponent* TargetGC, int32 Index)
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

	FVector Location = GetPieceWorldPosition(TargetGC, PieceIndex);
	FQuat Rot = GetPieceWorldRotation(TargetGC, PieceIndex);

	if (NiagaraSystemTemplate)
	{

		FHitResult hitresult;
		FVector Starttrace = Location + FVector(0, 0, 100);
		FVector Endtrace = Location + FVector(0, 0, -1000);

		if (GetWorld()->LineTraceSingleByChannel(hitresult, Starttrace, Endtrace, ECollisionChannel::ECC_Visibility))
		{
			NiagaraComponentTemplate->SetAsset(NiagaraSystemTemplate);
			Location = hitresult.Location + FVector(0, 0, 45.0f);

			UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				NiagaraSystemTemplate,
				Location,
				FRotator::ZeroRotator,
				FVector(1.f)
			);

			UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMesh(NiagaraComponent, TEXT("User.SourceMesh"), nullptr);
			UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMesh(NiagaraComponent, TEXT("User.SourceMesh"), GetTargetMesh(Index));
			NiagaraComponent->SetVariableQuat(TEXT("InitRotator"), Rot);
			NiagaraComponent->ActivateSystem(true);

			HideTargetPiece(TargetGC, PieceIndex);

			FTransform output;
			output.SetLocation(Location);
			output.SetRotation(FQuat::Identity);

			return output;
		}

	}

	return FTransform::Identity;

}

UStaticMesh* ASagaGummyBearPlayer::GetTargetMesh(int32 Index)
{
	return TargetMeshes[Index];
}


void ASagaGummyBearPlayer::InitTargetMeshes()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh1Asset(TEXT("StaticMesh'/Game/works/meshes/rarm_13.rarm_13'"));
	if (Mesh1Asset.Succeeded())
	{
		TargetMeshes.Add(Mesh1Asset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh2Asset(TEXT("StaticMesh'/Game/works/meshes/rleg_10.rleg_10'"));
	if (Mesh2Asset.Succeeded())
	{
		TargetMeshes.Add(Mesh2Asset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh3Asset(TEXT("StaticMesh'/Game/works/meshes/larm_6.larm_6'"));
	if (Mesh3Asset.Succeeded())
	{
		TargetMeshes.Add(Mesh3Asset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh4Asset(TEXT("StaticMesh'/Game/works/meshes/lleg_4.lleg_4'"));
	if (Mesh3Asset.Succeeded())
	{
		TargetMeshes.Add(Mesh4Asset.Object);
	}

	//UE_LOG(LogTemp, Warning, TEXT("targetmesheslen: %d"), TargetMeshes.Num());
}
