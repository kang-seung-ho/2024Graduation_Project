#include "SagaCharacterPlayer.h"
#include "SagaPlayerAnimInstance.h"
#include "SagaGummyBearAnimInstance.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

void
ASagaCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	//이 함수 호출되기 전에 SkeletalMeshComponent에 지정된 AnimInstance 클래스 이용하여 사용하기 위한 객체 만들어놨음.
	mAnimInst = Cast<USagaPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	mBearAnimInst = Cast<USagaGummyBearAnimInstance>(GetMesh()->GetAnimInstance());

	// TODO: 시스템 멤버 변수 말고 FSagaVirtualUser에서 가져와야 함
	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	UE_LOG(LogSagaGame, Log, TEXT("Weapon Type : %d"), (int)myWeaponType);
}

void
ASagaCharacterPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void
ASagaCharacterPlayer::Tick(float delta_time)
{
	Super::Tick(delta_time);

	ProcessMovement();
	ProcessAnimation(delta_time);
}
