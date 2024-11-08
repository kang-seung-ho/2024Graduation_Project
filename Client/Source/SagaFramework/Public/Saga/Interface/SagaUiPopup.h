#pragma once
#include "Saga/Interface/SagaLiveUserWidget.h"

#include "SagaUiPopup.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Popup C++ Class"))
class SAGAFRAMEWORK_API USagaUiPopup : public USagaLiveUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<class USagaUiPanelWidget> myBody;

	UPROPERTY()
	TObjectPtr<class UTextBlock> myTitle;

	UPROPERTY()
	TObjectPtr<class UTextBlock> myText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = SetTitle, BlueprintGetter = GetTitle, Category = "CandyLandSaga|UI|Content", meta = (ExposeOnSpawn = "true"))
	FText popupTitle;

	UPROPERTY()
	FGetText popupTitleDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = SetText, BlueprintGetter = GetText, Category = "CandyLandSaga|UI|Content", meta = (ExposeOnSpawn = "true"))
	FText popupContentText;

	UPROPERTY()
	FGetText popupContentTextDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = SetTitleColorAndOpacity, BlueprintGetter = GetTitleColorAndOpacity, Category = "CandyLandSaga|UI|Content")
	FSlateColor titleColorAndOpacity;

	UPROPERTY()
	FGetSlateColor titleColorAndOpacityDelegate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = SetTextColorAndOpacity, BlueprintGetter = GetTextColorAndOpacity, Category = "CandyLandSaga|UI|Content")
	FSlateColor textColorAndOpacity;

	UPROPERTY()
	FGetSlateColor textColorAndOpacityDelegate;

	USagaUiPopup(const FObjectInitializer& initializer);

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "Set Title (Saga Popup)"))
	void SetTitle(FText text) noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "Get Title (Saga Popup)"))
	FText GetTitle() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "Set Text (Saga Popup)"))
	void SetText(FText text) noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "Get Text (Saga Popup)"))
	FText GetText() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "Set Title Color And Opacity (Saga Popup)"))
	void SetTitleColorAndOpacity(FSlateColor colour) noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "Get Title Color And Opacity (Saga Popup)"))
	FSlateColor GetTitleColorAndOpacity() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "Set Title Opacity (Saga Popup)"))
	void SetTitleOpacity(float opacity) noexcept;
	
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "Set Text Color And Opacity (Saga Popup)"))
	void SetTextColorAndOpacity(FSlateColor colour) noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "Get Text Color And Opacity (Saga Popup)"))
	FSlateColor GetTextColorAndOpacity() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Content", meta = (DisplayName = "Set Text Opacity (Saga Popup)"))
	void SetTextOpacity(float opacity) noexcept;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& geometry, float delta_time) override;

	UFUNCTION()
	void HandleOnClosed();

	PROPERTY_BINDING_IMPLEMENTATION(FText, popupTitle);
	PROPERTY_BINDING_IMPLEMENTATION(FText, popupContentText);
	PROPERTY_BINDING_IMPLEMENTATION(FSlateColor, textColorAndOpacity);
	PROPERTY_BINDING_IMPLEMENTATION(FSlateColor, titleColorAndOpacity);
};
