#include "SagaInGameMode.h"
#include <Math/MathFwd.h>
#include <Containers/UnrealString.h>
#include <Containers/Array.h>
#include <UObject/Object.h>
#include <UObject/ConstructorHelpers.h>
#include <Templates/Casts.h>
#include <Kismet/GameplayStatics.h>
#include <Engine/World.h>
#include <EngineUtils.h>
#include <GameFramework/Controller.h>
#include <NiagaraSystem.h>

#include "SagaGameSubsystem.h"
#include "PlayerControllers/SagaInGamePlayerController.h"
#include "Player/SagaPlayerTeam.h"
#include "Character/SagaCharacterSpawner.h"
#include "Character/SagaCharacterBase.h"
#include "Character/SagaPlayableCharacter.h"
#include "Character/SagaGummyBearPlayer.h"
#include "Obstacles/MapObstacle1.h"
#include "Item/SagaItemBox.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

const FString SagaRedTeamName = TEXT("Red");
const FString SagaBluTeamName = TEXT("Blue");

ASagaInGameMode::ASagaInGameMode(const FObjectInitializer& initializer)
	: Super(initializer)
	, storedLocalPlayerController()
	, playerSpawners()
	, lastCharacterPosition(), lastCharacterRotation()
	, everyBears()
	, everyItemSpawnEntities(), everyItemBoxes()
	, readyTimerHandle()
	, transformUpdateTimer()
	, guardianScannerTimer()
{
	SetControllerClass(ASagaInGamePlayerController::StaticClass());

	static ConstructorHelpers::FClassFinder<ASagaPlayableCharacter> pawn_class{ TEXT("/Game/PlayerAssets/BP/BP_SagaPlayableCharacter.BP_SagaPlayableCharacter_C") };
	if (pawn_class.Succeeded())
	{
		SetPawnClass(pawn_class.Class);
	}
	else
	{
		const auto name = GetName();
		UE_LOG(LogSagaGame, Error, TEXT("[ASagaInGameMode][ctor] '%s' could not find the class of player character."), *name);
	}

	everyBears.Reserve(3);
	everyItemSpawnEntities.Reserve(100);
}

ASagaGummyBearPlayer*
ASagaInGameMode::FindBear(const int32 id)
const noexcept
{
	for (auto& bear : everyBears)
	{
		if (bear->GetBearId() == id)
		{
			return bear;
		}
	}

	return nullptr;
}

void
ASagaInGameMode::InitGame(const FString& mapname, const FString& optios, FString& err_msg)
{
	UE_LOG(LogSagaGame, Log, TEXT("[ASagaInGameMode][InitGame]"));

	Super::InitGame(mapname, optios, err_msg);

	const auto world = GetWorld();

	playerSpawners.Reserve(10);
	for (TActorIterator<ASagaCharacterSpawner> it{ world }; it; ++it)
	{
		const auto spawner = *it;

		playerSpawners.Add(spawner);
	}

	const auto spawners_number = playerSpawners.Num();
	UE_LOG(LogSagaGame, Log, TEXT("%d spawner(s) has found."), spawners_number);

	if (playerSpawners.IsEmpty())
	{
		UE_LOG(LogSagaGame, Warning, TEXT("Any spawner does not exist."));

		FVector center{ 0, 0, 100 };
		const auto automatic_spawner = world->SpawnActor<ASagaCharacterSpawner>(center, FRotator::ZeroRotator);
		ensure(automatic_spawner != nullptr);

		const FTransform transform{ FRotator::ZeroRotator, center };
		UGameplayStatics::FinishSpawningActor(automatic_spawner, transform);

		UE_LOG(LogSagaGame, Log, TEXT("An automatic spawner is generated."));
	}

	const auto net = USagaNetworkSubSystem::GetSubSystem(world);

	if (not net->IsOfflineMode())
	{
		net->OnLeftRoom.AddDynamic(this, &ASagaInGameMode::OnLeftRoom);
		net->OnGameStarted.AddDynamic(this, &ASagaInGameMode::OnGameStarted);
		net->OnCreatingCharacter.AddDynamic(this, &ASagaInGameMode::OnCreatingCharacter);
		net->OnRpc.AddDynamic(this, &ASagaInGameMode::OnRpc);
	}
}

void
ASagaInGameMode::StartPlay()
{
	const auto world = GetWorld();
	const auto net = USagaNetworkSubSystem::GetSubSystem(world);
	const auto sys = USagaGameSubsystem::GetSubSystem(world);

	if (net->IsOfflineMode())
	{
#if WITH_EDITOR

		UE_LOG(LogSagaGame, Warning, TEXT("[ASagaInGameMode][StartPlay] (Offline Mode)"));
#endif

		// NOTICE: 여기서 오프라인 모드에서 팀 설정함
		net->SetLocalUserTeam(ESagaPlayerTeam::Blue);
	}
	else
	{
#if WITH_EDITOR

		UE_LOG(LogSagaGame, Log, TEXT("[ASagaInGameMode][StartPlay]"));
#endif
	}

	switch (net->GetLocalUserTeam())
	{
	case ESagaPlayerTeam::Red:
	{
#if WITH_EDITOR

		UE_LOG(LogSagaGame, Log, TEXT("Assigning a player spawner for red team..."));
#endif

		sys->AssignLocalPlayerSpawner(GetSpawner(SagaRedTeamName));
	}
	break;

	case ESagaPlayerTeam::Blue:
	{
#if WITH_EDITOR

		UE_LOG(LogSagaGame, Log, TEXT("Assigning a player spawner for blue team..."));
#endif

		sys->AssignLocalPlayerSpawner(GetSpawner(SagaRedTeamName));
	}
	break;

	default:
	{
#if WITH_EDITOR

		UE_LOG(LogSagaGame, Warning, TEXT("Assigning a player spawner for unknown team..."));
#endif

		sys->AssignLocalPlayerSpawner(world->GetWorldSettings());
	}
	break;
	}

	// Find the local controller
	// At first, there is only an ASagaInGamePlayerController
	for (TActorIterator<ASagaInGamePlayerController> it{ world }; it; ++it)
	{
		const auto controller = *it;

		storedLocalPlayerController = controller;
		storedLocalPlayerController->SetAsLocalPlayerController();
	}

	int32 item_spawner_id = 0;

	// Seek and store every item spawner
	for (TActorIterator<AMapObstacle1> it{ world }; it; ++it)
	{
		const auto entity = *it;

		if (entity->GetID() < 0)
		{
			entity->SetID(item_spawner_id++);
		}

		everyItemSpawnEntities.Add(entity);
	}
	
	// Seek and store every item box
	for (TActorIterator<ASagaItemBox> it{ world }; it; ++it)
	{
		const auto box = *it;

		if (box->GetItemId() < 0)
		{
			box->SetItemId(item_spawner_id++);
		}

		everyItemBoxes.Add(box);
	}

#if WITH_EDITOR

	const auto item_entity_num = everyItemSpawnEntities.Num() + everyItemBoxes.Num();
	if (0 < item_entity_num)
	{
		UE_LOG(LogSagaGame, Log, TEXT("[ASagaInGameMode][StartPlay] %d item spawners are detected."), item_entity_num);
	}
	else
	{
		UE_LOG(LogSagaGame, Warning, TEXT("[ASagaInGameMode][StartPlay] There is no item spawner."));
	}
#endif

	// Preload effects
	//const auto effect = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Script/Niagara.NiagaraSystem'/Game/Item/VFX/NS_Smoke.NS_Smoke'"));

	if (net->IsOfflineMode())
	{
		OnCreatingCharacter(net->GetLocalUserId(), net->GetLocalUserTeam(), net->GetLocalUserWeapon());
	}
	else
	{
		UE_LOG(LogSagaGame, Log, TEXT("[ASagaInGameMode][StartPlay] Game mode would send a loaded packet"));

		net->SendGameIsLoadedPacket();

		net->SendRpcPacket(ESagaRpcProtocol::RPC_ASSIGN_ITEM_ID, 0, everyItemSpawnEntities.Num());
	}

	// First find the local controller and enumerate actors' BeginPlay
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(guardianScannerTimer, this, &ASagaInGameMode::ScanGuardians, 1.0f, false);
}

AActor*
ASagaInGameMode::ChoosePlayerStart_Implementation(AController* player)
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	switch (net->GetLocalUserTeam())
	{
	case ESagaPlayerTeam::Red:
	{
#if WITH_EDITOR

		UE_LOG(LogSagaGame, Log, TEXT("Choosing a player spawner for red team..."));
#endif

		return GetSpawner(SagaRedTeamName);
	}

	case ESagaPlayerTeam::Blue:
	{
#if WITH_EDITOR

		UE_LOG(LogSagaGame, Log, TEXT("Choosing a player spawner for blue team..."));
#endif

		return GetSpawner(SagaBluTeamName);
	}

	default:
	{
#if WITH_EDITOR

		UE_LOG(LogSagaGame, Warning, TEXT("Choosing a player spawner for unknown team..."));
#endif

		return Super::ChoosePlayerStart_Implementation(player);
	}
	}
}

AActor*
ASagaInGameMode::FindPlayerStart_Implementation(AController* player, const FString& tag)
{
#if WITH_EDITOR

	UE_LOG(LogSagaGame, Log, TEXT("Finding a player spawn by a tag '%s'..."), *tag);
#endif

	return Super::FindPlayerStart_Implementation(player, tag);
}

void
ASagaInGameMode::EndPlay(const EEndPlayReason::Type reason)
{
	Super::EndPlay(reason);
}

AActor*
ASagaInGameMode::GetSpawnerBy(ESagaPlayerTeam team)
const
{
	switch (team)
	{
	case ESagaPlayerTeam::Unknown:
	case ESagaPlayerTeam::Red:
	{
		return GetSpawner(SagaRedTeamName);
	}

	case ESagaPlayerTeam::Blue:
	{
		return GetSpawner(SagaBluTeamName);
	}

	default:
	{
		return nullptr;
	}
	}
}

AActor*
ASagaInGameMode::GetSpawner(const FString& team_name)
const
{
	for (TActorIterator<ASagaCharacterSpawner> It{ GetWorld() }; It; ++It)
	{
		const auto starter = *It;
		if (starter && starter->PlayerStartTag == team_name)
		{
			return starter;
		}
	}

	return nullptr;
}

void
ASagaInGameMode::HandleUpdateTransform()
{
	const auto world = GetWorld();
	const auto net = USagaNetworkSubSystem::GetSubSystem(world);

	if (net->IsConnected())
	{
		const auto controller = world->GetFirstPlayerController<ASagaInGamePlayerController>();
		const auto pawn = controller->GetPawn<ASagaCharacterBase>();

		if (pawn->IsAlive())
		{
			const auto loc = pawn->GetActorLocation();
			if (lastCharacterPosition != loc)
			{
				int64 arg0{};
				int32 arg1{};

				SerializePosition(loc, arg0, arg1);

				//net->SendPositionPacket(loc.X, loc.Y, loc.Z);
				net->SendRpcPacket(ESagaRpcProtocol::RPC_POSITION, arg0, arg1);

				lastCharacterPosition = loc;
			}

			const auto rot = pawn->GetActorRotation();
			if (lastCharacterRotation != rot)
			{
				int64 arg0{};
				int32 arg1{};

				SerializePosition(FVector{ rot.Pitch, rot.Yaw, rot.Roll }, arg0, arg1);

				//net->SendRotationPacket(rot.Pitch, rot.Yaw, rot.Roll);
				net->SendRpcPacket(ESagaRpcProtocol::RPC_ROTATION, arg0, arg1);

				lastCharacterRotation = rot;
			}
		}
	}
	else
	{
		GetWorldTimerManager().ClearTimer(transformUpdateTimer);
	}
}

void
ASagaInGameMode::ScanGuardians()
{
	int32 bear_id = 0;

	// Seek and store every gummy bear
	for (TActorIterator<ASagaGummyBearPlayer> it{ GetWorld() }; it; ++it)
	{
		const auto bear = *it;

		//NOTICE: 조건 달면 안됨. 달려면 곰도 수정해야 함
		//if (bear->bearUniqueId < 0)
		{
			//bear->bearUniqueId = bear_id++;
		}

		everyBears.Add(bear);
	}

#if WITH_EDITOR

	const auto bear_num = everyBears.Num();
	if (0 < bear_num)
	{
		UE_LOG(LogSagaGame, Log, TEXT("[ASagaInGameMode][StartPlay] %d bears are detected."), bear_num);
	}
	else
	{
		UE_LOG(LogSagaGame, Error, TEXT("[ASagaInGameMode][StartPlay] There is no bear."));
	}
#endif
}

void
SerializePosition(const FVector& vector, int64& arg0, int32& arg1)
{
	const auto x = static_cast<float>(vector.X);
	const auto y = static_cast<float>(vector.Y);
	const auto z = static_cast<float>(vector.Z);

	memcpy(reinterpret_cast<char*>(&arg0), &x, 4);
	memcpy(reinterpret_cast<char*>(&arg0) + 4, &y, 4);
	memcpy(reinterpret_cast<char*>(&arg1), &z, 4);
}

FVector
DeserializePosition(const int64& arg0, const int32& arg1)
{
	float x{};
	float y{};
	float z{};

	std::memcpy(&x, &arg0, 4);
	std::memcpy(&y, reinterpret_cast<const char*>(&arg0) + 4, 4);
	std::memcpy(&z, &arg1, 4);

	return FVector{ x, y, z };
}
