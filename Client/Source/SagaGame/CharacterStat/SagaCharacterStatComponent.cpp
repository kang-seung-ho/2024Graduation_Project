// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/SagaCharacterStatComponent.h"

// Sets default values for this component's properties
USagaCharacterStatComponent::USagaCharacterStatComponent()
{
	MaxHp = 200.0f;
	CurrentHp = MaxHp;
}


// Called when the game starts
void USagaCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	SetHp(MaxHp);
}

float USagaCharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	SetHp(PrevHp - ActualDamage); //0보다 작지 않고 MaxHp를 초과하지 않도록 제어
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		//죽었을때의 이벤트 실행
		OnHpZero.Broadcast();
	}
	return ActualDamage;
}

void USagaCharacterStatComponent::SetHp(float NewHp) //hp변동될때 실행하는 함수
{
	//hp가 변경되려면 이 함수를 거쳐야 합니다.
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);

	//hp바뀌었다는 신호 보내기
	OnHpChanged.Broadcast(CurrentHp);
}
