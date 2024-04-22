module;
#include <string_view>

module Demo.Framework;
import Iconer.Application.Packet;
import Iconer.Application.Resources.String;

demo::Framework::AcceptResult
demo::Framework::OnReserveAccept(iconer::app::User& user)
{
	if (user.TryChangeState(iconer::app::UserStates::None, iconer::app::UserStates::Reserved))
	{
		user.SetOperation(iconer::app::AsyncOperations::OpAccept);
		return serverListener.BeginAccept(user, user.mySocket);
	}
	else
	{
		return iconer::net::ErrorCode::OPERATION_ABORTED;
	}
}

void
demo::Framework::OnFailedReservingAccept()
{
	auto& msg = iconer::app::GetResourceString<6>();
	throw msg.data();
}

demo::Framework::IoResult
demo::Framework::OnUserConnected(iconer::app::User& user)
{
	if (auto fin = user.mySocket.EndAccept(serverListener); fin.has_value())
	{
		return std::unexpected{ fin.value() };
	}
	else if (user.TryChangeState(iconer::app::UserStates::Reserved, iconer::app::UserStates::Connected))
	{
		user.SetOperation(iconer::app::AsyncOperations::OpSignIn);

		return user.Receive(GetBuffer(user.GetID()));
	}
	else
	{
		return std::unexpected{ iconer::net::ErrorCode::OPERATION_ABORTED };
	}
}

void
demo::Framework::OnFailedUserConnect(iconer::app::User& user)
{
	if (not user.BeginClose(iconer::app::UserStates::Reserved))
	{
		user.Cleanup();
	}
}

demo::Framework::IoResult
demo::Framework::OnUserSignedIn(iconer::app::User& user, const ptrdiff_t& bytes)
{
	if (bytes <= 0)
	{
		return std::unexpected{ iconer::net::ErrorCode::OPERATION_ABORTED };
	}

	// Small receiving phase
	auto user_buffer = GetBuffer(user.GetID());
	auto& user_recv_offset = user.recvOffset;

	user_recv_offset += bytes;

	if (iconer::app::packets::CS_SignInPacket::SignedWannabeSize() <= user_recv_offset)
	{
		thread_local iconer::app::packets::CS_SignInPacket pk{};
		pk.Read(user_buffer.data(), user_recv_offset);

		if (pk.myProtocol != iconer::app::PacketProtocol::CS_SIGNIN or pk.mySize <= 0)
		{
			return std::unexpected{ iconer::net::ErrorCode::OPERATION_ABORTED };
		}

		if (true) [[likely]] { // The signing in is succeed
			// Don't necessary to pull back buffer of the receive buffer
			user_recv_offset -= iconer::app::packets::CS_SignInPacket::SignedWannabeSize();

			user.SetName(pk.userName);

			while (not user.TryChangeState(iconer::app::UserStates::Connected, iconer::app::UserStates::PendingID));

			user.SetOperation(iconer::app::AsyncOperations::OpAssignID);

			return user.SendSignInPacket();
			}
		else [[unlikely]] {
			// TODO: send sign in failed packet
			return std::unexpected{ iconer::net::ErrorCode::OPERATION_ABORTED };
			};
	}
	else
	{
		// Restart remain packet bytes
		// Still at OpSignIn
		return user.Receive(user_buffer);
	}
}

void
demo::Framework::OnFailedUserSignIn(iconer::app::User& user)
{
	if (not user.BeginClose())
	{
		user.Cleanup();
	}
}

demo::Framework::IoResult
demo::Framework::OnNotifyUserId(iconer::app::User& user)
{
	if (user.TryChangeState(iconer::app::UserStates::PendingID, iconer::app::UserStates::Idle))
	{
		user.SetOperation(iconer::app::AsyncOperations::OpRecv);

		return user.Receive(GetBuffer(user.GetID()));
	}
	else
	{
		return std::unexpected{ iconer::net::ErrorCode::OPERATION_ABORTED };
	}
}

void
demo::Framework::OnFailedNotifyId(iconer::app::User& user)
{
	if (not user.BeginClose(iconer::app::UserStates::PendingID))
	{
		user.Cleanup();
	}
}

demo::Framework::IoResult
demo::Framework::OnNotifyRoomsList(iconer::app::User& user)
{
	if (GetRoomModifiedFlag())
	{
		static iconer::app::packets::SC_RespondRoomsPacket pk{};

		pk.serializedRooms.clear();
		for (auto room : everyRoom)
		{
			if (nullptr != room and 0 < room->GetMembersCount())
			{
				pk.AddMember(room->GetID(), room->GetName(), room->GetMembersCount());
			}
		}
		pk.Write(serializedRoomsBuffer.get());
		serializedRoomsBufferSize = pk.WannabeSize();

		SetRoomModifiedFlag();
	}

	auto sender = AcquireSendContext();

	auto io = user.SendGeneralData(sender, serializedRoomsBuffer.get(), serializedRoomsBufferSize);
	if (not io)
	{
		sender->ReturnToBase();
	}

	return std::move(io);
}

demo::Framework::IoResult
demo::Framework::OnNotifyMemberOfRoom(iconer::app::User& user)
noexcept
{
	const auto room_id = user.myRoomId.Load();
	if (room_id == -1)
	{
		return std::unexpected{ iconer::net::ErrorCode::OPERATION_ABORTED };
	}

	auto room = FindRoom(room_id);
	if (room == nullptr)
	{
		return std::unexpected{ iconer::net::ErrorCode::OPERATION_ABORTED };
	}

	std::span<std::byte> members = room->SerializeMembers();

	auto [sent_r, ctx] = user.SendGeneralData(members.data(), members.size());
	if (not sent_r.has_value())
	{
		ctx.Complete();
	}

	return std::move(sent_r);
}

bool
demo::Framework::OnNotifyTeam(iconer::app::User& user)
{
	const auto room_id = user.myRoomId.Load();
	const auto& user_id = user.GetID();
	const auto team_id = user.myTeamId.Load();

	if (room_id != -1)
	{
		if (auto room = FindRoom(room_id); room != nullptr)
		{
			if (not room->HasMember(user_id))
			{
				// fixup
				user.myRoomId.CompareAndSet(room_id, -1);

				return false;
			}
			{
				if (room->GetState() == iconer::app::RoomStates::Idle)
				{
					room->ForEach(
						[user_id, team_id](iconer::app::User& user)
						{
							auto [io, ctx] = user.SendChangeTeamPacket(user_id, team_id == iconer::app::Team::Red);
							if (not io)
							{
								ctx.Complete();
							};
						}
					);
				}

				return true;
			}
		}
	}

	return false;
}

void
demo::Framework::OnFailedNotifyRoomMember(iconer::app::User& user)
noexcept
{
	//user.TryChangeState(iconer::app::UserStates::EnteringRoom, iconer::app::UserStates::Idle);
}
