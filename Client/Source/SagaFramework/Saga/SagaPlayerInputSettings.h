#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "SagaPlayerInputSettings.generated.h"

UCLASS(Category = "CandyLandSaga|Framework")
class SAGAFRAMEWORK_API USagaPlayerInputSettings : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network")
	static FKey GetMoveForwardKey() noexcept
	{
		return "W";
	}

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network")
	static FKey GetMoveBackwardKey() noexcept
	{
		return "S";
	}

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network")
	static FKey GetMoveLeftKey() noexcept
	{
		return "A";
	}

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network")
	static FKey GetMoveRightKey() noexcept
	{
		return "D";
	}

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network")
	static FKey GetJumpKey() noexcept
	{
		return "SpaceBar";
	}
};
