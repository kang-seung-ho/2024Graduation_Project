module;
#include <string_view>

module Demo.Framework;
import Iconer.Application.Packet;
import Iconer.Application.Resources.String;
import Demo.PacketProcessor;

#define LIKELY [[likely]]
#define UNLIKELY [[unlikely]]

demo::Framework::IoResult
demo::Framework::OnReceived(iconer::app::User& user, const ptrdiff_t& bytes)
{
	const IdType& id = user.GetID();
	auto user_buffer = GetBuffer(id);
	auto& user_recv_offset = user.recvOffset;

	user_recv_offset += bytes;

	if (user_recv_offset < iconer::app::BasicPacket::SignedMinSize())
	{
		myLogger.DebugLogWarning(iconer::app::GetResourceString<10>());
	}

	while (iconer::app::BasicPacket::SignedMinSize() <= user_recv_offset)
	{
		auto proceed_bytes = PacketProcessor(*this, user, user_buffer, user_recv_offset);
		if (proceed_bytes < 0) UNLIKELY
		{
			myLogger.LogWarning(iconer::app::GetResourceString<7>());

			return std::unexpected{ iconer::net::ErrorCode::NoBufferStorage };
		}
		else if (0 == proceed_bytes) UNLIKELY
		{
			myLogger.DebugLogWarning(iconer::app::GetResourceString<8>());
			break;
		}
		else LIKELY
		{
			myLogger.DebugLog(iconer::app::GetResourceString<9>(), static_cast<std::int8_t>(user_buffer[0]), static_cast<std::int8_t>(user_buffer[1]), static_cast<std::int8_t>(user_buffer[2]));

			const auto last_off = user_buffer.size_bytes() - proceed_bytes;
			std::memcpy(user_buffer.data() + proceed_bytes, user_buffer.data(), last_off);
			std::memset(user_buffer.data() + last_off, 0, static_cast<size_t>(last_off));
			user_recv_offset -= proceed_bytes;
		}
	}

	return user.Receive(user_buffer);
}
