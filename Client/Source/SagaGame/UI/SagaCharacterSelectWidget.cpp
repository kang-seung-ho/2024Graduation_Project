#include "SagaCharacterSelectWidget.h"
#include "Character/CharacterSelect/SagaCharacterSelectController.h"

void
USagaCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void
USagaCharacterSelectWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void 
USagaCharacterSelectWidget::StartButtonEnable(bool bEnable)
{
	mStartButton->SetIsEnabled(bEnable);
}
