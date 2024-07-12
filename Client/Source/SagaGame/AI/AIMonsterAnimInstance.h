#pragma once
#include "SagaGame.h"
#include <Animation/AnimInstance.h>
#include "../SagaGame/Public/SagaGameInfo.h"
#include "SagaAIInfo.h"

#include "AIMonsterAnimInstance.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game|AI")
class SAGAGAME_API UAIMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EAIMonsterAnim mAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float mHitAlpha = 0.f;

public:
	void ChangeAnim(EAIMonsterAnim Anim)
	{
		mAnim = Anim;
	}
	
public:
	virtual void NativeInitializeAnimation();
	virtual void NativeUpdateAnimation(float DeltaSeconds);
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds);
	virtual void NativePostEvaluateAnimation();
	virtual void NativeUninitializeAnimation();

	virtual void NativeBeginPlay();
};
