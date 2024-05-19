#pragma once
#include "Saga/Interface/SagaLiveUserWidget.h"

#include "Saga/Interface/SagaUiEditableTextBox.h"
#include "Saga/Interface/SagaUiButton.h"

#include "SagaMainMenuUiWidget.generated.h"

UCLASS(Blueprintable, BlueprintType, Category = "CandyLandSaga|UI")
class SAGAFRAMEWORK_API USagaMainMenuUiWidget final : public USagaLiveUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI", meta = (AllowPrivateAccess = true))
	TObjectPtr<USagaUiEditableTextBox> myInputBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI", meta = (AllowPrivateAccess = true))
	TObjectPtr<USagaUiButton> mySigninButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI", meta = (AllowPrivateAccess = true))
	TObjectPtr<USagaLiveUserWidget> myPopup;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI|Contents", meta = (AllowPrivateAccess = true))
	FText transientNickname;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NoClear, Category = "CandyLandSaga|UI", meta = (AllowPrivateAccess = true))
	bool isConnecting;

public:
	USagaMainMenuUiWidget(const FObjectInitializer& initializer) noexcept;

protected:
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
