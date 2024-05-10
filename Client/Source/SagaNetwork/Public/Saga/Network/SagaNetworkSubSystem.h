#pragma once
#include "SagaNetwork.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Async/Async.h"

#include "Saga/Network/SagaPacketProtocol.h"
#include "Saga/Network/SagaConnectionContract.h"
#include "Saga/Network/SagaGameContract.h"
#include "Saga/Network/SagaVirtualRoom.h"
#include "Saga/Network/SagaVirtualUser.h"
#include "Saga/Network/SagaRpcProtocol.h"

#include "SagaGame/Player/SagaUserTeam.h"
#include "SagaGame/Player/SagaPlayerWeaponTypes.h"
#include "SagaGame/Character/SagaPlayableCharacter.h"
#include "SagaGame/Character/CharacterSelect/SagaSelectCharacter.h"

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSagaEventOnUpdatePosition, int32, id, float, x, float, y, float, z);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSagaEventOnUpdateRotation, int32, id, float, r, float, y, float, p);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSagaEventOnCreatingCharacter, int32, user_id, EUserTeam, team, UClass*, character_class);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSagaEventOnRpc, ESagaRpcProtocol, category, int32, id, int64, arg0, int32, arg1);

UCLASS(Category = "CandyLandSaga|Network")
class SAGANETWORK_API USagaNetworkSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USagaNetworkSubSystem();

	/* State Machines */
#pragma region =========================
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
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

	/* Overall Clients Methods */
#pragma region =========================
	UFUNCTION(Category = "CandyLandSaga|Network|Session")
	void AddUser(const FSagaVirtualUser& client);
	UFUNCTION(Category = "CandyLandSaga|Network|Session")
	bool RemoveUser(int32 id) noexcept;
	UFUNCTION(Category = "CandyLandSaga|Network|Session")
	void ClearUserList() noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	const TArray<FSagaVirtualUser>& GetUserList() const noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	bool FindUser(int32 id, FSagaVirtualUser& outpin) const noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	bool HasUser(int32 id) const noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetCharacterHandle(int32 user_id, class ASagaCharacterPlayer* character) noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetTeam(int32 user_id, const EUserTeam& team) noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetWeapon(int32 user_id, EPlayerWeapon weapon) noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetHealth(int32 user_id, const float hp) noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	class ASagaCharacterPlayer* GetCharacterHandle(int32 user_id) const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	bool GetTeam(int32 user_id, EUserTeam& outpin) const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	bool GetWeapon(int32 user_id, EPlayerWeapon& outpin) const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	float GetHealth(int32 user_id) const noexcept;
#pragma endregion

	/* Overall Rooms Methods */
#pragma region =========================
	UFUNCTION(Category = "CandyLandSaga|Network|Session")
	void AddRoom(const FSagaVirtualRoom& room);
	UFUNCTION(Category = "CandyLandSaga|Network|Session")
	bool RemoveRoom(int32 id) noexcept;
	UFUNCTION(Category = "CandyLandSaga|Network|Session")
	void ClearRoomList() noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	const TArray<FSagaVirtualRoom>& GetRoomList() const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	bool RoomAt(int32 index, FSagaVirtualRoom& outpin) noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	bool FindRoom(int32 id, FSagaVirtualRoom& outpin) const noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	bool HasRoom(int32 user_id) const noexcept;
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
	bool GetLocalUserTeam(EUserTeam& outpin) const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetCurrentRoomId(int32 id) noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	int32 GetCurrentRoomId() const noexcept;
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Session")
	void SetCurrentRoomTitle(const FString& title);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network|Session")
	FString GetCurrentRoomTitle() const;
#pragma endregion

	/* Getters */
#pragma region =========================
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network", meta = (UnsafeDuringActorConstruction))
	static USagaNetworkSubSystem* GetSubSystem(const UWorld* world) noexcept;
#pragma endregion

	/* Observers */
#pragma region =========================
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network")
	bool IsSocketAvailable() const noexcept;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network")
	bool IsConnected() const noexcept;
#pragma endregion

	/* Network Methods */
#pragma region =========================
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network", meta = (UnsafeDuringActorConstruction))
	bool ConnectToServer(const FString& nickname);

	UFUNCTION(Category = "CandyLandSaga|Network", meta = (UnsafeDuringActorConstruction, Latent))
	bool Receive();

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network", meta = (UnsafeDuringActorConstruction))
	bool Close();

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network", meta = (UnsafeDuringActorConstruction, Latent))
	const TArray<FSagaVirtualUser>& UpdatePlayerList();
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network", meta = (UnsafeDuringActorConstruction, Latent))
	const TArray<FSagaVirtualRoom>& UpdateRoomList();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|Network", CallInEditor)
	static bool IsOfflineMode() noexcept;
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
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Packet", meta = (UnsafeDuringActorConstruction))
	int32 SendRotationPacket(float r, float y, float p);
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Packet", meta = (UnsafeDuringActorConstruction))
	int32 SendRpcPacket(ESagaRpcProtocol category, int64 argument0 = 0, int32 argument1 = 0);
	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network|Packet", meta = (UnsafeDuringActorConstruction))
	int32 SendRoomUpdaterPacket();
#pragma endregion

	/* Public Properties */
#pragma region =========================
#pragma endregion

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
	FSagaEventOnCreatingCharacter OnCreatingCharacter;

	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnUpdatePosition OnUpdatePosition;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnUpdateRotation OnUpdateRotation;
	UPROPERTY(BlueprintAssignable, Category = "CandyLandSaga|Network")
	FSagaEventOnRpc OnRpc;
#pragma endregion

public:
	EPlayerWeapon CurrentPlayerWeapon;

public:
	EPlayerWeapon GetWeaponType() const
	{
		return CurrentPlayerWeapon;
	}

	void SetWeaponType(EPlayerWeapon Type)
	{
		CurrentPlayerWeapon = Type;
	}

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Network", meta = (NotBlueprintThreadSafe, UnsafeDuringActorConstruction))
	UClass* GetPlayableCharacterClass(const int32& user_id) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentMode = 1;

	UFUNCTION(BlueprintCallable)
	void SetCurrentMode(int32 Mode)
	{
		CurrentMode = Mode;
	}

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentMode() const
		{
		return CurrentMode;
	}

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 RedTeamScore = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 BlueTeamScore = 0;

	UFUNCTION(BlueprintCallable)
	int32 GetRedTeamScore() const
	{
		return RedTeamScore;
	}

	UFUNCTION(BlueprintCallable)
	int32 GetBlueTeamScore() const
	{
		return BlueTeamScore;
	}

	void AddScore(EUserTeam team, int32 score)
	{
		if (team == EUserTeam::Red)
		{
			RedTeamScore += score;
		}
		else
		{
			BlueTeamScore += score;
		}
	}

	UFUNCTION(BlueprintCallable)
	FString GetWhoWon()
	{
		EUserTeam team;
		GetLocalUserTeam(team);

		if (RedTeamScore > BlueTeamScore)
		{
			if (team == EUserTeam::Red)
			{
				return "승리!"; // I'm Red Team And My team Win
			}
			else
			{
				return "패배"; // I'm Blue Team And My team Lose
			}
		}
		else if (RedTeamScore < BlueTeamScore)
		{
			if (team == EUserTeam::Red)
			{
				return "패배"; // I'm Red Team And My team Lose
			}
			else
			{
				return "승리!"; // I'm Blue Team And My team Win
			}
		}
		else
		{
			return "무승부"; // Draw
		}
	}


private:
	/* Internal Functions */
#pragma region =========================
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	bool InitializeNetwork_Implementation();
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	ESagaConnectionContract ConnectToServer_Implementation();
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	bool CloseNetwork_Implementation();

	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void RouteEvents(const TArray<uint8>& packet_buffer, const int32& offset, EPacketProtocol protocol, int16 packet_size);
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnNetworkInitialized_Implementation(bool succeed);
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnConnected_Implementation();
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnFailedToConnect_Implementation(ESagaConnectionContract reason);
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnDisconnected_Implementation();
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnRoomCreated_Implementation(int32 id);
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnJoinedRoom_Implementation(int32 id);
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnLeftRoomBySelf_Implementation();
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnLeftRoom_Implementation(int32 id);
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnRespondVersion_Implementation(const FString& version_string);
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnUpdateRoomList_Implementation(UPARAM(ref) const TArray<FSagaVirtualRoom>& list);
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnUpdateMembers_Implementation(UPARAM(ref) const TArray<FSagaVirtualUser>& list);
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnTeamChanged_Implementation(int32 user_id, bool is_red_team);
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnGetPreparedGame_Implementation();
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnFailedToStartGame_Implementation(ESagaGameContract reason);
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnStartGame_Implementation();
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnUpdatePosition_Implementation(int32 id, float x, float y, float z);
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnUpdateRotation_Implementation(int32 id, float p, float y, float r);
	UFUNCTION(meta = (NotBlueprintThreadSafe))
	void OnRpc_Implementation(ESagaRpcProtocol cat, int32 id, int64 arg0, int32 arg1);
#pragma endregion

	/* Event Broadcasting Methods */
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
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnCreatingCharacter(int32 user_id, EUserTeam team, UClass* character_class) const;
	UFUNCTION(Category = "CandyLandSaga|Network|Internal", meta = (BlueprintInternalUseOnly, UnsafeDuringActorConstruction, NotBlueprintThreadSafe))
	void BroadcastOnRpc(ESagaRpcProtocol cat, int32 user_id, int64 arg0, int32 arg1) const;
#pragma endregion

	/* Managed Network Properties */
#pragma region =========================
	FSocket* clientSocket;
	class FSagaNetworkWorker* netWorker;
	class FSagaTaskWorker* taskWorker;

	static inline constexpr int32 recvLimit = 512;
	UPROPERTY(VisibleInstanceOnly)
	TArray<uint8> recvBuffer;
	UPROPERTY(VisibleInstanceOnly)
	int32 recvBytes;
	UPROPERTY(VisibleInstanceOnly)
	TArray<uint8> transitBuffer;
	UPROPERTY(VisibleInstanceOnly)
	int32 transitOffset;
	FCriticalSection receivedDataLock;
#pragma endregion

	/// <remarks>로컬 플레이어도 포함</remarks>
	UPROPERTY()
	TArray<FSagaVirtualUser> everyUsers;
	UPROPERTY()
	TArray<FSagaVirtualRoom> everyRooms;
	TAtomic<bool> wasUsersUpdated;
	TAtomic<bool> wasRoomsUpdated;
};
