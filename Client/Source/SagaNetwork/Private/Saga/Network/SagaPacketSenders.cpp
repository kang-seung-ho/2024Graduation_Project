#include "Saga/Network/SagaPacketSenders.h"
#include "Sockets.h"

#include "Saga/Network/SagaNetworkUtility.h"
#include "Saga/Network/SagaClientPacketPrefabs.h"

[[nodiscard]]
bool IsConnected(FSocket* socket) noexcept;

std::optional<int32>
saga::SendSignInPacket(FSocket* socket, FStringView nickname)
{
	if (socket)
	{
		if (::IsConnected(socket))
		{
			const saga::CS_SignInPacket pk{ nickname.GetData(), static_cast<size_t>(nickname.Len()) };
			auto ptr = pk.Serialize();

			const int32 sent_bytes = saga::RawSend(socket, ptr.get(), pk.WannabeSize());
			if (0 < sent_bytes)
			{
				return sent_bytes;
			}
		}
		else
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("The socket is not connected. (SendSignInPacket)"));
		}
	}
	else
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("The socket is null. (SendSignInPacket)"));
	}

	return std::nullopt;
}

std::optional<int32>
saga::SendCreateRoomPacket(FSocket* socket, FStringView title)
{
	if (socket)
	{
		if (::IsConnected(socket))
		{
			const saga::CS_CreateRoomPacket pk{ title.GetData(), static_cast<size_t>(title.Len()) };
			auto ptr = pk.Serialize();

			const int32 sent_bytes = saga::RawSend(socket, ptr.get(), pk.WannabeSize());
			if (0 < sent_bytes)
			{
				return sent_bytes;
			}
		}
		else
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("The socket is not connected. (SendCreateRoomPacket)"));
		}
	}
	else
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("The socket is null. (SendCreateRoomPacket)"));
	}

	return std::nullopt;
}

std::optional<int32>
saga::SendJoinRoomPacket(FSocket* socket, int32 room_id)
{
	if (socket)
	{
		if (::IsConnected(socket))
		{
			const saga::CS_EnterRoomPacket pk{ room_id };
			auto ptr = pk.Serialize();

			const int32 sent_bytes = saga::RawSend(socket, ptr.get(), pk.WannabeSize());
			if (0 < sent_bytes)
			{
				return sent_bytes;
			}
		}
		else
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("The socket is not connected. (SendJoinRoomPacket)"));
		}
	}
	else
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("The socket is null. (SendJoinRoomPacket)"));
	}

	return std::nullopt;
}

std::optional<int32>
saga::SendLeaveRoomPacket(FSocket* socket)
{
	if (socket)
	{
		if (::IsConnected(socket))
		{
			static constexpr saga::CS_LeaveRoomPacket pk{};
			static auto ptr = pk.Serialize();

			const int32 sent_bytes = saga::RawSend(socket, ptr.get(), pk.WannabeSize());
			if (0 < sent_bytes)
			{
				return sent_bytes;
			}
		}
		else
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("The socket is not connected. (SendLeaveRoomPacket)"));
		}
	}
	else
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("The socket is null. (SendLeaveRoomPacket)"));
	}

	return std::nullopt;
}

std::optional<int32>
saga::SendRequestVersionPacket(FSocket* socket)
{
	if (socket)
	{
		if (::IsConnected(socket))
		{
			static constexpr saga::CS_RequestVersionPacket pk{};
			static auto ptr = pk.Serialize();

			const int32 sent_bytes = saga::RawSend(socket, ptr.get(), pk.WannabeSize());
			if (0 < sent_bytes)
			{
				return sent_bytes;
			}
		}
		else
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("The socket is not connected. (SendRequestVersionPacket)"));
		}
	}
	else
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("The socket is null. (SendRequestVersionPacket)"));
	}

	return std::nullopt;
}

std::optional<int32>
saga::SendRequestRoomsPacket(FSocket* socket)
{
	if (socket)
	{
		if (::IsConnected(socket))
		{
			static constexpr saga::CS_RequestRoomsPacket pk{};
			static auto ptr = pk.Serialize();

			const int32 sent_bytes = saga::RawSend(socket, ptr.get(), pk.WannabeSize());
			if (0 < sent_bytes)
			{
				return sent_bytes;
			}
		}
		else
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("The socket is not connected. (SendRequestRoomsPacket)"));
		}
	}
	else
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("The socket is null. (SendRequestRoomsPacket)"));
	}

	return std::nullopt;
}

std::optional<int32>
saga::SendRequestMembersPacket(FSocket* socket)
{
	if (socket)
	{
		if (::IsConnected(socket))
		{
			static constexpr saga::CS_RequestMembersPacket pk{};
			static auto ptr = pk.Serialize();

			const int32 sent_bytes = saga::RawSend(socket, ptr.get(), pk.WannabeSize());
			if (0 < sent_bytes)
			{
				return sent_bytes;
			}
		}
		else
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("The socket is not connected. (SendRequestMembersPacket)"));
		}
	}
	else
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("The socket is null. (SendRequestMembersPacket)"));
	}

	return std::nullopt;
}

std::optional<int32>
saga::SendChangeTeamPacket(FSocket* socket, bool is_red_team)
{
	if (socket)
	{
		if (::IsConnected(socket))
		{
			if (is_red_team)
			{
				static constexpr saga::CS_SetTeamPacket pk1{ 0 };
				static auto ptr = pk1.Serialize();

				const int32 sent_bytes = saga::RawSend(socket, ptr.get(), pk1.WannabeSize());
				if (0 < sent_bytes)
				{
					return sent_bytes;
				}
			}
			else
			{
				static constexpr saga::CS_SetTeamPacket pk2{ 1 };
				static auto ptr = pk2.Serialize();

				const int32 sent_bytes = saga::RawSend(socket, ptr.get(), pk2.WannabeSize());
				if (0 < sent_bytes)
				{
					return sent_bytes;
				}
			}
		}
		else
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("The socket is not connected. (SendChangeTeamPacket)"));
		}
	}
	else
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("The socket is null. (SendChangeTeamPacket)"));
	}

	return std::nullopt;
}

std::optional<int32>
saga::SendGameStartPacket(FSocket* socket)
{
	if (socket)
	{
		if (::IsConnected(socket))
		{
			static constexpr saga::CS_GameStartPacket pk{};
			static auto ptr = pk.Serialize();

			const int32 sent_bytes = saga::RawSend(socket, ptr.get(), pk.WannabeSize());
			if (0 < sent_bytes)
			{
				return sent_bytes;
			}
		}
		else
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("The socket is not connected. (SendGameStartPacket)"));
		}
	}
	else
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("The socket is null. (SendGameStartPacket)"));
	}

	return std::nullopt;
}

std::optional<int32>
saga::SendGameIsLoadedPacket(FSocket* socket)
{
	if (socket)
	{
		if (::IsConnected(socket))
		{
			static constexpr saga::CS_GameLoadedPacket pk{};
			static auto ptr = pk.Serialize();

			const int32 sent_bytes = saga::RawSend(socket, ptr.get(), pk.WannabeSize());
			if (0 < sent_bytes)
			{
				return sent_bytes;
			}
		}
		else
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("The socket is not connected. (SendGameIsLoadedPacket)"));
		}
	}
	else
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("The socket is null. (SendGameIsLoadedPacket)"));
	}

	return std::nullopt;
}

std::optional<int32>
saga::SendPositionPacket(FSocket* socket, float x, float y, float z)
{
	if (socket)
	{
		if (::IsConnected(socket))
		{
			const saga::CS_UpdatePositionPacket pk{ x, y, z };
			auto ptr = pk.Serialize();

			const int32 sent_bytes = saga::RawSend(socket, ptr.get(), pk.WannabeSize());
			if (0 < sent_bytes)
			{
				return sent_bytes;
			}
		}
		else
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("The socket is not connected. (SendPositionPacket)"));
		}
	}
	else
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("The socket is null. (SendPositionPacket)"));
	}

	return std::nullopt;
}

bool
IsConnected(FSocket* socket)
noexcept
{
	return socket->GetConnectionState() == ESocketConnectionState::SCS_Connected;
}
