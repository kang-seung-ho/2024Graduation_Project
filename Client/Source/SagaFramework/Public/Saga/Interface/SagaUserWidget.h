#pragma once
#include "SagaFramework.h"
#include <Blueprint/UserWidget.h>

#include "SagaUserWidget.generated.h"

UCLASS(BlueprintType, Blueprintable, Abstract, EditInlineNew, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga User Widget C++ Class"))
class SAGAFRAMEWORK_API USagaUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// 현재 월드에서 소유자 보관
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, BlueprintSetter = SetOwningActor, BlueprintGetter = GetOwningActor, Category = "CandyLandSaga|UI")
	TObjectPtr<AActor> OwningActor;

public:
	USagaUserWidget(const FObjectInitializer& initializer) noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	void SetOwningActor(AActor* const owner) noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI")
	AActor* GetOwningActor() const noexcept;
};
