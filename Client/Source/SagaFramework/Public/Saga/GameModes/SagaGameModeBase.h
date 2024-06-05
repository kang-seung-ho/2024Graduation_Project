#pragma once
#include "SagaFramework.h"
#include <GameFramework/GameModeBase.h>
#include <GameFramework/PlayerController.h>
#include <GameFramework/Pawn.h>
#include <GameFramework/SpectatorPawn.h>
#include <GameFramework/GameSession.h>
#include <GameFramework/HUD.h>
#include <Templates/SubclassOf.h>

#include "SagaGameModeBase.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract, Category = "CandyLandSaga|System|Game Mode")
class SAGAFRAMEWORK_API ASagaGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASagaGameModeBase() noexcept;

protected:
	UFUNCTION(Category = "CandyLandSaga|System|Game Mode")
	void SetControllerClass(TSubclassOf<APlayerController> controller_class) noexcept;
	UFUNCTION(Category = "CandyLandSaga|System|Game Mode")
	void SetPawnClass(TSubclassOf<APawn> pawn_class) noexcept;
	UFUNCTION(Category = "CandyLandSaga|System|Game Mode")
	void SetSpectatorClass(TSubclassOf<ASpectatorPawn> pawn_class) noexcept;
	UFUNCTION(Category = "CandyLandSaga|System|Game Mode")
	void SetSessionClass(TSubclassOf<AGameSession> session_class) noexcept;
	UFUNCTION(Category = "CandyLandSaga|System|Game Mode")
	void SetHUDClass(TSubclassOf<AHUD> hud_class) noexcept;
};
