#pragma once
#include "Saga/GameModes/SagaInterfaceOnlyModeBase.h"

#include "SagaCharacterChoiceMode.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSagaSystem|System|Game Mode")
class SAGAFRAMEWORK_API ASagaCharacterChoiceMode final : public ASagaInterfaceOnlyModeBase
{
	GENERATED_BODY()

public:
	ASagaCharacterChoiceMode(const FObjectInitializer& initializer) noexcept;
};
