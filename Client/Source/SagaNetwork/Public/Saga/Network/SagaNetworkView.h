#pragma once
#include "SagaNetwork.h"
#include <UObject/Interface.h>

#include "Saga/Network/SagaConnectionContract.h"
#include "Saga/Network/SagaGameContract.h"
#include "Saga/Network/SagaRpcProtocol.h"
#include "SagaNetworkView.generated.h"

enum class [[nodiscard]] ESagaPlayerTeam : uint8;
enum class [[nodiscard]] EPlayerWeapon : uint8;

UINTERFACE(BlueprintType, Blueprintable, MinimalAPI)
class USagaNetworkView : public UInterface
{
	GENERATED_BODY()
};

class SAGANETWORK_API ISagaNetworkView
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "CandyLandSaga|Network|Event")
	void OnNetworkInitialized(bool succeed);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnConnected();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnFailedToConnect(ESagaConnectionContract reason);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnDisconnected();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnSignedIn(int32 my_id, const FName& nickname);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnRoomCreated(int32 room_id);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnJoinedRoom(int32 room_or_user_id);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnLeftRoomBySelf();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnLeftRoom(int32 user_id);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnRespondVersion(const FString& version_string);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnUpdateRoomList(UPARAM(ref) const TArray<struct FSagaVirtualRoom>& list);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnUpdateMembers(UPARAM(ref) const TArray<struct FSagaVirtualUser>& list);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnTeamChanged(int32 user_id, bool is_red_team);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnFailedToStartGame(ESagaGameContract reason);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnBeginPrepareGame();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnGameStarted();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnCreatePlayerCharacter(int32 user_id, ESagaPlayerTeam team, EPlayerWeapon weapon);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnUpdatePosition(int32 id, float x, float y, float z);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnUpdateRotation(int32 id, float roll, float yaw, float pitch);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CandyLandSaga|Network|Event")
	void OnRpc(ESagaRpcProtocol cat, int32 user_id, int64 arg0, int32 arg1);
};
