#pragma once
#include "SagaGame.h"
#include <GameFramework/Pawn.h>

#include "SagaSelectPawn.generated.h"

UCLASS()
class SAGAGAME_API ASagaSelectPawn : public APawn
{
	GENERATED_BODY()

public:
	ASagaSelectPawn();

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* mRoot;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* mCamera;
};
