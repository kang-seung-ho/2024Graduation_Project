#pragma once
#include <CoreMinimal.h>
#include <UObject/Interface.h>

#include "SagaNetworkRpcView.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class USagaNetworkRpcView : public UInterface
{
	GENERATED_BODY()
};

class SAGANETWORK_API ISagaNetworkRpcView
{
	GENERATED_BODY()

public:
	
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void ExecuteWalk() {}
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void TerminateWalk() {}

	
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void ExecuteRun() {}
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void TerminateRun() {}

	
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void ExecuteJump() {}
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void TerminateJump() {}

	
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void ExecuteAttack() {}
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void TerminateAttack() {}

	
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void ExecuteRide() {}
	UFUNCTION(Category = "CandyLandSaga|Game|RPC", meta = (NotBlueprintThreadSafe))
	virtual void TerminateRide() {}
};
