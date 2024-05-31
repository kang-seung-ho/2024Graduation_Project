#include "UI/SagaWidgetComponent.h"
#include "Saga/Interface/SagaUserWidget.h"

void
USagaWidgetComponent::InitWidget()
{
	UWidgetComponent::InitWidget();

	USagaUserWidget* SagaUserWidget = Cast<USagaUserWidget>(GetWidget());
	if (SagaUserWidget)
	{
		SagaUserWidget->SetOwningActor(GetOwner());
	}
}
