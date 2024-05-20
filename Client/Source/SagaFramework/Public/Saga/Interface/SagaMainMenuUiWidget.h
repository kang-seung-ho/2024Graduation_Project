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
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI", meta = (AllowPrivateAccess = true))
	TObjectPtr<USagaUiEditableTextBox> myInputBox;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI", meta = (AllowPrivateAccess = true))
	TObjectPtr<USagaUiButton> mySigninButton;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI", meta = (AllowPrivateAccess = true))
	TObjectPtr<USagaLiveUserWidget> myPopup;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI", meta = (DisplayName = "Nickname"))
	FText transientNickname;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI")
	bool isConnecting;

	USagaMainMenuUiWidget(const FObjectInitializer& initializer) noexcept;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void OnConnected();
	UFUNCTION()
	void OnFailedToConnect(ESagaConnectionContract reason);

	UFUNCTION()
	void HandleSigningIn();
	UFUNCTION()
	void HandleButtonPressed();
	UFUNCTION()
	void HandleInputTextChanged(const FText& nickname);
	UFUNCTION()
	void HandleInputCommitted(const FText& nickname, ETextCommit::Type method);
};
