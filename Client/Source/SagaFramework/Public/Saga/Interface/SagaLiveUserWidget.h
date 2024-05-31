#pragma once
#include "Saga/Interface/SagaUserWidget.h"
#include <Delegates/DelegateCombinations.h>
#include <Containers/Map.h>
#include <Templates/SharedPointer.h>

#include "Saga/Interface/SagaLiveUserWidgetStates.h"
#include "SagaLiveUserWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnProgressEnded, ESagaLiveUserWidgetStates, state);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSagaLiveUserWidgetEvent);

UENUM(BlueprintType, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Live Widget Tick Policy", Bitflags))
enum class ESagaLiveUserWidgetTickPolicy : uint8
{
	None = 0,

	TickBySelf = 1,
	FromParent = 2,
	Both = 3,

	Default = TickBySelf
};

ENUM_CLASS_FLAGS(ESagaLiveUserWidgetTickPolicy)

UCLASS(BlueprintType, Blueprintable, Abstract, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Live User Widget C++ Class"))
class SAGAFRAMEWORK_API USagaLiveUserWidget : public USagaUserWidget
{
	GENERATED_BODY()

public:
	/*
	*    A state of this live widget
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintSetter = SetState, BlueprintGetter = GetState, NoClear, Category = "CandyLandSaga|UI", meta = (ExposeOnSpawn = "true"))
	ESagaLiveUserWidgetStates myState;

	/*
	*    The parent of this live widget
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI")
	TObjectPtr<USagaLiveUserWidget> myParent;

	/*
	*    Live widgets belong to this live widget
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI")
	TArray<TObjectPtr<USagaLiveUserWidget>> myChildren;

	/*
	*    List of states of this live widget
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI")
	TMap<ESagaLiveUserWidgetStates, FSagaLiveUserWidgetStatus> everyStates;

	/*
	*    The tick policy of this live widget
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|UI")
	ESagaLiveUserWidgetTickPolicy myTickPolicy;

	/*
	*    A flag that indicates whether if this live widget does interact or not
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = SetUserInteractable, BlueprintGetter = IsUserInteractable, Category = "CandyLandSaga|UI", meta = (ExposeOnSpawn = "true"))
	bool isUserInteractable;

	UPROPERTY()
	FGetBool isUserInteractableDelegate;

	/*
	*    Executes after an UI's progress is completed
	*/
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI")
	FSagaEventOnProgressEnded OnProgressEnded;
	/*
	*    Executes after an UI's progress is started
	*/
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI")
	FSagaLiveUserWidgetEvent OnJustOpened;
	/*
	*    Executes after an UI's opening progress is completed
	*/
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI")
	FSagaLiveUserWidgetEvent OnOpened;
	/*
	*    Executes after an UI's closing progress is completed
	*/
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI")
	FSagaLiveUserWidgetEvent OnClosed;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI")
	FSagaLiveUserWidgetEvent OnIdle;

	USagaLiveUserWidget(const FObjectInitializer& initializer) noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	void SetSagaWidgetParent(USagaLiveUserWidget* const ptr);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI")
	USagaLiveUserWidget* GetSagaWidgetParent() const;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	void SetParentProgressive(const bool flag) noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI")
	bool IsParentProgressive() const noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI")
	bool IsBoundedSagaWidget() const;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	void SetProgressTickable(const bool flag) noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI")
	bool IsProgressTickable() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	void SetUserInteractable(const bool flag) noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI")
	bool IsUserInteractable() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	void SetState(ESagaLiveUserWidgetStates state) noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI")
	ESagaLiveUserWidgetStates GetState() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	void SetProgress(const float value) noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	void SetTransitionPeriod(ESagaLiveUserWidgetStates state, const float period) noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	virtual void Open(const float begin_progress = 0);

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	virtual void Close(const float begin_progress = 0);

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	void OpenNow();

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	void CloseNow();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI")
	float GetProgress() const noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI")
	float GetGraphicalProgress() const noexcept;

protected:
	UPROPERTY()
	FTimerHandle myTicker;

	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& geometry, float delta_time) override;

	// Default is linear
	UFUNCTION(BlueprintNativeEvent, Category = "CandyLandSaga|UI", meta = (HideSelfPin, CallableWithoutWorldContext))
	float EaseProgress(const float& progress) const;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	FSagaLiveUserWidgetStatus& GetStatus() noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	FSagaLiveUserWidgetStatus& GetSpecificStatus(const ESagaLiveUserWidgetStates state) noexcept;

	PROPERTY_BINDING_IMPLEMENTATION(bool, isUserInteractable);

private:
	UFUNCTION()
	void BroadcastOnJustOpened();
	UFUNCTION()
	void BroadcastOnOpened();
	UFUNCTION()
	void BroadcastOnClosed();
	UFUNCTION()
	void BroadcastOnIdle();

	UFUNCTION()
	virtual void HandleJustOpened();
	UFUNCTION()
	virtual void HandleOpened();
	UFUNCTION()
	virtual void HandleClosed();
	UFUNCTION()
	virtual void HandleCompletion();
};

class SAGAFRAMEWORK_API FSagaEasingLibrary
{
public:
	[[nodiscard]]
	static constexpr
		double
		CatmullRomSpline(double alpha, double pt0, double pt1, double pt2, double pt3)
		noexcept
	{
		auto&& __c1 = pt1;
		auto&& __c2 = (pt2 - pt0) * 0.5;
		auto&& __c3 = pt0 - pt1 * 2.5 + pt2 * 2 - pt3 * 0.5;
		auto&& __c4 = (pt3 - pt0) * 0.5 + (pt1 - pt2) * 1.5;

		return (((__c4 * alpha + __c3) * alpha + __c2) * alpha + __c1);
	}

	[[nodiscard]]
	static constexpr
		float
		CatmullRomSpline(float alpha, float pt0, float pt1, float pt2, float pt3)
		noexcept
	{
		auto&& __c1 = pt1;
		auto&& __c2 = (pt2 - pt0) * 0.5f;
		auto&& __c3 = pt0 - pt1 * 2.5f + pt2 * 2.0f - pt3 * 0.5f;
		auto&& __c4 = (pt3 - pt0) * 0.5f + (pt1 - pt2) * 1.5f;

		return (((__c4 * alpha + __c3) * alpha + __c2) * alpha + __c1);
	}
};
