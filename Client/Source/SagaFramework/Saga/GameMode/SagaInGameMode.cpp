#include "Saga/GameMode/SagaInGameMode.h"
#include "Saga/Controller/SagaInGameController.h"

ASagaInGameMode::ASagaInGameMode()
	: ASagaGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> SagaCharacterClassRef(TEXT("/Script/Engine.Blueprint'/Game/BP/BP_SagaCharacterPlayer.BP_SagaCharacterPlayer_C'"));
	if (SagaCharacterClassRef.Succeeded() && SagaCharacterClassRef.Class)
	{
		DefaultPawnClass = SagaCharacterClassRef.Class;
	}

	//static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/SagaFramework.SagaInGameController_C'"));
	//if (PlayerControllerClassRef.Succeeded() && PlayerControllerClassRef.Class)
	//{
	//		PlayerControllerClass = PlayerControllerClassRef.Class;
	//}

	static auto PlayerControllerClassRef = ASagaInGameController::StaticClass();
	if (PlayerControllerClassRef)
	{
		PlayerControllerClass = PlayerControllerClassRef;
	}
}
