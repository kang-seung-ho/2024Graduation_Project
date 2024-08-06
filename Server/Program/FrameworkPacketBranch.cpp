module;
#include <utility>

module Iconer.Framework;
import Iconer.App.User;
import Iconer.App.PacketContext;
import Iconer.App.PacketProtocol;

namespace
{
	iconer::util::AtomicQueue<iconer::app::PacketContext*, ServerFramework::reservedPacketContextCount> storedPacketContexts{};
}

void
ServerFramework::ProcessPacketsInline(iconer::app::User& user, const size_t& buf_size)
{
	auto buffer = user.GetReceivedData(buf_size);

	if (buf_size < 3)
	{
		PrintLn("The packet protocol: {}. Not received yet.", std::to_underlying(buffer[0]));

		return;
	}

	auto ptr = buffer.data();
	const auto end = buffer.data() + buf_size;

	size_t stacked_size = 0;

	while (ptr < end)
	{
		auto&& protocol_value = std::to_underlying(ptr[0]);
		//PrintLn("Received packet protocol: {}.", protocol_value);

		if (256 <= protocol_value)
		{
			PrintLn("[Error] The packet's protocol is {}.", protocol_value);

			break;
		}

		ptr += 1;
		if (end <= ptr) break;

		const std::int16_t pk_size = *reinterpret_cast<const std::int16_t*>(ptr);
		if (pk_size <= 0)
		{
			PrintLn("[Error] The packet's size is {}.", pk_size);

			break;
		}

		ptr += 2;
		if (end < ptr)
		{
			break;
		}

	//	PrintLn("Received packet size: {}.", pk_size);

		if (end - pk_size + 3 < ptr)
		{
			//PrintLn("[Warning] Packet {} is incompleted. (size: {})", protocol_value, pk_size);

			break;
		}

		const auto protocol = static_cast<iconer::app::PacketProtocol>(protocol_value);
		const auto it = packetProcessors.find(protocol);

		if (it != packetProcessors.cend())
		{
			//PrintLn("Processing a packet {}.", protocol_value);

			const auto& method = it->second;

			std::invoke(method, this, std::ref(user), ptr);
		}
		else
		{
			PrintLn("Has no packet processor for {}.", std::to_underlying(protocol));
		}

		ptr += pk_size - 3;
		stacked_size += static_cast<ptrdiff_t>(pk_size);
	}

	if (0 < stacked_size)
	{
		user.myReceiver.PullReceivedData(stacked_size);
	}
}

void
ServerFramework::RoutePackets(iconer::app::User& user)
{
	auto buffer = user.GetReceivedData();
	const size_t buf_size = buffer.size();

	if (buf_size <= 2)
	{
		//PrintLn("The packet protocol: {}. Not received yet.", std::to_underlying(buffer[0]));

		return;
	}

	size_t stacked_size = 0;
	for (size_t offset = 0; offset < buf_size; ++offset)
	{
		const auto ptr = buffer.data() + offset;

		const iconer::app::PacketProtocol protocol = static_cast<iconer::app::PacketProtocol>(*ptr);
		//PrintLn("Received packet protocol: {}.", std::to_underlying(protocol));

		const std::int16_t size = *reinterpret_cast<const std::int16_t*>(ptr + 1);

		if (buf_size - stacked_size < size)
		{
			//PrintLn("Received and mssing packet size: {}.", size);

			break;
		}
		else
		{
			//PrintLn("Received packet size: {}.", size);

			offset += size;
			stacked_size += size;
		}
	}

	if (0 < stacked_size)
	{
		//PrintLn("Defer procssing packets within {} bytes.", stacked_size);

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
		//const auto protocol_value = std::to_underlying(protocol);
		//PrintLn("The packet protocol: {}.", protocol_value);

		const std::int16_t size = *reinterpret_cast<const std::int16_t*>(ptr + 1);
		//PrintLn("The packet size: {}.", size);

		if (auto it = packetProcessors.find(protocol); it != packetProcessors.cend())
		{
			//PrintLn("Processing a packet {}.", protocol_value);

			const auto& method = it->second;

			std::invoke(method, this, std::ref(user), ptr + 3);
		}
		else
		{
			//PrintLn("Has no packet processor for {}.", protocol_value);
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
