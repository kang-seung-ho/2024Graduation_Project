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
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Session")
	void BeginWalk();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Session")
	void EndWalk();
	UFUNCTION(Category = "CandyLandSaga|Game|Session")
	virtual void ExecuteWalk() {}
	UFUNCTION(Category = "CandyLandSaga|Game|Session")
	virtual void TerminateWalk() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Session")
	void BeginRun();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Session")
	void EndRun();
	UFUNCTION(Category = "CandyLandSaga|Game|Session")
	virtual void ExecuteRun() {}
	UFUNCTION(Category = "CandyLandSaga|Game|Session")
	virtual void TerminateRun() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Session")
	void BeginJump();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Session")
	void EndJump();
	UFUNCTION(Category = "CandyLandSaga|Game|Session")
	virtual void ExecuteJump() {}
	UFUNCTION(Category = "CandyLandSaga|Game|Session")
	virtual void TerminateJump() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Session")
	void BeginAttack();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Session")
	void EndAttack();
	UFUNCTION(Category = "CandyLandSaga|Game|Session")
	virtual void ExecuteAttack() {}
	UFUNCTION(Category = "CandyLandSaga|Game|Session")
	virtual void TerminateAttack() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Session")
	void BeginRide();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Session")
	void EndRide();
	UFUNCTION(Category = "CandyLandSaga|Game|Session")
	virtual void ExecuteRide() {}
	UFUNCTION(Category = "CandyLandSaga|Game|Session")
	virtual void TerminateRide() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Session")
	int32 GetID() const noexcept;
};
