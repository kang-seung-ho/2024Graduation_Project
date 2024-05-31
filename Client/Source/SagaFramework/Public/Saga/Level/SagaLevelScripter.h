#pragma once
#include "SagaFramework.h"
#include <Engine/LevelScriptActor.h>
#include <Misc/Optional.h>

#include "SagaLevelScripter.generated.h"

UCLASS(Blueprintable, BlueprintType, Category = "CandyLandSaga|Framework|Level")
class SAGAFRAMEWORK_API ASagaLevelScripter : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	ASagaLevelScripter();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure = false, Category = "CandyLandSaga|Level")
	void GotoPrevLevel() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure = false, Category = "CandyLandSaga|Level")
	void GotoNextLevel() const;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Level")
	void SetPrevLevelName(const FName& level_name);
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Level")
	void SetNextLevelName(const FName& level_name);
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Level")
	void SetPrevLevelNameFrom(const ULevel* level);
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Level")
	void SetNextLevelNameFrom(const ULevel* level);

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "CandyLandSaga|Level")
	void TransitionLevel(const FName& level_name) const;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Level")
	bool CanGotoPrevLevel() const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Level")
	bool CanGotoNextLevel() const noexcept;

private:
	TOptional<FName> PrevLevelName;
	TOptional<FName> NextLevelName;
};
