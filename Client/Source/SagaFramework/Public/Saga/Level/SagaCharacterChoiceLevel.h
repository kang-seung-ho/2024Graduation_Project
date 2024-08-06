#pragma once
#include "Saga/Level/SagaLevelScripter.h"

#include "SagaCharacterChoiceLevel.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game|Level", meta = (DisplayName = "Saga Character Choice Level C++ Class"))
class SAGAFRAMEWORK_API ASagaCharacterChoiceLevel final : public ASagaLevelScripter
{
	GENERATED_BODY()

public:
	ASagaCharacterChoiceLevel() noexcept;
};
