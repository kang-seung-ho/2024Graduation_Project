#include "CharacterChoice/SagaSelectPawn.h"

ASagaSelectPawn::ASagaSelectPawn()
	: Super()
	, mRoot()
	, mCamera()
{
	PrimaryActorTick.bCanEverTick = true;

	mRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(mRoot);

	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	mCamera->SetupAttachment(mRoot);
}
