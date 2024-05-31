#pragma once
#include "Saga/Level/SagaLevelScripter.h"

#include "Saga/Network/SagaRpcProtocol.h"
#include "Saga/Network/SagaGameContract.h"

#include "SagaCharacterChoiceLevel.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game|Level", meta = (DisplayName = "Saga Character Choice Level C++ Class"))
class SAGAFRAMEWORK_API ASagaCharacterChoiceLevel final : public ASagaLevelScripter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, NoClear)
	TSubclassOf<class USagaCharacterSelectWidget> levelUiClass;
	UPROPERTY()
	TObjectPtr<class USagaCharacterSelectWidget> levelUiInstance;

	UPROPERTY()
	FTimerHandle choiceTimerHandle;
	UPROPERTY()
	FTimerHandle countdownTimerHandle;

	ASagaCharacterChoiceLevel() noexcept;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void PeriodicUpdate();
	UFUNCTION()
	void HandleCompleteCountdown();

	UFUNCTION()
	void HandleClickedCharacter(class ASagaSelectCharacter* character);

	UFUNCTION()
	void OnDisconnected();
	UFUNCTION()
	void OnFailedToStartGame(ESagaGameContract reason);
	UFUNCTION()
	void OnRpc(ESagaRpcProtocol cat, int32 user_id, int64 arg0, int32 arg1);
};
