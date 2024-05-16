#include "Saga/Level/SagaLevelScripter.h"
#include <Engine/Level.h>
#include <Engine/LevelScriptActor.h>
#include <UObject/NameTypes.h>
#include <Kismet/GameplayStatics.h>
#include <Async/Async.h>

#include "Saga/Network/SagaNetworkSettings.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

ASagaLevelScripter::ASagaLevelScripter()
	: Super()
	, PrevLevelName(), NextLevelName()
{}

void
ASagaLevelScripter::BeginPlay()
{
	Super::BeginPlay();

	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());
	if (IsValid(system))
	{
		const auto my_name = GetName();
		UE_LOG(LogSagaFramework, Log, TEXT("[BeginPlay] Level '%s' is registering network events..."), *my_name);

		RegisterView(system, this);
	}
	else if (system->IsOfflineMode())
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[BeginPlay] Network subsystem is not ready. (Offline Mode)"));
	}
	else
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[BeginPlay] Network subsystem is not ready."));
	}
}

void
ASagaLevelScripter::EndPlay(const EEndPlayReason::Type reason)
{
	Super::EndPlay(reason);

	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());
	if (IsValid(system))
	{
		const auto my_name = GetName();
		UE_LOG(LogSagaFramework, Log, TEXT("[BeginPlay] Level '%s' is deregistering network events..."), *my_name);

		DeregisterView(system, this);
	}
	else
	{
		UE_LOG(LogSagaFramework, Warning, TEXT("[EndPlay] Network subsystem is not ready."));
	}
}

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
		UE_LOG(LogLevel, Warning, TEXT("Same level has been translated by `TransitionLevel`."));
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
