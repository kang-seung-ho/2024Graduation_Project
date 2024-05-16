#include "SagaInGameMode.h"
#include "Character/SagaInGamePlayerController.h"

ASagaInGameMode::ASagaInGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassRef(TEXT("/Game/PlayerAssets/BP/BP_SagaPlayableCharacter.BP_SagaPlayableCharacter_C"));
	if (PlayerPawnClassRef.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnClassRef.Class;
	}

	PlayerControllerClass = ASagaInGamePlayerController::StaticClass();
}
