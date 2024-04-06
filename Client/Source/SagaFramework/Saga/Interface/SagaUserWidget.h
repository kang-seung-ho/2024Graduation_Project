#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "SagaUserWidget.generated.h"

UCLASS(BlueprintType, Blueprintable, EditInlineNew, Category = "CandyLandSaga|Interface")
class SAGAFRAMEWORK_API USagaUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Interface|Property")
	FORCEINLINE void SetOwningActor(AActor* NewOwner) { OwningActor = NewOwner; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Interface|Property")
	TObjectPtr<AActor> OwningActor; // 소유자 보관
};
