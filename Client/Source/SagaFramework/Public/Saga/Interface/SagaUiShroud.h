#pragma once
#include "Saga/Interface/SagaLiveUserWidget.h"
#include <Components/Widget.h>

#include "SagaUiShroud.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Shroud"))
class SAGAFRAMEWORK_API USagaUiShroud : public USagaLiveUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = SetMaxOpacity, BlueprintGetter = GetMaxOpacity, Export, Category = "CandyLandSaga|UI|Content", meta = (ExposeOnSpawn = "true"))
	float maxOpacity;

	UPROPERTY()
	FGetFloat maxOpacityDelegate;

	USagaUiShroud(const FObjectInitializer& initializer) noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "Get Max Opacity (Saga Shroud)"))
	void SetMaxOpacity(const float value) noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "Get Max Opacity (Saga Shroud)"))
	float GetMaxOpacity() const noexcept;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& geometry, float delta_time) override;

	PROPERTY_BINDING_IMPLEMENTATION(float, maxOpacity);
};
