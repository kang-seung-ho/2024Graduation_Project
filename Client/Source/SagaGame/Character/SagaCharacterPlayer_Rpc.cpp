#include "SagaCharacterPlayer.h"

void
ASagaCharacterPlayer::ExecuteWalk()
{
	const FRotator rotation = K2_GetActorRotation();
	const FRotator yaw = FRotator(0.0, rotation.Yaw, 0.0);
	const FVector forward_dir = yaw.Vector();
	const FVector right_dir = FRotationMatrix(yaw).GetScaledAxis(EAxis::Y);

	AddMovementInput(forward_dir, straightMoveDirection * GetVerticalMoveAcceleration());
	AddMovementInput(right_dir, strafeMoveDirection * GetHorizontalMoveAcceleration());
}

void
ASagaCharacterPlayer::TerminateStraightWalk()
{
	straightMoveDirection = 0;
}

void
ASagaCharacterPlayer::TerminateStrafeWalk()
{
	strafeMoveDirection = 0;
}

void
ASagaCharacterPlayer::ExecuteRun()
{
	// 달리기 시작할 때 속도를 높임
	GetCharacterMovement()->MaxWalkSpeed = GetMaxMoveSpeed(true);
}

void
ASagaCharacterPlayer::TerminateRun()
{
	// 달리기를 멈췄을 때 속도를 원래대로 복원
	GetCharacterMovement()->MaxWalkSpeed = GetMaxMoveSpeed(false);
}

void
ASagaCharacterPlayer::ExecuteJump()
{
	if (CanJump())
	{
		Jump();
	}
}

void
ASagaCharacterPlayer::TerminateJump()
{}

void
ASagaCharacterPlayer::ExecuteAttack()
{
	/*PlayAttackAnimation();*/
}

void
ASagaCharacterPlayer::TerminateAttack()
{

}

void
ASagaCharacterPlayer::ExecuteRide()
{

}

void
ASagaCharacterPlayer::TerminateRide()
{

}

void
ASagaCharacterPlayer::ProcessStraightWalk(const int& direction)
noexcept
{
	UE_LOG(LogSagaGame, Log, TEXT("[Character] ProcessStraightWalk (%d)"), direction);

	straightMoveDirection = direction;
}

void
ASagaCharacterPlayer::ProcessStrafeWalk(const int& direction)
noexcept
{
	UE_LOG(LogSagaGame, Log, TEXT("[Character] ProcessStrafeWalk (%d)"), direction);

	strafeMoveDirection = direction;
}
