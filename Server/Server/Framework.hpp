#pragma once
#include "ThreadPool.hpp"

import Iconer.Net.IFramework;
import Iconer.Utility.TypeTraits;
import Iconer.Utility.Container.AtomicQueue;
import Iconer.Net.ErrorCode;
import Iconer.App.UserManager;
import Iconer.App.Settings;
import <cstdint>;
import <array>;
import <unordered_map>;
import <thread>;

#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

namespace iconer::app
{
	class [[nodiscard]] TaskContext;
	class [[nodiscard]] PacketContext;
	class [[nodiscard]] User;
	enum class [[nodiscard]] PacketProtocol : std::uint8_t;
}

using EventDelegate = iconer::method_t<class ServerFramework, void, class iconer::app::User&, std::byte*>;

class ServerFramework final : public iconer::net::IFramework
{
public:
	using super = iconer::net::IFramework;

	static inline constexpr std::uint16_t serverPort = iconer::app::Settings::serverPort;

	ServerFramework() = default;
	~ServerFramework() = default;

	iconer::net::IoResult Initialize() override;
	void Startup() override;
	void Cleanup() override;

private:
	alignas(std::hardware_constructive_interference_size) ServerThreadPool myTaskPool{};
	alignas(std::hardware_constructive_interference_size) iconer::app::UserManager userManager{};

	alignas(std::hardware_constructive_interference_size) iconer::util::AtomicQueue2<iconer::app::PacketContext*, 10000> storedPacketContexts{};

	alignas(std::hardware_constructive_interference_size) std::unordered_map<iconer::app::PacketProtocol, EventDelegate> packetProcessors{};

	void OnTaskSucceed(iconer::net::IoContext* context, std::uint64_t id, std::uint32_t bytes);
	void OnTaskFailure(iconer::net::IoContext* context, std::uint64_t id, std::uint32_t bytes);

	void ReserveUser(class iconer::app::User& user) const noexcept;
	iconer::net::IoResult TriggerUser(class iconer::app::User& user) const noexcept;
	iconer::net::IoResult StartUser(class iconer::app::User& user) const;
	void CleanupUser(class iconer::app::User& user) const;
	void RoutePackets(class iconer::app::User& user);
	void ProcessPackets(class iconer::app::User& user, class iconer::app::PacketContext* context, std::uint32_t recv_bytes);

	ServerFramework(const ServerFramework&) = delete;
	ServerFramework& operator=(const ServerFramework&) = delete;
	ServerFramework(ServerFramework&&) = delete;
	ServerFramework& operator=(ServerFramework&&) = delete;
};

