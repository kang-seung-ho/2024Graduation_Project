#include "SagaCharacterPlayer.h"
#include "SagaPlayerAnimInstance.h"
#include "SagaGummyBearAnimInstance.h"

void
ASagaCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	// BeginPlay 호출되기 전에 SkeletalMeshComponent에 지정된
	// AnimInstance 클래스를 사용하기 위한 객체 만들어놨음.
	mAnimInst = Cast<USagaPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	mBearAnimInst = Cast<USagaGummyBearAnimInstance>(GetMesh()->GetAnimInstance());

	if (myId == -1)
	{
		SetTeamColorAndCollision(EUserTeam::Blue);
	}
	
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
