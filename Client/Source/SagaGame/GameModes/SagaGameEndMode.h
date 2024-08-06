#pragma once
#include "SagaGame.h"
#include <GameFramework/GameModeBase.h>

#include "SagaGameEndMode.generated.h"

UCLASS()
class SAGAGAME_API AGameEndMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGameEndMode(const FObjectInitializer& initializer) noexcept;

	virtual void BeginPlay() override;
};
