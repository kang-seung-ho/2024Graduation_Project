#pragma once
#include "SagaNetwork.h"
#include <Subsystems/GameInstanceSubsystem.h>
#include <Async/Async.h>

// SagaUtility
#include <Saga/Containers/atomic_queue.h>
#include <Saga/Containers/barrier.h>

// SagaNetwork
#include "SagaNetworkConnectionCategory.h"
#include "SagaPacketProtocol.h"
#include "SagaConnectionContract.h"
#include "SagaGameContract.h"
#include "SagaVirtualRoom.h"
#include "SagaVirtualUser.h"
#include "SagaRpcProtocol.h"

// SagaGame
#include "SagaGame/Player/SagaPlayerTeam.h"
#include "SagaGame/Player/SagaPlayerWeaponTypes.h"

#include "SagaNetworkSubSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnNetworkInitialized, bool, was_succeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSagaEventOnConnected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnFailedToConnect, ESagaConnectionContract, reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSagaEventOnDisconnected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSagaEventOnSignedIn, int32, my_id, const FText&, nickname);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnRoomCreated, int32, room_id);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnJoinedRoom, int32, room_id);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSagaEventOnOtherJoinedRoom, int32, user_id, const FText&, nickname, const ESagaPlayerTeam&, team);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSagaEventOnLeftRoomBySelf);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnLeftRoom, int32, id);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnRespondVersion, const FString&, version_string);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnUpdateRoomList, const TArray<FSagaVirtualRoom>&, list);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnUpdateUserList, const TArray<FSagaVirtualUser>&, list);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSagaEventOnTeamChanged, int32, user_id, bool, is_red_team);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSagaEventOnFailedToStartGame, ESagaGameContract, reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSagaEventOnGetPreparedGame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSagaEventOnStartGame);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSagaEventOnUpdatePosition, int32, id, float, x, float, y, float, z);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSagaEventOnUpdateRotation, int32, id, float, r, float, y, float, p);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSagaEventOnCreatingCharacter, int32, user_id, ESagaPlayerTeam, team, EPlayerWeapon, weapon);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSagaEventOnRpc, ESagaRpcProtocol, category, int32, id, int64, arg0, int32, arg1);

UCLASS(Config = Game, Category = "CandyLandSaga|Network")
class SAGANETWORK_API USagaNetworkSubSystem final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	/* Network Setting */
#pragma region =========================
	UPROPERTY(BlueprintSetter = SetOfflineMode, BlueprintGetter = IsOfflineMode)
	bool netIsOfflineMode;
	UPROPERTY(BlueprintSetter = SetConnectionOption, BlueprintGetter = GetConnectionOption)
	ESagaNetworkConnectionCategory netConnectionCategory;
	UPROPERTY(BlueprintSetter = SetRemoteAddress, BlueprintGetter = GetRemoteAddress)
	FString netRemoteAddress;
	UPROPERTY(BlueprintSetter = SetRemotePort, BlueprintGetter = GetRemotePort)
	int32 netRemotePort;
	UPROPERTY(BlueprintSetter = SetLocalPort, BlueprintGetter = GetLocalPort)
	int32 netLocalPort;
#pragma endregion

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Network")
	FSagaVirtualUser localUser;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Network")
	FSagaVirtualRoom currentRoom;

	/* Events */
#pragma region =========================
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnNetworkInitialized OnNetworkInitialized;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnConnected OnConnected;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnFailedToConnect OnFailedToConnect;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnDisconnected OnDisconnected;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnSignedIn OnSignedIn;

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
	FSagaEventOnStartGame OnGameStarted;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnCreatingCharacter OnCreatingCharacter;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnUpdatePosition OnUpdatePosition;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnUpdateRotation OnUpdateRotation;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnRpc OnRpc;
#pragma endregion

	USagaNetworkSubSystem();

	/* State Machines */
#pragma region =========================
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
#pragma endregion

	/* Network Methods */
#pragma region =========================
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network", meta = (NotBlueprintThreadSafe, UnsafeDuringActorConstruction))
	ESagaConnectionContract ConnectToServer(const FText& nickname);
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network", meta = (NotBlueprintThreadSafe, UnsafeDuringActorConstruction))
	bool Close();

	UFUNCTION()
	bool Receive();
	UFUNCTION()
	bool ProcessPackets();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "CandyLandSaga|Network")
	void SetOfflineMode(bool flag) noexcept;
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "CandyLandSaga|Network")
	void SetConnectionOption(ESagaNetworkConnectionCategory category) noexcept;
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "CandyLandSaga|Network")
	void SetRemoteAddress(const FString& address);
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "CandyLandSaga|Network")
	void SetRemotePort(int32 port) noexcept;
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "CandyLandSaga|Network")
	void SetLocalPort(int32 port) noexcept;
#pragma endregion

	/* Packet Senders */
#pragma region =========================
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Packet", meta = (UnsafeDuringActorConstruction))
	int32 SendSignInPacket(const FText& nickname);
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Packet", meta = (UnsafeDuringActorConstruction))
	int32 SendCreateRoomPacket(const FText& title);
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
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Packet", meta = (UnsafeDuringActorConstruction))
	int32 SendRotationPacket(float r, float y, float p);
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Packet", meta = (UnsafeDuringActorConstruction))
	int32 SendRpcPacket(ESagaRpcProtocol category, int64 argument0 = 0, int32 argument1 = 0);
#pragma endregion

	/* Overall Clients Methods */
#pragma region =========================
	void AddUser(const FSagaVirtualUser& client);
	void AddUser(FSagaVirtualUser&& client);
	bool RemoveUser(int32 id) noexcept;
	void ClearUserList(bool removed_me_also) noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	const TArray<FSagaVirtualUser>& GetUserList() const noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	bool FindUser(int32 id, FSagaVirtualUser& outpin) const noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	bool HasUser(int32 id) const noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetCharacterHandle(int32 user_id, class ASagaCharacterBase* character) noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetTeam(int32 user_id, const ESagaPlayerTeam& team) noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetWeapon(int32 user_id, EPlayerWeapon weapon) noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetHealth(int32 user_id, const float hp) noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void StorePosition(int32 user_id, const double& x, const double& y, const double& z) noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	class ASagaCharacterBase* GetCharacterHandle(int32 user_id) const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	bool GetTeam(int32 user_id, ESagaPlayerTeam& outpin) const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	bool GetWeapon(int32 user_id, EPlayerWeapon& outpin) const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	float GetHealth(int32 user_id) const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	FVector GetStoredPosition(int32 user_id) const noexcept;
#pragma endregion

	/* Overall Rooms Methods */
#pragma region =========================
	void AddRoom(const FSagaVirtualRoom& room);
	void AddRoom(FSagaVirtualRoom&& room);
	bool RemoveRoom(int32 room_id) noexcept;
	void ClearRoomList() noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	const TArray<FSagaVirtualRoom>& GetRoomList() const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	bool RoomAt(int32 index, FSagaVirtualRoom& outpin) noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	bool FindRoom(int32 room_id, FSagaVirtualRoom& outpin) const noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	bool HasRoom(int32 room_id) const noexcept;
#pragma endregion

	/* Local Client Methods */
#pragma region =========================
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetLocalUserId(int32 user_id) noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	int32 GetLocalUserId() const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetLocalUserName(FText nickname);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	FText GetLocalUserName() const;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetLocalUserTeam(ESagaPlayerTeam team);
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	ESagaPlayerTeam GetLocalUserTeam() const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetLocalUserWeapon(EPlayerWeapon weapon) noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	EPlayerWeapon GetLocalUserWeapon() const noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetCurrentRoomId(int32 room_id) noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	int32 GetCurrentRoomId() const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetCurrentRoomTitle(FText title);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	FText GetCurrentRoomTitle() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	int32 GetCurrentRoomMemberCount() const noexcept;
#pragma endregion

	/* Observers */
#pragma region =========================
#pragma endregion

	/* Getters */
#pragma region =========================
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network", meta = (UnsafeDuringActorConstruction))
	static USagaNetworkSubSystem* GetSubSystem(const UWorld* world) noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, CallInEditor, Category = "CandyLandSaga|Network")
	ESagaNetworkConnectionCategory GetConnectionOption() const noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, CallInEditor, Category = "CandyLandSaga|Network")
	FString GetRemoteAddress() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, CallInEditor, Category = "CandyLandSaga|Network")
	int32 GetRemotePort() const noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, CallInEditor, Category = "CandyLandSaga|Network")
	int32 GetLocalPort() const noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, CallInEditor, Category = "CandyLandSaga|Network")
	bool IsOfflineMode() const noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, CallInEditor, Category = "CandyLandSaga|Network")
	bool IsSocketAvailable() const noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, CallInEditor, Category = "CandyLandSaga|Network")
	bool IsConnected() const noexcept;
#pragma endregion

private:
	/* Internal Functions */
#pragma region =========================
	UFUNCTION(meta = (NotBlueprintThreadSafe, UnsafeDuringActorConstruction))
	bool CloseNetwork_Implementation();

	UFUNCTION(meta = (NotBlueprintThreadSafe, UnsafeDuringActorConstruction))
	void RouteEvents(const TArray<uint8>& packet_buffer, const int32& offset, EPacketProtocol protocol, int16 packet_size);
	void RouteTasks(TUniquePtr<uint8[]>&& packet_buffer, EPacketProtocol protocol, int16 packet_size);

	[[nodiscard]] static FSocket* CreateSocket();
	[[nodiscard]] TSharedRef<FInternetAddr> CreateRemoteEndPoint(ESagaNetworkConnectionCategory category) const;
#pragma endregion

	/* Basic Event Implementations */
#pragma region =========================
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnNetworkInitialized_Implementation(bool succeed);
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnConnected_Implementation();
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnFailedToConnect_Implementation(ESagaConnectionContract reason);
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnDisconnected_Implementation();
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnFailedToStartGame_Implementation(ESagaGameContract reason);
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnRpc_Implementation(ESagaRpcProtocol cat, int32 user_id, int64 arg0, int32 arg1);
#pragma endregion

	/* Event Broadcaster Methods */
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
	void BroadcastOnSignedIn(int32 my_id, const FText& nickname) const;
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
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnCreatingCharacter(int32 user_id, ESagaPlayerTeam team, EPlayerWeapon weapon) const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnRpc(ESagaRpcProtocol cat, int32 user_id, int64 arg0, int32 arg1) const;
#pragma endregion

	/* Network Properties */
#pragma region =========================
	static inline constexpr int32 recvLimit = 512;

	FSocket* clientSocket;
	class FSagaNetworkWorker* netWorker;
	class FSagaTaskWorker* taskWorker;

	UPROPERTY(VisibleInstanceOnly)
	TArray<uint8> recvBuffer;
	UPROPERTY(VisibleInstanceOnly)
	int32 recvBytes;

	atomic_queue::AtomicQueueB2<TUniquePtr<uint8[]>> taskQueue{ 200 };
	atomic_queue::Barrier taskBarrier;
	atomic_queue::AtomicQueueB2<TUniquePtr<uint8[]>> delayedTaskQueue{ 100 };

	/// <remarks>로컬 플레이어도 포함</remarks>
	UPROPERTY()
	TArray<FSagaVirtualUser> everyUsers;
	UPROPERTY()
	TArray<FSagaVirtualRoom> everyRooms;
	TAtomic<bool> wasUsersUpdated;
	TAtomic<bool> wasRoomsUpdated;
#pragma endregion
};

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

FORCEINLINE void CallPureFunctionOnGameThread(TUniqueFunction<void()>&& function)
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
