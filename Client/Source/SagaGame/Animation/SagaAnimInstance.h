// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SagaAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SAGAGAME_API USagaAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	USagaAnimInstance();
	

protected:
	//이벤트 함수
	virtual void NativeInitializeAnimation() override; // AnimInstance가 처음 생성될때 한번 호출

	virtual void NativeUpdateAnimation(float DeltaSeconds) override; //매 프레임마다 호출됨

	//anim graph가 참조할 변수들을 선언
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class ACharacter> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class UCharacterMovementComponent> Movement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	FVector Velocity; //속도 저장

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float GroundSpeed; //땅위의 속력 지정

	//idle상태인지 아닌지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsIdle : 1;

	//움직이는지 Idle인지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float MovingThreshould;

	//떨어지는중인지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsFalling : 1;

	//지금 점프중인지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsJumping : 1;

	//점프중인지 나타내기위함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float JumpingThreshould;

};
