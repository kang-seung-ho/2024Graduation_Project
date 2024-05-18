#include "Saga/Controllers/SagaMainMenuController.h"
#include <UObject/Object.h>
#include <UObject/ConstructorHelpers.h>
#include <Blueprint/UserWidget.h>

#include "Saga/Interface/SagaMainMenuUiWidget.h"

ASagaMainMenuController::ASagaMainMenuController()
	: Super()
	, levelUiClass(), levelUiInstance()
{
	static ConstructorHelpers::FClassFinder<USagaMainMenuUiWidget> widget_class(TEXT("/Game/UI/Level/SagaMainMenuLevelUI.SagaMainMenuLevelUI_C"));

	if (widget_class.Succeeded())
	{
		levelUiClass = widget_class.Class;
	}
	else
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[ASagaMainMenuController][Ctor] Could not find the class of user interface for main menu."));
	}
}

void
ASagaMainMenuController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeUIOnly UIOnlyInputMode;
	SetInputMode(UIOnlyInputMode);

	levelUiInstance = SpawnUI(GetWorld(), levelUiClass);
	if (IsValid(levelUiInstance))
	{
		levelUiInstance->AddToViewport(1);
		//levelUiInstance->isu
	}
	else
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[ASagaMainMenuController][BeginPlay] Could not create user interface for main menu."));
	}
}

USagaMainMenuUiWidget*
ASagaMainMenuController::SpawnUI(UWorld* world, UClass* const uclass)
{
	return CreateWidget<USagaMainMenuUiWidget>(world, uclass);
}
