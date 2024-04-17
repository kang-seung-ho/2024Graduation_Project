#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "SagaNetworkRpcView.generated.h"

UINTERFACE(MinimalAPI)
class USagaNetworkRpcView : public UInterface
{
	GENERATED_BODY()
};

class SAGANETWORK_API ISagaNetworkRpcView
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Event")
	void BeginWalk();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Event")
	void EndWalk();
	UFUNCTION(Category = "CandyLandSaga|Game|Event")
	virtual void ExecuteWalk() {}
	UFUNCTION(Category = "CandyLandSaga|Game|Event")
	virtual void TerminateWalk() {}
};
