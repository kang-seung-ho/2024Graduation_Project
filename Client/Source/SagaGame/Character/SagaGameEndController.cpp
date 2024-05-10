// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SagaGameEndController.h"
#include "Blueprint/UserWidget.h"

ASagaGameEndController::ASagaGameEndController()
{
	//Load GameEnd Widget

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_ShowResult.UI_ShowResult_C'"));

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
			mGameResultWidget->AddToViewport();
		}
	}
}
