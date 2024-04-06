// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_AttackHitChecking.h"
#include "Interface/SagaAttackAnimationInterface.h"

void UAnimNotify_AttackHitChecking::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (MeshComp)
	{
		ISagaAttackAnimationInterface* AttackPawn = Cast<ISagaAttackAnimationInterface>(MeshComp->GetOwner());
		if (AttackPawn)
		{
			AttackPawn->AttackHitCheck();
		}
	}
}
