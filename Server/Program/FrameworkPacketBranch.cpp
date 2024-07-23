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
ServerFramework::ProcessPacketsInline(iconer::app::User& user)
{
	auto buffer = user.GetReceivedData();
	const size_t buf_size = buffer.size();

	if (buf_size <= 2)
	{
		std::println("The packet protocol: {}. Not received yet.", std::to_underlying(buffer[0]));

		return;
	}

	size_t offset = 0;
	size_t stacked_size = 0;

	for (; offset < buf_size; ++offset)
	{
		const auto ptr = buffer.data() + offset;

		const iconer::app::PacketProtocol protocol = static_cast<iconer::app::PacketProtocol>(*ptr);
		//std::println("Received packet protocol: {}.", std::to_underlying(protocol));
		if (256 <= std::to_underlying(protocol)) break;

		const std::int16_t size = *reinterpret_cast<const std::int16_t*>(ptr + 1);
		if (size <= 0) break;

		if (buf_size - stacked_size < size)
		{
			//std::println("Received and mssing packet size: {}.", size);

			break;
		}
		else
		{
			//std::println("Received packet size: {}.", size);

			if (auto it = packetProcessors.find(protocol); it != packetProcessors.cend())
			{
				//std::println("Processing a packet {}.", protocol_value);

				const auto& method = it->second;

				std::invoke(method, this, std::ref(user), ptr + 3);
			}
			else
			{
				std::println("Has no packet processor for {}.", std::to_underlying(protocol));
			}

			offset += size;
			stacked_size += size + 3;
		}

		if (buf_size <= offset - 3) break;
	}

	if (0 < stacked_size)
	{
		user.myReceiver.PullReceivedData(offset);
	}

	/*
	if (0 < stacked_size)
	{
		//std::println("Defer procssing packets within {} bytes.", stacked_size);
		const auto buf = user.AcquireReceivedData(stacked_size);

		const auto data = buf.get();

		for (size_t offset = 0; offset < stacked_size; ++offset)
		{
			const auto ptr = data + offset;

			const iconer::app::PacketProtocol protocol = static_cast<iconer::app::PacketProtocol>(*ptr);
			const auto protocol_value = std::to_underlying(protocol);
			//std::println("The packet protocol: {}.", protocol_value);

			const std::int16_t size = *reinterpret_cast<const std::int16_t*>(ptr + 1);
			//std::println("The packet size: {}.", size);

			if (auto it = packetProcessors.find(protocol); it != packetProcessors.cend())
			{
				//std::println("Processing a packet {}.", protocol_value);

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
	*/

	/*
	auto buffer = user.GetReceivedData();
	const ptrdiff_t buf_size = static_cast<ptrdiff_t>(buffer.size_bytes());

	if (buf_size < 3)
	{
		return;
	}

	const auto beg = buffer.data();
	const auto end = buffer.data() + buffer.size_bytes();

	ptrdiff_t offset = 0;

	while (true)
	{
		auto ptr = beg;

		const iconer::app::PacketProtocol protocol = static_cast<iconer::app::PacketProtocol>(*ptr);
		ptr += sizeof(iconer::app::PacketProtocol);
		std::println("The packet protocol: {}.", std::to_underlying(protocol));

		const ptrdiff_t size = static_cast<ptrdiff_t>(*reinterpret_cast<const std::int16_t*>(ptr));
		ptr += sizeof(std::int16_t);

		if (size <= 0 or buffer.size_bytes() < size) break;
		std::println("Received packet size: {}.", size);

		if (auto it = packetProcessors.find(protocol); it != packetProcessors.cend())
		{
			const auto& method = it->second;

			std::invoke(method, this, std::ref(user), ptr);
		}

		const auto pk_size = sizeof(iconer::app::PacketProtocol) + sizeof(std::int16_t) + size;

		user.myReceiver.PullReceivedData(pk_size);
		offset += pk_size;
		//ptr += pk_size;

		if (buf_size <= offset) break;
	}
	*/
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

		const iconer::app::PacketProtocol protocol = static_cast<iconer::app::PacketProtocol>(*ptr);
		//std::println("Received packet protocol: {}.", std::to_underlying(protocol));

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
		//const auto protocol_value = std::to_underlying(protocol);
		//std::println("The packet protocol: {}.", protocol_value);

		const std::int16_t size = *reinterpret_cast<const std::int16_t*>(ptr + 1);
		//std::println("The packet size: {}.", size);

		if (auto it = packetProcessors.find(protocol); it != packetProcessors.cend())
		{
			//std::println("Processing a packet {}.", protocol_value);

			const auto& method = it->second;

			std::invoke(method, this, std::ref(user), ptr + 3);
		}
		else
		{
			//std::println("Has no packet processor for {}.", protocol_value);
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
