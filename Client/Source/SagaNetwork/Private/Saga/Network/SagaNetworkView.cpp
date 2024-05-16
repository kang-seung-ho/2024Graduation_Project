#include "Saga/Network/SagaNetworkView.h"
#include <UObject/Object.h>
#include <Delegates/Delegate.h>
#include <Templates/Casts.h>

#include "Saga/Network/SagaConnectionContract.h"
#include "Saga/Network/SagaGameContract.h"
#include "Saga/Network/SagaVirtualRoom.h"
#include "Saga/Network/SagaVirtualUser.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

#include "SagaGame/Player/SagaUserTeam.h"
#include "SagaGame/Player/SagaPlayerWeaponTypes.h"

template<typename RetValType, typename... ParamTypes>
using FDelegate = TBaseDynamicDelegate<FNotThreadSafeDelegateMode, RetValType, ParamTypes...>;

template<typename RetValType, typename... ParamTypes>
auto
SagaMakeDelegate(UObject* const& inst, const FName& method_name)
{
	FDelegate<RetValType, ParamTypes...> delegate{};
	delegate.BindUFunction(inst, method_name);

	return MoveTempIfPossible(delegate);
}

#define SagadAddDelegate_Imp(inst, method, ...) SagaMakeDelegate<__VA_ARGS__>(inst, FName{ TEXT(#method) })

#define SagaAddDelegate(inst, method, ...) Add(SagadAddDelegate_Imp(inst, method, __VA_ARGS__))

#define SagaAddDelegate2(event, inst, method, ...) FDelegate<__VA_ARGS__> delegate_ ## inst ## event{}; \
(delegate_ ## inst ## event).BindUFunction(inst, FName{ TEXT(#method) }); \
event.Add(MoveTempIfPossible((delegate_ ## inst ## event)));

void
ISagaNetworkView::RegisterView(USagaNetworkSubSystem* system, TScriptInterface<ISagaNetworkView> target)
{
	const auto actor = target.GetObject();

	system->OnNetworkInitialized.SagaAddDelegate(actor, ISagaNetworkView::OnNetworkInitialized, bool);
	system->OnConnected.SagaAddDelegate(actor, ISagaNetworkView::OnConnected, void);
	system->OnFailedToConnect.SagaAddDelegate(actor, ISagaNetworkView::OnFailedToConnect, ESagaConnectionContract);
	system->OnDisconnected.SagaAddDelegate(actor, ISagaNetworkView::OnDisconnected, void);

	system->OnRoomCreated.SagaAddDelegate(actor, ISagaNetworkView::OnRoomCreated, int32);
	system->OnJoinedRoom.SagaAddDelegate(actor, ISagaNetworkView::OnJoinedRoom, int32);
	system->OnLeftRoomBySelf.SagaAddDelegate(actor, ISagaNetworkView::OnLeftRoomBySelf, void);
	system->OnLeftRoom.SagaAddDelegate(actor, ISagaNetworkView::OnLeftRoom, int32);

	system->OnRespondVersion.SagaAddDelegate(actor, ISagaNetworkView::OnRespondVersion, const FString&);
	system->OnUpdateRoomList.SagaAddDelegate(actor, ISagaNetworkView::OnUpdateRoomList, const TArray<FSagaVirtualRoom>&);
	system->OnUpdateMembers.SagaAddDelegate(actor, ISagaNetworkView::OnUpdateMembers, const TArray<FSagaVirtualUser>&);
	system->OnTeamChanged.SagaAddDelegate(actor, ISagaNetworkView::OnTeamChanged, int32, bool);

	system->OnFailedToStartGame.SagaAddDelegate(actor, ISagaNetworkView::OnFailedToStartGame, ESagaGameContract);
	system->OnGetPreparedGame.SagaAddDelegate(actor, ISagaNetworkView::OnBeginPrepareGame, void);
	system->OnStartGame.SagaAddDelegate(actor, ISagaNetworkView::OnGameStarted, void);
	system->OnCreatingCharacter.SagaAddDelegate(actor, ISagaNetworkView::OnCreatePlayerCharacter, int32, EUserTeam, EPlayerWeapon);

	system->OnRpc.SagaAddDelegate(actor, ISagaNetworkView::OnRpc, ESagaRpcProtocol, int32, int64, int32);
}

#define SagaRemoveDelegate(inst, method_name) Remove(inst, FName{ TEXT(#method_name) })

void
ISagaNetworkView::DeregisterView(USagaNetworkSubSystem* system, TScriptInterface<ISagaNetworkView> target)
{
	const auto actor = target.GetObject();

	system->OnNetworkInitialized.SagaRemoveDelegate(actor, ISagaNetworkView::OnNetworkInitialized);
	system->OnConnected.SagaRemoveDelegate(actor, ISagaNetworkView::OnConnected);
	system->OnFailedToConnect.SagaRemoveDelegate(actor, ISagaNetworkView::OnFailedToConnect);
	system->OnDisconnected.SagaRemoveDelegate(actor, ISagaNetworkView::OnDisconnected);

	system->OnRoomCreated.SagaRemoveDelegate(actor, ISagaNetworkView::OnRoomCreated);
	system->OnJoinedRoom.SagaRemoveDelegate(actor, ISagaNetworkView::OnJoinedRoom);
	system->OnLeftRoomBySelf.SagaRemoveDelegate(actor, ISagaNetworkView::OnLeftRoomBySelf);
	system->OnLeftRoom.SagaRemoveDelegate(actor, ISagaNetworkView::OnLeftRoom);

	system->OnRespondVersion.SagaRemoveDelegate(actor, ISagaNetworkView::OnRespondVersion);
	system->OnUpdateRoomList.SagaRemoveDelegate(actor, ISagaNetworkView::OnUpdateRoomList);
	system->OnUpdateMembers.SagaRemoveDelegate(actor, ISagaNetworkView::OnUpdateMembers);
	system->OnTeamChanged.SagaRemoveDelegate(actor, ISagaNetworkView::OnTeamChanged);

	system->OnFailedToStartGame.SagaRemoveDelegate(actor, ISagaNetworkView::OnFailedToStartGame);
	system->OnGetPreparedGame.SagaRemoveDelegate(actor, ISagaNetworkView::OnBeginPrepareGame);
	system->OnStartGame.SagaRemoveDelegate(actor, ISagaNetworkView::OnGameStarted);
	system->OnCreatingCharacter.SagaRemoveDelegate(actor, ISagaNetworkView::OnCreatePlayerCharacter);

	system->OnRpc.SagaRemoveDelegate(actor, ISagaNetworkView::OnRpc);
}

bool
ISagaNetworkView::TryRegisterView(const UWorld* world, TScriptInterface<ISagaNetworkView> target)
{
	if (not target or not IsValid(world))
	{
		return false;
	}

	const auto actor = target.GetObject();
	if (not IsValid(actor))
	{
		return false;
	}

	const auto uclass = actor->StaticClass();
	if (not uclass->ImplementsInterface(USagaNetworkView::StaticClass()))
	{
		return false;
	}

	const auto system = USagaNetworkSubSystem::GetSubSystem(world);
	if (IsValid(system))
	{
		RegisterView(system, actor);
		return true;
	}
	else
	{
		return false;
	}
}

bool
ISagaNetworkView::TryDeregisterView(const UWorld* world, TScriptInterface<ISagaNetworkView> target	)
{
	if (not target or not IsValid(world))
	{
		return false;
	}

	const auto actor = target.GetObject();
	if (not IsValid(actor))
	{
		return false;
	}

	const auto uclass = actor->StaticClass();
	if (not uclass->ImplementsInterface(USagaNetworkView::StaticClass()))
	{
		return false;
	}

	const auto system = USagaNetworkSubSystem::GetSubSystem(world);
	if (IsValid(system))
	{
		DeregisterView(system, actor);
		return true;
	}
	else
	{
		return false;
	}
}
