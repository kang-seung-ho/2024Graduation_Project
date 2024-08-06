#include "AI/AIMonsterAnimInstance.h"
#include "SagaGameInfo.h"
#include "SagaMonsterAIPawn.h"

void UAIMonsterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	mAnim = EAIMonsterAnim::Idle;
}

void UAIMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UAIMonsterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

void UAIMonsterAnimInstance::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();
}

void UAIMonsterAnimInstance::NativeUninitializeAnimation()
{
	Super::NativeUninitializeAnimation();
}

void UAIMonsterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UAIMonsterAnimInstance::AnimNotify_AttackEnd()
{
	ASagaMonsterAIPawn* Monster = Cast<ASagaMonsterAIPawn>(TryGetPawnOwner());

	if (IsValid(Monster))
	{
		Monster->SetAttackAnimEnd(true);
	}
}

void UAIMonsterAnimInstance::AnimNotify_Attack()
{
	ASagaMonsterAIPawn* Monster = Cast<ASagaMonsterAIPawn>(TryGetPawnOwner());

	if (IsValid(Monster))
	{
		Monster->ExecuteAttack();
	}
}
