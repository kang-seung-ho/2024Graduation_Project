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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Event")
	void BeginRun();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Event")
	void EndRun();
	UFUNCTION(Category = "CandyLandSaga|Game|Event")
	virtual void ExecuteRun() {}
	UFUNCTION(Category = "CandyLandSaga|Game|Event")
	virtual void TerminateRun() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Event")
	void BeginJump();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Event")
	void EndJump();
	UFUNCTION(Category = "CandyLandSaga|Game|Event")
	virtual void ExecuteJump() {}
	UFUNCTION(Category = "CandyLandSaga|Game|Event")
	virtual void TerminateJump() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Event")
	void BeginAttack();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Event")
	void EndAttack();
	UFUNCTION(Category = "CandyLandSaga|Game|Event")
	virtual void ExecuteAttack() {}
	UFUNCTION(Category = "CandyLandSaga|Game|Event")
	virtual void TerminateAttack() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Event")
	void BeginRide();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Game|Event")
	void EndRide();
	UFUNCTION(Category = "CandyLandSaga|Game|Event")
	virtual void ExecuteRide() {}
	UFUNCTION(Category = "CandyLandSaga|Game|Event")
	virtual void TerminateRide() {}

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "CandyLandSaga|Game|Session")
	int32 myId = -1;
};
