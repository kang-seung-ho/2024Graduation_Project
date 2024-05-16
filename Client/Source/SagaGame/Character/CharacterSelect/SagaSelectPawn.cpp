#include "SagaSelectPawn.h"
#include "SagaGameInfo.h"

ASagaSelectPawn::ASagaSelectPawn()
{ 
	mRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	SetRootComponent(mRoot);

	mCamera->SetupAttachment(mRoot);
}
