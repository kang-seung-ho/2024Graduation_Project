// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SagaComboActionData.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API USagaComboActionData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	USagaComboActionData();

	UPROPERTY(EditAnywhere, Category = Name)
	FString MontageSectionNamePrefix; //몽타주 섹션 이름 저장

	UPROPERTY(EditAnywhere, Category = Name)
	uint8 MaxComboCount;			  //콤보액션 총 개수

	UPROPERTY(EditAnywhere, Category = Name)
	float FrameRate;				//프레임 기준 재생속도. 정확한 타이밍에 체크되도록.

	UPROPERTY(EditAnywhere, Category = ComboData)
	TArray<float> EffectiveFrameCount; //입력이 사전에 입력된건지 감지하는 프레임
};
