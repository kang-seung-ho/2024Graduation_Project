#include "Saga/Level/SagaGameLevel.h"
#include <HAL/Platform.h>
#include <UObject/ConstructorHelpers.h>
#include <UObject/Object.h>
#include <Delegates/Delegate.h>
#include <Blueprint/UserWidget.h>

#include "Saga/Network/SagaRpcProtocol.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaGameLevel::ASagaGameLevel()
noexcept
	: Super()
	, mTeamScoreBoardClass(), mTeamScoreBoard()
{
	SetNextLevelName(TEXT("GameVictoryLevel"));
	SetPrevLevelName(TEXT("MainMenuLevel"));

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_ScoreBoard.UI_ScoreBoard_C'"));

	if (WidgetClass.Succeeded())
	{
		mTeamScoreBoardClass = WidgetClass.Class;
	}
}

void
ASagaGameLevel::BeginPlay()
{
	Super::BeginPlay();

	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	system->OnDisconnected.AddDynamic(this, &ASagaGameLevel::OnDisconnected);
	system->OnLeftRoomBySelf.AddDynamic(this, &ASagaGameLevel::OnLeftRoomBySelf);
	system->OnRpc.AddDynamic(this, &ASagaGameLevel::OnRpc);

	if (IsValid(mTeamScoreBoardClass))
	{
		mTeamScoreBoard = CreateWidget<UUserWidget>(GetWorld(), mTeamScoreBoardClass);

		if (IsValid(mTeamScoreBoard))
		{
			mTeamScoreBoard->AddToViewport(1);
		}
	}
}

void
ASagaGameLevel::BeginDestroy()
{
	Super::BeginDestroy();
}

void
ASagaGameLevel::OnDisconnected()
{
	GotoPrevLevel();
}

void
ASagaGameLevel::OnLeftRoomBySelf()
{
	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	if (not system->IsConnected())
	{
		SetPrevLevelName(TEXT("MainMenuLevel"));
	}
	else
	{
		SetPrevLevelName(TEXT("LobbyLevel"));
	}

	GotoPrevLevel();
}

void
ASagaGameLevel::OnRpc(ESagaRpcProtocol cat, int32 id, int64 arg0, int32 arg1)
{
	
}
