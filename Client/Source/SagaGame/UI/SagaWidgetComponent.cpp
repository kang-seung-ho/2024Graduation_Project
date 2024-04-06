#include "UI/SagaWidgetComponent.h"
#include "Saga/Interface/SagaUserWidget.h"

void
USagaWidgetComponent::InitWidget()
{
	Super::InitWidget();

	USagaUserWidget* SagaUserWidget = Cast<USagaUserWidget>(GetWidget());
	if (SagaUserWidget != nullptr)
	{
		SagaUserWidget->SetOwningActor(GetOwner());
	}
}
