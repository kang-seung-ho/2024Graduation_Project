#include "Saga/LevelScripters/SagaLevelScripter.h"
#include <Engine/Level.h>
#include <Engine/LevelScriptActor.h>
#include <UObject/NameTypes.h>
#include <Kismet/GameplayStatics.h>
#include <Async/Async.h>

ASagaLevelScripter::ASagaLevelScripter()
	: Super()
	, PrevLevelName(), NextLevelName()
{}

void
ASagaLevelScripter::GotoPrevLevel_Implementation()
const
{
	if (CanGotoPrevLevel())
	{
		TransitionLevel(*PrevLevelName);
	}
}

void
ASagaLevelScripter::GotoNextLevel_Implementation()
const
{
	if (CanGotoNextLevel())
	{
		TransitionLevel(*NextLevelName);
	}
}

void
ASagaLevelScripter::SetPrevLevelName(const FName& level_name)
{
	PrevLevelName = level_name;
}

void
ASagaLevelScripter::SetNextLevelName(const FName& level_name)
{
	NextLevelName = level_name;
}

void
ASagaLevelScripter::SetPrevLevelNameFrom(const ULevel* level)
{
	PrevLevelName = level->GetFName();
}

void
ASagaLevelScripter::SetNextLevelNameFrom(const ULevel* level)
{
	NextLevelName = level->GetFName();
}

void
ASagaLevelScripter::TransitionLevel(const FName& level_name)
const
{
	if (GetName() != level_name)
	{
		AsyncTask(ENamedThreads::GameThread
			, [this, level_name]()
			{
				UGameplayStatics::OpenLevel(this, level_name);
			}
		);
	}
	else
	{
		UE_LOG(LogSagaFramework, Warning, TEXT("Same level has been translated by `TransitionLevel`."));
	}
}

bool
ASagaLevelScripter::CanGotoPrevLevel()
const noexcept
{
	return PrevLevelName.IsSet();
}

bool
ASagaLevelScripter::CanGotoNextLevel()
const noexcept
{
	return NextLevelName.IsSet();
}
