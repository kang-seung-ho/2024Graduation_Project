#pragma once
#include "ThreadPool.hpp"

import Iconer.Net.IFramework;
import Iconer.Utility.TypeTraits;
import Iconer.Utility.Container.AtomicQueue;
import Iconer.Net.ErrorCode;
import Iconer.App.UserManager;
import Iconer.App.PacketProtocol;
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
	class [[nodiscard]] SendContext;
	class [[nodiscard]] User;
}

using EventDelegate = iconer::method_t<class ServerFramework, void, iconer::app::User&, std::byte*>;

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

	alignas(std::hardware_constructive_interference_size) iconer::util::AtomicQueue<iconer::app::PacketContext*, 3000> storedPacketContexts{};
	alignas(std::hardware_constructive_interference_size) iconer::util::AtomicQueue<iconer::app::SendContext*, 10000> storedSendContexts{};

	alignas(std::hardware_constructive_interference_size) std::unordered_map<iconer::app::PacketProtocol, EventDelegate> packetProcessors{};

	void OnTaskSucceed(iconer::net::IoContext* context, std::uint64_t id, std::uint32_t bytes);
	void OnTaskFailure(iconer::net::IoContext* context, std::uint64_t id, std::uint32_t bytes);

	/* Set nickname and assign user's id */
	void EventOnSignIn(iconer::app::User& user, std::byte* data);
	/* Make a room */
	void EventOnMakeRoom(iconer::app::User& user, std::byte* data);
	/* Join to the room */
	void EventOnJoinRoom(iconer::app::User& user, std::byte* data);
	/* Exit from the room */
	void EventOnExitRoom(iconer::app::User& user, std::byte* data);
	/* Match a random room */
	void EventOnSeekRoom(iconer::app::User& user, std::byte* data);
	/* Send a list of room */
	void EventOnRoomList(iconer::app::User& user, std::byte* data);

	void ReserveUser(iconer::app::User& user) const noexcept;
	iconer::net::IoResult TriggerUser(iconer::app::User& user) const noexcept;
	iconer::net::IoResult StartUser(iconer::app::User& user) const;
	void CleanupUser(iconer::app::User& user) const;
	void RoutePackets(iconer::app::User& user);
	void ProcessPackets(iconer::app::User& user, iconer::app::PacketContext* context, std::uint32_t recv_bytes);

	[[nodiscard]]
	iconer::app::SendContext* AcquireSendContext() noexcept;

	ServerFramework(const ServerFramework&) = delete;
	ServerFramework& operator=(const ServerFramework&) = delete;
	ServerFramework(ServerFramework&&) = delete;
	ServerFramework& operator=(ServerFramework&&) = delete;
};

