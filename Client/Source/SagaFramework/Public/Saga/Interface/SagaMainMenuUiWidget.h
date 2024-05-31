#pragma once
#include "Saga/Interface/SagaLiveUserWidget.h"

#include "Saga/Interface/SagaUiEditableTextBox.h"
#include "Saga/Interface/SagaUiButton.h"

#include "SagaMainMenuUiWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Main Menu Level UI Widget C++ Class"))
class SAGAFRAMEWORK_API USagaMainMenuUiWidget final : public USagaLiveUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<class USagaUiEditableTextBox> myInputBox;

	UPROPERTY()
	TObjectPtr<class USagaUiButton> mySigninButton;
	
	UPROPERTY()
	TObjectPtr<class UButton> mySettingButton;

	UPROPERTY()
	TObjectPtr<USagaLiveUserWidget> myConnectionPopup;

	UPROPERTY()
	FText transientNickname;

	UPROPERTY()
	bool isConnecting;

	UPROPERTY()
	bool isSettingOpened;

	USagaMainMenuUiWidget(const FObjectInitializer& initializer) noexcept;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnConnected();
	UFUNCTION()
	void OnFailedToConnect(ESagaConnectionContract reason);

	UFUNCTION()
	void HandleSigningIn();
	UFUNCTION()
	void HandleButtonPressed();
	UFUNCTION()
	void HandleOpenSetting();
	UFUNCTION()
	void HandleCloseSetting();
	UFUNCTION()
	void HandleInputTextChanged(const FText& nickname);
	UFUNCTION()
	void HandleInputCommitted(const FText& nickname, ETextCommit::Type method);

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, NoClear, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class USagaUserWidget> settingUiClass;
	UPROPERTY()
	TObjectPtr<class USagaMainMenuSettingsUiWidget> settingUiInstance;
};
