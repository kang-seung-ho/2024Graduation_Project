#include "Saga/Network/SagaNetworkSubSystem.h"

#include "SagaGame/Player/SagaPlayerTeam.h"
#include "SagaGame/Player/SagaPlayerWeaponTypes.h"
#include "Saga/Network/SagaGameContract.h"
#include "Saga/Network/SagaVirtualUser.h"
#include "Saga/Network/SagaPacketProcessor.h"

void
USagaNetworkSubSystem::RouteEvents(const TArray<uint8>& packet_buffer, const int32& offset
	, EPacketProtocol protocol, int16 packet_size)
{}

void
USagaNetworkSubSystem::RouteTasks(TUniquePtr<uint8[]>&& packet_buffer, EPacketProtocol protocol, int16 packet_size)
{
	const auto raw_packet_buffer = packet_buffer.Get();
	const auto alt_buffer = reinterpret_cast<const std::byte*>(raw_packet_buffer);

	switch (protocol)
	{
	case EPacketProtocol::SC_SIGNIN_SUCCESS:
	{
		int32 my_id{};
		saga::ReceiveSignInSucceedPacket(alt_buffer, my_id);

		UE_LOG(LogSagaNetwork, Log, TEXT("Local client's id is %d"), my_id);

		CallFunctionOnGameThread([this, my_id, name = GetLocalUserName()]()
			{
				SetLocalUserId(my_id);

				BroadcastOnSignedIn(my_id, name);
			}
		);
	}
	break;

	case EPacketProtocol::SC_SIGNIN_FAILURE:
	{
		int32 error{};
		saga::ReceiveSignInFailurePacket(alt_buffer, error);

		CallFunctionOnGameThread([this]()
			{
				SetLocalUserId(-1);

				BroadcastOnFailedToConnect(ESagaConnectionContract::SignInFailed);
			}
		);
	}
	break;

	case EPacketProtocol::SC_ROOM_CREATED:
	{
		int32 room_id{};
		saga::ReceiveRoomCreatedPacket(alt_buffer, room_id);

		CallFunctionOnGameThread([this, room_id]()
			{
				SetCurrentRoomId(room_id);
				currentRoom.MembersCount = 1;

				BroadcastOnRoomCreated(room_id);
			}
		);
	}
	break;

	case EPacketProtocol::SC_ROOM_CREATE_FAILED:
	{
		ERoomContract error{};
		saga::ReceiveRoomCreationFailedPacket(alt_buffer, error);

		const auto msg = std::to_wstring(error);
		UE_LOG(LogSagaNetwork, Warning, TEXT("Could not create a room due to %s"), msg.data());
	}
	break;

	case EPacketProtocol::SC_ROOM_JOINED:
	{
		saga::datagrams::SerializedMember newbie{};
		int32 room_id{};
		saga::ReceiveRoomJoinedPacket(alt_buffer, newbie, room_id, GetLocalUserId());

		if (newbie.id == GetLocalUserId())
		{
			UE_LOG(LogSagaNetwork, Log, TEXT("Local client has joined to the room %d"), room_id);

			CallFunctionOnGameThread(
				[this, room_id, team = static_cast<ESagaPlayerTeam>(newbie.team_id)]()
				{
					SetCurrentRoomId(room_id);
					SetTeam(GetLocalUserId(), team);

					BroadcastOnJoinedRoom(GetLocalUserId());
				}
			);
		}
		else
		{
			UE_LOG(LogSagaNetwork, Log, TEXT("Client %d [%s] has joined to the room %d"), newbie.id, newbie.nickname, room_id);

			CallFunctionOnGameThread(
				[this
				, id = MoveTemp(newbie.id)
				, nickname = FText::FromString(newbie.nickname)
				, team = static_cast<ESagaPlayerTeam>(newbie.team_id)]()
				{
					AddUser(FSagaVirtualUser
						{
							id,
							nickname,
							nullptr,
							team,
							EPlayerWeapon::LightSabor
						}
					);

					++currentRoom.MembersCount;

					BroadcastOnJoinedRoom(id);
				}
			);
		}
	}
	break;

	case EPacketProtocol::SC_ROOM_JOIN_FAILED:
	{
		ERoomContract error{};
		saga::ReceiveRoomJoinFailedPacket(alt_buffer, error);

		const auto msg = std::to_wstring(error);
		UE_LOG(LogSagaNetwork, Warning, TEXT("Failed to join to a room due to %s"), msg.data());
	}
	break;

	case EPacketProtocol::SC_ROOM_LEFT:
	{
		int32 left_client_id{};
		saga::ReceiveRoomLeftPacket(alt_buffer, left_client_id);

		if (left_client_id == GetLocalUserId())
		{
			CallFunctionOnGameThread([this]()
				{
					BroadcastOnLeftRoomBySelf();

					SetCurrentRoomId(-1);
					ClearUserList(true);
				}
			);
		}
		else
		{
			CallFunctionOnGameThread([this, left_client_id]()
				{
					BroadcastOnLeftRoom(left_client_id);

					RemoveUser(left_client_id);

					--currentRoom.MembersCount;
				}
			);
		}
	}
	break;

	case EPacketProtocol::SC_RESPOND_VERSION:
	{
		wchar_t version_string[16]{};

		saga::ReceiveRespondVersionPacket(alt_buffer, version_string, 16);

		CallPureFunctionOnGameThread([this, version_string]()
			{
				BroadcastOnRespondVersion(version_string);
			}
		);
	}
	break;

	case EPacketProtocol::SC_RESPOND_ROOMS:
	{
		std::vector<saga::datagrams::SerializedRoom> rooms{};

		saga::ReceiveRespondRoomsPacket(alt_buffer, rooms);

		UE_LOG(LogSagaNetwork, Log, TEXT("Number of rooms: %d"), rooms.size());

		CallFunctionOnGameThread([this, tr_rooms = std::move(rooms)]()
			{
				ClearRoomList();

				for (auto& room : tr_rooms)
				{
					AddRoom(FSagaVirtualRoom
						{
							room.id, FText::FromString(room.title), static_cast<int>(room.members)
						}
					);

					UE_LOG(LogSagaNetwork, Log, TEXT("Room (%d): %s (%d/4)"), room.id, room.title, room.members);
				}

				BroadcastOnUpdateRoomList(everyRooms);
			}
		);
	}
	break;

	case EPacketProtocol::SC_RESPOND_USERS:
	{
		std::vector<saga::datagrams::SerializedMember> users{};

		saga::ReceiveRespondUsersPacket(alt_buffer, users);

		UE_LOG(LogSagaNetwork, Log, TEXT("Members: %d"), users.size());

		CallFunctionOnGameThread([this, tr_users = std::move(users)]()
			{
				ClearUserList(true);

				for (auto& user : tr_users)
				{
					const auto team_id = user.team_id == 1 ? ESagaPlayerTeam::Red : ESagaPlayerTeam::Blue;

					const auto str = UEnum::GetValueAsString(team_id);

					UE_LOG(LogSagaNetwork, Log, TEXT("Client (%d): %s in team '%s'"), user.id, user.nickname, *str);

					AddUser(FSagaVirtualUser
						{
							user.id,
							FText::FromString(user.nickname),
							nullptr,
							team_id,
							EPlayerWeapon::LightSabor
						}
					);

					if (user.id == GetLocalUserId())
					{
						SetTeam(user.id, team_id);
					}
				}

				currentRoom.MembersCount = everyUsers.Num();

				BroadcastOnUpdateMembers(everyUsers);
			}
		);
	}
	break;

	case EPacketProtocol::SC_SET_TEAM:
	{
		int32 client_id{};
		bool is_red_team{};

		saga::ReceiveTeamChangerPacket(alt_buffer, client_id, is_red_team);

		UE_LOG(LogSagaNetwork, Log, TEXT("Client %d's team changed to %s"), client_id, is_red_team ? TEXT("Red") : TEXT("Blue"));

		SetTeam(client_id, is_red_team ? ESagaPlayerTeam::Red : ESagaPlayerTeam::Blue);

		CallPureFunctionOnGameThread([this, client_id, is_red_team]()
			{
				BroadcastOnTeamChanged(client_id, is_red_team);
			}
		);
	}
	break;

	case EPacketProtocol::SC_FAILED_GAME_START:
	{
		saga::SC_FailedGameStartingPacket pk{};
		pk.Read(alt_buffer);

		const ESagaGameContract cause = static_cast<ESagaGameContract>(static_cast<uint8>(pk.errCause));
		auto str = UEnum::GetValueAsString(cause);

		UE_LOG(LogSagaNetwork, Log, TEXT("Failed to start game due to %s"), *str);

		CallPureFunctionOnGameThread([this, cause]()
			{
				BroadcastOnFailedToStartGame(cause);
			}
		);
	}
	break;

	case EPacketProtocol::SC_GAME_GETTING_READY:
	{
		UE_LOG(LogSagaNetwork, Log, TEXT("Now start loading game..."));

		CallPureFunctionOnGameThread([this]()
			{
				BroadcastOnGetPreparedGame();
			}
		);
	}
	break;

	case EPacketProtocol::SC_GAME_START:
	{
		UE_LOG(LogSagaNetwork, Log, TEXT("Now start game..."));

		CallFunctionOnGameThread([this]()
			{
				everyUsers.Sort(
					[](const FSagaVirtualUser& lhs, const FSagaVirtualUser& rhs) noexcept -> bool
					{
						return lhs.myID < rhs.myID;
					}
				);

				BroadcastOnStartGame();
			}
		);
	}
	break;

	case EPacketProtocol::SC_CREATE_PLAYER:
	{
		UE_LOG(LogSagaNetwork, Log, TEXT("[SagaGame] %d Characters would be created"), everyUsers.Num());

		CallFunctionOnGameThread([this]()
			{
				for (auto& member : everyUsers)
				{
					if (member.myID == GetLocalUserId())
					{
						UE_LOG(LogSagaNetwork, Log, TEXT("[SC_CREATE_PLAYER] Local user `%d`"), member.myID);

						BroadcastOnCreatingCharacter(member.myID, GetLocalUserTeam(), GetLocalUserWeapon());
					}
					else
					{
						UE_LOG(LogSagaNetwork, Log, TEXT("[SC_CREATE_PLAYER] User `%d`"), member.myID);

						BroadcastOnCreatingCharacter(member.myID, member.myTeam, member.myWeapon);
					}
				}
			}
		);
	}
	break;

	case EPacketProtocol::SC_MOVE_CHARACTER:
	{
		int32 client_id{};
		float x{}, y{}, z{};

		saga::ReceivePositionPacket(alt_buffer, client_id, x, y, z);

		UE_LOG(LogSagaNetwork, Log, TEXT("[SagaGame] Client id %d: pos(%f,%f,%f)"), client_id, x, y, z);

		CallFunctionOnGameThread([this, client_id, x, y, z]()
			{
				BroadcastOnUpdatePosition(client_id, x, y, z);
			}
		);
	}
	break;

	case EPacketProtocol::SC_LOOK_CHARACTER:
	{
		int32 client_id{};
		float p{}, y{}, r{};

		saga::ReceiveRotationPacket(alt_buffer, client_id, p, y, r);

		UE_LOG(LogSagaNetwork, Log, TEXT("[SagaGame] Client id %d: rotation(%f,%f,%f)"), client_id, p, y, r);

		CallFunctionOnGameThread([this, client_id, p, y, r]()
			{
				BroadcastOnUpdateRotation(client_id, p, y, r);
			}
		);
	}
	break;

	case EPacketProtocol::SC_UPDATE_CHARACTER:
	{
	}
	break;

	case EPacketProtocol::SC_RPC:
	{
		ESagaRpcProtocol category{};
		int32 user_id{};
		int64 argument0{};
		int32 argument1{};

		saga::ReceiveRpcPacket(alt_buffer, category, user_id, argument0, argument1);

		//auto name = UEnum::GetValueAsString(category);

		//UE_LOG(LogSagaNetwork, Log, TEXT("[SagaGame][RPC] %s(%lld, %d) from client %d"), *name, argument0, argument1, user_id);

		CallFunctionOnGameThread([this, category, user_id, argument0, argument1]()
			{
				BroadcastOnRpc(category, user_id, argument0, argument1);
			}
		);
	}
	break;
	}
}
