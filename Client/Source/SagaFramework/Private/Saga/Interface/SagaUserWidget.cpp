#include "Saga/Interface/SagaUserWidget.h"
#include <UObject/UObjectGlobals.h>
#include <GameFramework/Actor.h>

USagaUserWidget::USagaUserWidget(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, OwningActor()
{}

void
USagaUserWidget::SetOwningActor(AActor* const owner)
noexcept
{
	OwningActor = owner;
}

AActor*
USagaUserWidget::GetOwningActor()
const noexcept
{
	return OwningActor;
}
