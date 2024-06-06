#include "Saga/Network/SagaNetworkView.h"
#include <UObject/Object.h>
#include <Delegates/Delegate.h>
#include <Templates/Casts.h>

#include "Saga/Network/SagaConnectionContract.h"
#include "Saga/Network/SagaGameContract.h"
#include "Saga/Network/SagaVirtualRoom.h"
#include "Saga/Network/SagaVirtualUser.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

#include "SagaGame/Player/SagaPlayerTeam.h"
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

	net->OnNetworkInitialized.SagaAddDelegate(actor, ISagaNetworkView::OnNetworkInitialized_Implementation, bool);
	net->OnConnected.SagaAddDelegate(actor, ISagaNetworkView::OnConnected_Implementation, void);
	net->OnFailedToConnect.SagaAddDelegate(actor, ISagaNetworkView::OnFailedToConnect_Implementation, ESagaConnectionContract);
	net->OnDisconnected.SagaAddDelegate(actor, ISagaNetworkView::OnDisconnected_Implementation, void);

	net->OnRoomCreated.SagaAddDelegate(actor, ISagaNetworkView::OnRoomCreated_Implementation, int32);
	net->OnJoinedRoom.SagaAddDelegate(actor, ISagaNetworkView::OnJoinedRoom_Implementation, int32);
	net->OnLeftRoomBySelf.SagaAddDelegate(actor, ISagaNetworkView::OnLeftRoomBySelf_Implementation, void);
	net->OnLeftRoom.SagaAddDelegate(actor, ISagaNetworkView::OnLeftRoom_Implementation, int32);

	net->OnRespondVersion.SagaAddDelegate(actor, ISagaNetworkView::OnRespondVersion_Implementation, const FString&);
	net->OnUpdateRoomList.SagaAddDelegate(actor, ISagaNetworkView::OnUpdateRoomList_Implementation, const TArray<FSagaVirtualRoom>&);
	net->OnUpdateMembers.SagaAddDelegate(actor, ISagaNetworkView::OnUpdateMembers_Implementation, const TArray<FSagaVirtualUser>&);
	net->OnTeamChanged.SagaAddDelegate(actor, ISagaNetworkView::OnTeamChanged_Implementation, int32, bool);

	net->OnFailedToStartGame.SagaAddDelegate(actor, ISagaNetworkView::OnFailedToStartGame_Implementation, ESagaGameContract);
	net->OnGetPreparedGame.SagaAddDelegate(actor, ISagaNetworkView::OnBeginPrepareGame_Implementation, void);
	net->OnGameStarted.SagaAddDelegate(actor, ISagaNetworkView::OnGameStarted_Implementation, void);
	net->OnCreatingCharacter.SagaAddDelegate(actor, ISagaNetworkView::OnCreatePlayerCharacter_Implementation, int32, ESagaPlayerTeam, EPlayerWeapon);

	net->OnRpc.SagaAddDelegate(actor, ISagaNetworkView::OnRpc, ESagaRpcProtocol, int32, int64, int32);
}

#define SagaRemoveDelegate(inst, method_name) Remove(inst, FName{ TEXT(#method_name) })

void
ISagaNetworkView::DeregisterView(USagaNetworkSubSystem* system, TScriptInterface<ISagaNetworkView> target)
{
	const auto actor = target.GetObject();

	net->OnNetworkInitialized.SagaRemoveDelegate(actor, ISagaNetworkView::OnNetworkInitialized_Implementation);
	net->OnConnected.SagaRemoveDelegate(actor, ISagaNetworkView::OnConnected_Implementation);
	net->OnFailedToConnect.SagaRemoveDelegate(actor, ISagaNetworkView::OnFailedToConnect_Implementation);
	net->OnDisconnected.SagaRemoveDelegate(actor, ISagaNetworkView::OnDisconnected_Implementation);

	net->OnRoomCreated.SagaRemoveDelegate(actor, ISagaNetworkView::OnRoomCreated_Implementation);
	net->OnJoinedRoom.SagaRemoveDelegate(actor, ISagaNetworkView::OnJoinedRoom_Implementation);
	net->OnLeftRoomBySelf.SagaRemoveDelegate(actor, ISagaNetworkView::OnLeftRoomBySelf_Implementation);
	net->OnLeftRoom.SagaRemoveDelegate(actor, ISagaNetworkView::OnLeftRoom_Implementation);

	net->OnRespondVersion.SagaRemoveDelegate(actor, ISagaNetworkView::OnRespondVersion_Implementation);
	net->OnUpdateRoomList.SagaRemoveDelegate(actor, ISagaNetworkView::OnUpdateRoomList_Implementation);
	net->OnUpdateMembers.SagaRemoveDelegate(actor, ISagaNetworkView::OnUpdateMembers_Implementation);
	net->OnTeamChanged.SagaRemoveDelegate(actor, ISagaNetworkView::OnTeamChanged_Implementation);

	net->OnFailedToStartGame.SagaRemoveDelegate(actor, ISagaNetworkView::OnFailedToStartGame_Implementation);
	net->OnGetPreparedGame.SagaRemoveDelegate(actor, ISagaNetworkView::OnBeginPrepareGame_Implementation);
	net->OnGameStarted.SagaRemoveDelegate(actor, ISagaNetworkView::OnGameStarted_Implementation);
	net->OnCreatingCharacter.SagaRemoveDelegate(actor, ISagaNetworkView::OnCreatePlayerCharacter_Implementation);

	net->OnRpc.SagaRemoveDelegate(actor, ISagaNetworkView::OnRpc_Implementation);
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

	const auto net = USagaNetworkSubSystem::GetSubSystem(world);
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

	const auto net = USagaNetworkSubSystem::GetSubSystem(world);
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
