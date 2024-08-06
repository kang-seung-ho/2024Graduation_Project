#include "AI/SagaAIMovementComponent.h"
#include "SagaGameInfo.h"

void USagaAIMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If the Velocity becomes zero, the monster looks at the first place
	// so in order to prevent this, if the velocity is zero, do not calculate the rotation

	/*if (PawnOwner == nullptr || UpdatedComponent == nullptr || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.0f) * DeltaTime * MaxSpeed;
	if (!DesiredMovementThisFrame.IsNearlyZero())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentQuat(), true, Hit);
	}*/

	if (Velocity.Length() > 0)
	{
		FVector Dir = Velocity;
		Dir.Z = 0.0;

		Dir.Normalize();

		FRotator DirRot = Dir.Rotation();
		PawnOwner->SetActorRotation(FRotator(0.0, DirRot.Yaw, 0.0));
	}
	
}
