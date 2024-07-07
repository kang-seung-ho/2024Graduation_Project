module;
#include <utility>
//#include <bit>
//#include <format>
#include <print>

module Iconer.Framework;
import Iconer.App.User;
import Iconer.App.PacketContext;
import Iconer.App.PacketProtocol;

namespace
{
	iconer::util::AtomicQueue<iconer::app::PacketContext*, ServerFramework::reservedPacketContextCount> storedPacketContexts{};
}

void
ServerFramework::RoutePackets(iconer::app::User& user)
{
	auto buffer = user.GetReceivedData();
	const size_t buf_size = buffer.size();

	if (buf_size <= 2)
	{
		//std::println("The packet protocol: {}. Not received yet.", std::to_underlying(buffer[0]));

		return;
	}

	size_t stacked_size = 0;
	for (size_t offset = 0; offset < buf_size; ++offset)
	{
		const auto ptr = buffer.data() + offset;
		const auto byte = *ptr;

		const iconer::app::PacketProtocol protocol = static_cast<iconer::app::PacketProtocol>(*ptr);
		std::println("Received packet protocol: {}.", std::to_underlying(protocol));

		const std::int16_t size = *reinterpret_cast<const std::int16_t*>(ptr + 1);

		if (buf_size - stacked_size < size)
		{
			//std::println("Received and mssing packet size: {}.", size);

			break;
		}
		else
		{
			//std::println("Received packet size: {}.", size);

			offset += size;
			stacked_size += size;
		}
	}

	if (0 < stacked_size)
	{
		//std::println("Defer procssing packets within {} bytes.", stacked_size);

		const auto ctx = storedPacketContexts.pop();
		ctx->myData = user.AcquireReceivedData(stacked_size);

		myTaskPool.Schedule(ctx, user.GetID(), static_cast<unsigned long>(stacked_size));
	}
}

void
ServerFramework::ProcessPackets(iconer::app::User& user, iconer::app::PacketContext* context, std::uint32_t recv_bytes)
{
	struct Guard
	{
		iconer::app::PacketContext* ctx;
		ServerFramework* framework;

		~Guard() noexcept
		{
			ctx->ClearIoStatus();
			ctx->myData.reset();
			storedPacketContexts.push(ctx);
		}
	};

	Guard guard{ context, this };

	const auto& data = context->myData;
	const auto buffer = data.get();

	for (std::uint32_t offset = 0; offset < recv_bytes; ++offset)
	{
		const auto ptr = buffer + offset;

		const iconer::app::PacketProtocol protocol = static_cast<iconer::app::PacketProtocol>(*ptr);
		const auto protocol_value = std::to_underlying(protocol);
		//std::println("The packet protocol: {}.", protocol_value);

		const std::int16_t size = *reinterpret_cast<const std::int16_t*>(ptr + 1);
		//std::println("The packet size: {}.", size);

		if (auto it = packetProcessors.find(protocol); it != packetProcessors.cend())
		{
			std::println("Processing a packet {}.", protocol_value);

			const auto& method = it->second;

			std::invoke(method, this, std::ref(user), ptr + 3);
		}
		else
		{
			std::println("Has no packet processor for {}.", protocol_value);
		}

		offset += size;
	}
}

void
ServerFramework::AddPacketContext(iconer::app::PacketContext* context)
noexcept
{
	storedPacketContexts.push(context);
}
