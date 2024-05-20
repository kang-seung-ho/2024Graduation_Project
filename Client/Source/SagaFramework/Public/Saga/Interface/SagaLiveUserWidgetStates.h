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
	/*
	*    Current seconds time of progress
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|UI", meta = (DisplayName = "Time"))
	float progressTime = 0.0f;
	/*
	*    Max seconds time of progress
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|UI", meta = (DisplayName = "Period"))
	float progressPeriod = 0.2f;
};
