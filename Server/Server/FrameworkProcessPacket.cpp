#include "Framework.hpp"

import Iconer.App.User;
import Iconer.App.PacketContext;
import Iconer.App.PacketProtocol;
import <utility>;
import <bit>;
import <format>;

void
ServerFramework::RoutePackets(iconer::app::User& user)
{
	auto buffer = user.GetReceivedData();
	const size_t buf_size = buffer.size();

	if (buf_size <= 2)
	{
		std::println("The packet protocol: {}. Not received yet.", std::to_underlying(buffer[0]));

		return;
	}

	for (size_t offset = 0; offset < buf_size; ++offset)
	{
		const auto ptr = buffer.data() + offset;
		const auto byte = *ptr;

		const iconer::app::PacketProtocol protocol = static_cast<iconer::app::PacketProtocol>(*ptr);
		std::println("The packet protocol: {}.", std::to_underlying(buffer[0]));

		const std::int16_t size = *reinterpret_cast<const std::int16_t*>(ptr + 1);

		if (buf_size < size)
		{
			std::println("The packet size: {}. Not received yet.", size);

			break;
		}
		else
		{
			std::println("The packet size: {}.", size);

			offset += size;
		}

		if (buf_size - 2 <= offset)
		{
			std::println("Defer procssing packets within {} bytes.", offset);

			const auto ctx = storedPacketContexts.pop();
			ctx->myData = user.AcquireReceivedData(offset);

			myTaskPool.Schedule(ctx, user.GetID(), static_cast<unsigned long>(offset));

			break;
		}
	}
}
