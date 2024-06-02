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

	system->OnNetworkInitialized.SagaAddDelegate(actor, ISagaNetworkView::OnNetworkInitialized_Implementation, bool);
	system->OnConnected.SagaAddDelegate(actor, ISagaNetworkView::OnConnected_Implementation, void);
	system->OnFailedToConnect.SagaAddDelegate(actor, ISagaNetworkView::OnFailedToConnect_Implementation, ESagaConnectionContract);
	system->OnDisconnected.SagaAddDelegate(actor, ISagaNetworkView::OnDisconnected_Implementation, void);

	system->OnRoomCreated.SagaAddDelegate(actor, ISagaNetworkView::OnRoomCreated_Implementation, int32);
	system->OnJoinedRoom.SagaAddDelegate(actor, ISagaNetworkView::OnJoinedRoom_Implementation, int32);
	system->OnLeftRoomBySelf.SagaAddDelegate(actor, ISagaNetworkView::OnLeftRoomBySelf_Implementation, void);
	system->OnLeftRoom.SagaAddDelegate(actor, ISagaNetworkView::OnLeftRoom_Implementation, int32);

	system->OnRespondVersion.SagaAddDelegate(actor, ISagaNetworkView::OnRespondVersion_Implementation, const FString&);
	system->OnUpdateRoomList.SagaAddDelegate(actor, ISagaNetworkView::OnUpdateRoomList_Implementation, const TArray<FSagaVirtualRoom>&);
	system->OnUpdateMembers.SagaAddDelegate(actor, ISagaNetworkView::OnUpdateMembers_Implementation, const TArray<FSagaVirtualUser>&);
	system->OnTeamChanged.SagaAddDelegate(actor, ISagaNetworkView::OnTeamChanged_Implementation, int32, bool);

	system->OnFailedToStartGame.SagaAddDelegate(actor, ISagaNetworkView::OnFailedToStartGame_Implementation, ESagaGameContract);
	system->OnGetPreparedGame.SagaAddDelegate(actor, ISagaNetworkView::OnBeginPrepareGame_Implementation, void);
	system->OnGameStarted.SagaAddDelegate(actor, ISagaNetworkView::OnGameStarted_Implementation, void);
	system->OnCreatingCharacter.SagaAddDelegate(actor, ISagaNetworkView::OnCreatePlayerCharacter_Implementation, int32, EUserTeam, EPlayerWeapon);

	system->OnRpc.SagaAddDelegate(actor, ISagaNetworkView::OnRpc, ESagaRpcProtocol, int32, int64, int32);
}

#define SagaRemoveDelegate(inst, method_name) Remove(inst, FName{ TEXT(#method_name) })

void
ISagaNetworkView::DeregisterView(USagaNetworkSubSystem* system, TScriptInterface<ISagaNetworkView> target)
{
	const auto actor = target.GetObject();

	system->OnNetworkInitialized.SagaRemoveDelegate(actor, ISagaNetworkView::OnNetworkInitialized_Implementation);
	system->OnConnected.SagaRemoveDelegate(actor, ISagaNetworkView::OnConnected_Implementation);
	system->OnFailedToConnect.SagaRemoveDelegate(actor, ISagaNetworkView::OnFailedToConnect_Implementation);
	system->OnDisconnected.SagaRemoveDelegate(actor, ISagaNetworkView::OnDisconnected_Implementation);

	system->OnRoomCreated.SagaRemoveDelegate(actor, ISagaNetworkView::OnRoomCreated_Implementation);
	system->OnJoinedRoom.SagaRemoveDelegate(actor, ISagaNetworkView::OnJoinedRoom_Implementation);
	system->OnLeftRoomBySelf.SagaRemoveDelegate(actor, ISagaNetworkView::OnLeftRoomBySelf_Implementation);
	system->OnLeftRoom.SagaRemoveDelegate(actor, ISagaNetworkView::OnLeftRoom_Implementation);

	system->OnRespondVersion.SagaRemoveDelegate(actor, ISagaNetworkView::OnRespondVersion_Implementation);
	system->OnUpdateRoomList.SagaRemoveDelegate(actor, ISagaNetworkView::OnUpdateRoomList_Implementation);
	system->OnUpdateMembers.SagaRemoveDelegate(actor, ISagaNetworkView::OnUpdateMembers_Implementation);
	system->OnTeamChanged.SagaRemoveDelegate(actor, ISagaNetworkView::OnTeamChanged_Implementation);

	system->OnFailedToStartGame.SagaRemoveDelegate(actor, ISagaNetworkView::OnFailedToStartGame_Implementation);
	system->OnGetPreparedGame.SagaRemoveDelegate(actor, ISagaNetworkView::OnBeginPrepareGame_Implementation);
	system->OnGameStarted.SagaRemoveDelegate(actor, ISagaNetworkView::OnGameStarted_Implementation);
	system->OnCreatingCharacter.SagaRemoveDelegate(actor, ISagaNetworkView::OnCreatePlayerCharacter_Implementation);

	system->OnRpc.SagaRemoveDelegate(actor, ISagaNetworkView::OnRpc_Implementation);
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
