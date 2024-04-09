#pragma once
#include <cstddef>
#include "SagaNetwork.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "Tasks/Task.h"
#include "Async/Async.h"

#include "Saga/Network/SagaPacketProtocol.h"
#include "Saga/Network/SagaConnectionContract.h"
#include "Saga/Network/SagaGameContract.h"
#include "Saga/Network/SagaVirtualRoom.h"
#include "Saga/Network/SagaVirtualUser.h"
#include "SagaNetworkSubSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnNetworkInitialized, bool, was_succeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSagaEventOnConnected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnFailedToConnect, ESagaConnectionContract, reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSagaEventOnDisconnected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnRoomCreated, int32, room_id);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnJoinedRoom, int32, room_id);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSagaEventOnOtherJoinedRoom, int32, user_id, const FString&, nickname, const EUserTeam&, team);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSagaEventOnLeftRoomBySelf);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnLeftRoom, int32, id);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnRespondVersion, const FString&, version_string);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnUpdateRoomList, const TArray<FSagaVirtualRoom>&, list);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnUpdateUserList, const TArray<FSagaVirtualUser>&, list);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSagaEventOnTeamChanged, int32, user_id, bool, is_red_team);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnFailedToStartGame, ESagaGameContract, reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSagaEventOnGetPreparedGame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSagaEventOnStartGame);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnRpc, int64, argument);
DECLARE_EVENT_OneParam(FSagaNetworkWorker, FSagaEventOnRpc, int64);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSagaEventOnUpdatePosition, int32, id, float, x, float, y, float, z);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSagaEventOnUpdateRotation, int32, id, float, r, float, y, float, p);

class SAGANETWORK_API FSagaNetworkWorker final : public FRunnable, public FNoncopyable
{
public:
	FSagaNetworkWorker(TObjectPtr<USagaNetworkSubSystem> instance);
	~FSagaNetworkWorker();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	virtual void Stop() override;

private:
	FRunnableThread* MyThread;
	TObjectPtr<USagaNetworkSubSystem> SubSystemInstance;
};

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Network")
class SAGANETWORK_API USagaNetworkSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USagaNetworkSubSystem();

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/* State Machines */
#pragma region =========================
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network", meta = (UnsafeDuringActorConstruction))
	bool Close();
#pragma endregion

	/* Complicated Network Methods */
#pragma region =========================
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Phase", meta = (UnsafeDuringActorConstruction))
	bool ConnectToServer(const FString& nickname);

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network", meta = (Latent, UnsafeDuringActorConstruction))
	void UpdatePlayerList();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network", meta = (Latent, UnsafeDuringActorConstruction))
	void UpdateRoomList();
#pragma endregion

	/* General Methods */
#pragma region =========================
	FORCEINLINE void CallFunctionOnGameThread(TUniqueFunction<void()>&& function)
	{
		if (IsInGameThread())
		{
			function();
		}
		else
		{
			AsyncTask(ENamedThreads::GameThread, MoveTemp(function));
		}
	}
	FORCEINLINE	void CallPureFunctionOnGameThread(TUniqueFunction<void()>&& function) const
	{
		if (IsInGameThread())
		{
			function();
		}
		else
		{
			AsyncTask(ENamedThreads::GameThread, MoveTemp(function));
		}
	}
#pragma endregion

	/* Local Client Methods */
#pragma region =========================
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetLocalUserId(int32 id) noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	int32 GetLocalUserId() const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetLocalUserName(const FString& nickname);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	FString GetLocalUserName() const;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetCurrentRoomId(int32 id) noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	int32 GetCurrentRoomId() const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetCurrentRoomTitle(const FString& title);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	FString GetCurrentRoomTitle() const;
#pragma endregion

	/* Overall Clients Methods */
#pragma region =========================
	void AddUser(const FSagaVirtualUser& client);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	bool FindUser(int32 id, FSagaVirtualUser& outpin) const noexcept;
	bool RemoveUser(int32 id) noexcept;
	void ClearUserList() noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	bool HasUser(int32 id) const noexcept;
#pragma endregion

	/* Overall Rooms Methods */
#pragma region =========================
	void AddRoom(const FSagaVirtualRoom& room);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	bool FindRoom(int32 id, FSagaVirtualRoom& outpin) const noexcept;
	bool RoomAt(int32 index, FSagaVirtualRoom& outpin) noexcept;
	bool RemoveRoom(int32 id) noexcept;
	void ClearRoomList() noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	bool HasRoom(int32 id) const noexcept;
#pragma endregion

	/* Getters */
#pragma region =========================
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session", meta = (UnsafeDuringActorConstruction))
	const TArray<FSagaVirtualUser>& GetUserList() const noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session", meta = (UnsafeDuringActorConstruction))
	const TArray<FSagaVirtualRoom>& GetRoomList() const noexcept;
#pragma endregion

	/* Observers */
#pragma region =========================
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network")
	bool IsSocketAvailable() const noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network")
	bool IsConnected() const noexcept;
#pragma endregion

	/* Packet Senders */
#pragma region =========================
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Packet", meta = (UnsafeDuringActorConstruction))
	int32 SendSignInPacket(const FString& nickname);
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Packet", meta = (UnsafeDuringActorConstruction))
	int32 SendCreateRoomPacket(const FString& title);
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Packet", meta = (UnsafeDuringActorConstruction))
	int32 SendJoinRoomPacket(int32 room_id);
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Packet", meta = (UnsafeDuringActorConstruction))
	int32 SendLeaveRoomPacket();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Packet", meta = (UnsafeDuringActorConstruction))
	int32 SendRequestVersionPacket();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Packet", meta = (UnsafeDuringActorConstruction))
	int32 SendRequestRoomsPacket();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Packet", meta = (UnsafeDuringActorConstruction))
	int32 SendRequestMembersPacket();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Packet", meta = (UnsafeDuringActorConstruction))
	int32 SendChangeTeamPacket(bool is_red_team);
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Packet", meta = (UnsafeDuringActorConstruction))
	int32 SendGameStartPacket();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Packet", meta = (UnsafeDuringActorConstruction))
	int32 SendGameIsLoadedPacket();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Packet", meta = (UnsafeDuringActorConstruction))
	int32 SendPositionPacket(float x, float y, float z);
#pragma endregion

	/* Internal Event Broadcasting Methods */

#pragma region =========================
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnNetworkInitialized() const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnFailedToInitializeNetwork() const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnConnected() const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnFailedToConnect(ESagaConnectionContract reason) const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnDisconnected() const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnRoomCreated(int32 room_id) const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnJoinedRoom(int32 user_id) const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnLeftRoomBySelf() const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnLeftRoom(int32 id) const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnRespondVersion(const FString& version_string) const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnUpdateRoomList(UPARAM(ref) const TArray<FSagaVirtualRoom>& list) const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnUpdateMembers(UPARAM(ref) const TArray<FSagaVirtualUser>& list) const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnTeamChanged(int32 id, bool is_red_team) const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnGetPreparedGame() const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnFailedToStartGame(ESagaGameContract reason) const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnStartGame() const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnUpdatePosition(int32 user_id, float x, float y, float z) const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnUpdateRotation(int32 user_id, float r, float y, float p) const;
#pragma endregion

	friend class FSagaNetworkWorker;

	/* Public Properties */
#pragma region =========================

	/* Local Session's Properties */
#pragma region =========================
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Network")
	int32 localUserId;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Network")
	FString localUserName;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Network")
	int32 currentRoomId;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Network")
	FString currentRoomTitle;
#pragma endregion

	/* Events */
#pragma region =========================
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnNetworkInitialized OnNetworkInitialized;
	//FDelegateHandle OnNetworkInitializedDelegateHandle;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnConnected OnConnected;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnFailedToConnect OnFailedToConnect;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnDisconnected OnDisconnected;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnRoomCreated OnRoomCreated;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnJoinedRoom OnJoinedRoom;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnOtherJoinedRoom OnOtherJoinedRoom;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnLeftRoomBySelf OnLeftRoomBySelf;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnLeftRoom OnLeftRoom;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnRespondVersion OnRespondVersion;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnUpdateRoomList OnUpdateRoomList;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnUpdateUserList OnUpdateMembers;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnTeamChanged OnTeamChanged;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnFailedToStartGame OnFailedToStartGame;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnGetPreparedGame OnGetPreparedGame;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnStartGame OnStartGame;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnUpdatePosition OnUpdatePosition;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnUpdateRotation OnUpdateRotation;
#pragma endregion

	/* Tasks */
#pragma region =========================
	void AddRpcCallback(const FString& id, FSagaEventOnRpc& delegate)
	{
		rpcDatabase.Add(id) = delegate;
	}

	static TQueue<UE::Tasks::TTask<int32>> taskQueue;
	FGraphEventArray TaskCompletionEvents;

	//static TMap<FStringView, TUniqueFunction<void()>> rpcDatabase;
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CandyLandSaga|Network")
	TMap<FString, FSagaEventOnRpc> rpcDatabase;

#pragma endregion

protected:
	/* Internal Functions */
#pragma region =========================
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	bool InitializeNetwork_Implementation();
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	ESagaConnectionContract ConnectToServer_Implementation();
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	bool CloseNetwork_Implementation();

	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnNetworkInitialized_Implementation(bool succeed);
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnConnected_Implementation();
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnFailedToConnect_Implementation(ESagaConnectionContract reason);
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnDisconnected_Implementation();
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnRoomCreated_Implementation(int32 id);
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnJoinedRoom_Implementation(int32 id);
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnLeftRoomBySelf_Implementation();
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnLeftRoom_Implementation(int32 id);
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnRespondVersion_Implementation(const FString& version_string);
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnUpdateRoomList_Implementation(UPARAM(ref) const TArray<FSagaVirtualRoom>& list);
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnUpdateMembers_Implementation(UPARAM(ref) const TArray<FSagaVirtualUser>& list);
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnTeamChanged_Implementation(int32 user_id, bool is_red_team);
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnGetPreparedGame_Implementation();
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnFailedToStartGame_Implementation(ESagaGameContract reason);
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnStartGame_Implementation();
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnUpdatePosition_Implementation(int32 id, float x, float y, float z);
	UFUNCTION(meta = (BlueprintInternalUseOnly, NotBlueprintThreadSafe))
	void OnUpdateRotation_Implementation(int32 id, float r, float y, float p);

	void RouteEvents(const std::byte* packet_buffer, EPacketProtocol protocol, int16 packet_size);
#pragma endregion

	FSocket* clientSocket;
	TUniquePtr<FSagaNetworkWorker> netWorker;

	/// <remarks>로컬 플레이어도 포함</remarks>
	TArray<FSagaVirtualUser> everyUsers;
	TAtomic<bool> wasUsersUpdated;
	TArray<FSagaVirtualRoom> everyRooms;
	TAtomic<bool> wasRoomsUpdated;
};
