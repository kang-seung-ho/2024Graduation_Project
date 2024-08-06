#pragma once
#include "SagaGame.h"
#include "SagaGameInfo.h"
#include <UObject/ObjectPtr.h>
#include <GameFramework/Pawn.h>

#include "SagaSelectPawn.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Character Choice Level")
class SAGAGAME_API ASagaSelectPawn : public APawn
{
	GENERATED_BODY()

public:
	ASagaSelectPawn();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USceneComponent> mRoot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCameraComponent> mCamera;
};
