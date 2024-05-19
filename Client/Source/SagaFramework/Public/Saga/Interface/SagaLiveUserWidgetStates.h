#pragma once
#include "SagaFramework.h"

#include "SagaLiveUserWidgetStates.generated.h"

UENUM(BlueprintType, Category = "CandyLandSaga|UI")
enum class ESagaLiveUserWidgetStates : uint8
{
	None = 0,
	Opening, Idle, Closing, Dead
};

USTRUCT(BlueprintType, Category = "CandyLandSaga|UI")
struct SAGAFRAMEWORK_API FSagaLiveUserWidgetStatus
{
	GENERATED_BODY()

public:
	float Tick(const float& delta_time) noexcept;

	void SetPeriod(const float value) noexcept;

	void SetProgress(const float value) noexcept;

	float GetProgress() const noexcept;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|UI")
	float progressTime = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|UI")
	float progressPeriod = 0.2f;
};
