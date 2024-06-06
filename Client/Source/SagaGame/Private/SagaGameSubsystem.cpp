#include "SagaGameSubsystem.h"
#include <UObject/Object.h>
#include <Engine/World.h>
#include <GameFramework/Actor.h>
#include <Subsystems/SubsystemCollection.h>

USagaGameSubsystem::USagaGameSubsystem()
	: Super()
	, localPlayerSpawner()
{}

void
USagaGameSubsystem::Initialize(FSubsystemCollectionBase& collection)
{

}

void
USagaGameSubsystem::Deinitialize()
{

}

bool
USagaGameSubsystem::ShouldCreateSubsystem(UObject* outer)
const
{
	return true;
}

USagaGameSubsystem*
USagaGameSubsystem::GetSubSystem(const UWorld* world)
{
	const auto singleton = world->GetGameInstance();

	return singleton->GetSubsystem<USagaGameSubsystem>();
}

void
USagaGameSubsystem::AssignLocalPlayerSpawner(AActor* spawner)
noexcept
{
	localPlayerSpawner = spawner;
}

const AActor*
USagaGameSubsystem::GetLocalPlayerSpawner()
const noexcept
{
	return localPlayerSpawner;
}
