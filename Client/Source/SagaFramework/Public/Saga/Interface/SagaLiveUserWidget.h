#pragma once
#include "Saga/Interface/SagaUserWidget.h"
#include <Delegates/DelegateCombinations.h>
#include <Containers/Map.h>
#include <Templates/SharedPointer.h>

#include "Saga/Interface/SagaLiveUserWidgetStates.h"
#include "SagaLiveUserWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnProgressEnded, ESagaLiveUserWidgetStates, state);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSagaLiveUserWidgetEvent);

UENUM(BlueprintType, Category = "CandyLandSaga|UI")
enum class ESagaLiveUserWidgetTickPolicy : uint8
{
	None = 0,
	TickBySelf = 1,
	FromParent = 2,
	Default = TickBySelf
};

UCLASS(BlueprintType, Blueprintable, Abstract, Category = "CandyLandSaga|UI")
class SAGAFRAMEWORK_API USagaLiveUserWidget : public USagaUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, NoClear, Category = "CandyLandSaga|UI", meta = (ExposeOnSpawn = "true"))
	ESagaLiveUserWidgetStates myState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI")
	TObjectPtr<USagaLiveUserWidget> myParent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI")
	TArray<TObjectPtr<USagaLiveUserWidget>> myChildren;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI")
	TMap<ESagaLiveUserWidgetStates, FSagaLiveUserWidgetStatus> everyStates;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|UI")
	float myProgress;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|UI")
	ESagaLiveUserWidgetTickPolicy myTickPolicy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = SetUserInteractable, BlueprintGetter = IsUserInteractable, Category = "CandyLandSaga|UI", meta = (ExposeOnSpawn = "true"))
	bool isUserInteractable;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI")
	FSagaEventOnProgressEnded OnProgressEnded;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI")
	FSagaLiveUserWidgetEvent OnJustOpened;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI")
	FSagaLiveUserWidgetEvent OnOpended;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI")
	FSagaLiveUserWidgetEvent OnClosed;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|UI")
	FSagaLiveUserWidgetEvent OnIdle;

public:
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
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& geometry, float delta_time) override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	void SetTransitionPeriod(ESagaLiveUserWidgetStates state, const float period) noexcept;

	// Default is linear
	UFUNCTION(BlueprintNativeEvent, Category = "CandyLandSaga|UI", meta = (HideSelfPin, CallableWithoutWorldContext))
	float EaseProgress(const float& progress) const;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	FSagaLiveUserWidgetStatus& GetStatus() noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI")
	FSagaLiveUserWidgetStatus& GetSpecificStatus(const ESagaLiveUserWidgetStates state) noexcept;

private:
	UFUNCTION()
	void SetProgress(const float value) noexcept;

	UFUNCTION()
	void BroadcastOnJustOpened();
	UFUNCTION()
	void BroadcastOnOpened();
	UFUNCTION()
	void BroadcastOnClosed();
	UFUNCTION()
	void BroadcastOnIdle();

	UFUNCTION()
	void ShowOnOpened();
	UFUNCTION()
	void HideOnClosed();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI")
	float GetInternalProgress() const noexcept;
};
