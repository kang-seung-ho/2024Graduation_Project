// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/SagaAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/SagaCharacterPlayer.h"

USagaAnimInstance::USagaAnimInstance()
{
	MovingThreshould = 3.0f;
	JumpingThreshould = 100.0f;
}

void USagaAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	//초기화 로직
	//GetOwningActor(); //현재 AnimInstance를 사용하고 있는 액터정보 얻어올 수 있음.
	//액터타입으로 반환되기 때문에 캐릭터로 형변환해줌
	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}

}

void USagaAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	//프레임마다 Movement객체가 있을때 Movement객체로부터 원하는 값들을 얻으면 됨
	if (Movement)
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D();
		bIsIdle = GroundSpeed < MovingThreshould;
		bIsFalling = Movement->IsFalling();
		bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshould);
	}

	// 이 코드에서 탑승한 캐릭터를 가져온다
	APawn* OwningPawn = TryGetPawnOwner();
	if (!OwningPawn)
	{
		return;
	}

	// 탑승 상태에 따라 다른 애니메이션 로직을 구현
	ASagaCharacterPlayer* CurrentCharacter = Cast<ASagaCharacterPlayer>(OwningPawn);
	//if (CurrentCharacter && CurrentCharacter->IsRiding())
	//{
	//	// 탑승 애니메이션 로직
	//}
	//else
	//{
	//	// 기본 애니메이션 로직
	//}
}
