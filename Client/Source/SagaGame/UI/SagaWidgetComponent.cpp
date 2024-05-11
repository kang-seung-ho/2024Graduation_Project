// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SagaWidgetComponent.h"
#include "../SagaFramework/Saga/Interface/SagaUserWidget.h"

void USagaWidgetComponent::InitWidget()
{
	Super::InitWidget();

	USagaUserWidget* SagaUserWidget = Cast<USagaUserWidget>(GetWidget());
	if (SagaUserWidget)
	{
		SagaUserWidget->SetOwningActor(GetOwner());
	}
}
