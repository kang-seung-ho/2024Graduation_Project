#pragma once
#include "Saga/Interface/SagaLiveUserWidget.h"

#include "SagaMainMenuSettingsUiWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Main Menu Settings UI Widget C++ Class"))
class SAGAFRAMEWORK_API USagaMainMenuSettingsUiWidget final : public USagaLiveUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<class UButton> myResetButton;

	UPROPERTY()
	TObjectPtr<class UButton> myOkButton;

	UPROPERTY()
	TObjectPtr<class UCheckBox> myCheckBox;

	UPROPERTY()
	TObjectPtr<class UComboBoxKey> myComboBox;

	UPROPERTY()
	TObjectPtr<class USagaUiEditableTextBox> myIpAddressInputBox;

	UPROPERTY()
	TObjectPtr<class USagaUiEditableTextBox> myPortInputBox;

	UPROPERTY()
	TObjectPtr<class UButton> myOfflineModeResetButton;

	UPROPERTY()
	TObjectPtr<class UButton> myConnectOptionResetButton;

	UPROPERTY()
	TObjectPtr<class UButton> myIpAddressResetButton;

	UPROPERTY()
	TObjectPtr<class UButton> mPortResetButton;

	USagaMainMenuSettingsUiWidget(const FObjectInitializer& initializer) noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Main Menu Level")
	void SetOfflineModeChecker(bool is_checked);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Main Menu Level")
	bool GetOfflineModeChecker() const noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Main Menu Level")
	FText GetIpAddressHintText() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Main Menu Level")
	FText GetPortHintText() const;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleConnectionOption(FName item, ESelectInfo::Type method);
	UFUNCTION()
	void HandleOnIpAddressModified(const FText& text);
	UFUNCTION()
	void HandleOnIpAddressCommitted(const FText& text, ETextCommit::Type method);
	UFUNCTION()
	void HandleOnPortNumberModified(const FText& text);
	UFUNCTION()
	void HandleOnPortNumberCommitted(const FText& text, ETextCommit::Type method);
	UFUNCTION()
	void HandleOnResetEverything();
	UFUNCTION()
	void HandleOnResetOfflineMode();
	UFUNCTION()
	void HandleOnResetConnectionOption();
	UFUNCTION()
	void HandleOnResetRemoteIpAddress();
	UFUNCTION()
	void HandleOnResetRemotePortNumber();
	UFUNCTION()
	void HandleClose();
	UFUNCTION()
	void HandleOnClosed();

	UFUNCTION()
	bool AcquireOfflineMode() const;
	UFUNCTION()
	ESagaNetworkConnectionCategory AcquireConnectionOption() const;
	UFUNCTION()
	FString AcquireIpAddress() const;
	UFUNCTION()
	int32 AcquirePort() const;

	UPROPERTY()
	FText storedIpAddress;
	UPROPERTY()
	FText storedPortNumber;
};
