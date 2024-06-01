// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AI/SagaMonsterAIPawn.h"
#include "SagaMonsterAIController.h"

ASagaMonsterAIPawn::ASagaMonsterAIPawn()
{
	AIControllerClass = ASagaMonsterAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    mCapsule->SetCollisionProfileName(TEXT("Enemy"));
}

void ASagaMonsterAIPawn::BeginPlay()
{
	Super::BeginPlay();

    // 서버에서 실행 중인지 확인
    if (!HasAuthority())
    {
        return;
    }

    // AI가 동적으로 스폰되면 자동 빙의를 활성화
    //AutoPossessAI = EAutoPossessAI::Spawned;

    // 필요한 경우 기본 컨트롤러를 스폰
    /*if (AIControllerClass != nullptr && GetController() == nullptr)
    {
        SpawnDefaultController();
    }*/
}

void ASagaMonsterAIPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
