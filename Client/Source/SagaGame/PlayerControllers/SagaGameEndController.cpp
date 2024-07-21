#include "PlayerControllers/SagaGameEndController.h"
#include <Blueprint/UserWidget.h>

ASagaGameEndController::ASagaGameEndController()
noexcept
	: Super()
	, mGameResultClass(), mGameResultWidget()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass{ TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_ShowResult.UI_ShowResult_C'") };

	if (WidgetClass.Succeeded())
	{
		mGameResultClass = WidgetClass.Class;
	}
}

void ASagaGameEndController::BeginPlay()
{
	Super::BeginPlay();

	//Add GameEnd Widget to Viewport

	if (IsValid(mGameResultClass))
	{
		mGameResultWidget = CreateWidget<UUserWidget>(GetWorld(), mGameResultClass);

		if (IsValid(mGameResultWidget))
		{
			mGameResultWidget->AddToViewport(1);
		}
	}

	FInputModeGameAndUI mode{};
	SetInputMode(mode);

	SetShowMouseCursor(true);
}
