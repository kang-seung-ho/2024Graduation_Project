#include "Saga/Level/Scripter/SagaLevelScripterBase.h"
#include "Kismet/GameplayStatics.h"
#include "EngineLogs.h"

ASagaLevelScripterBase::ASagaLevelScripterBase()
	: ALevelScriptActor()
	, PrevLevelName(), NextLevelName()
{
}

void
ASagaLevelScripterBase::GotoPrevLevel_Implementation()
const
{
	if (CanGotoPrevLevel())
	{
		TransitionLevel(*PrevLevelName);
	}
}

void
ASagaLevelScripterBase::GotoNextLevel_Implementation()
const
{
	if (CanGotoNextLevel())
	{
		TransitionLevel(*NextLevelName);
	}
}

bool
ASagaLevelScripterBase::CanGotoPrevLevel()
const noexcept
{
	return PrevLevelName.IsSet();
}

bool
ASagaLevelScripterBase::CanGotoNextLevel()
const noexcept
{
	return NextLevelName.IsSet();
}

void
ASagaLevelScripterBase::SetPrevLevelName(const FName& level_name)
{
	PrevLevelName = level_name;
}

void
ASagaLevelScripterBase::SetNextLevelName(const FName& level_name)
{
	NextLevelName = level_name;
}

void
ASagaLevelScripterBase::SetPrevLevelNameFrom(const ULevel* level)
{
	PrevLevelName = level->GetFName();
}

void
ASagaLevelScripterBase::SetNextLevelNameFrom(const ULevel* level)
{
	NextLevelName = level->GetFName();
}

void
ASagaLevelScripterBase::TransitionLevel(const FName& level_name)
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
		UE_LOG(LogLevel, Warning, TEXT("Same level has been translated by `TransitionLevel`."));
	}
}
