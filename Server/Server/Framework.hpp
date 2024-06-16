#pragma once
#include "ThreadPool.hpp"

import Iconer.Net.IFramework;
import Iconer.Utility.TypeTraits;
import Iconer.Net.ErrorCode;
import Iconer.Net.Socket;
import Iconer.App.PacketProtocol;
import Iconer.App.UserManager;
import <cstdint>;
import <expected>;
import <array>;
import <unordered_map>;
import <thread>;

#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

using EventDelegate = iconer::function_t<void, class ServerFramework&, iconer::net::IoEvent&>;

class ServerFramework final : public iconer::net::IFramework
{
public:
	using super = iconer::net::IFramework;

	static inline constexpr std::uint16_t serverPort = 40000;

	ServerFramework() = default;
	~ServerFramework() = default;

	std::expected<void, iconer::net::ErrorCode> Initialize() override;
	void Startup() override;
	void Cleanup() override;

private:
	iconer::app::UserManager userManager;
	std::unordered_map<iconer::app::PacketProtocol, EventDelegate> packetProcessors;

	ServerThreadPool myTaskPool;

	void OnTaskSucceed(iconer::net::IoContext* context, std::uint64_t id, std::uint32_t bytes) const;
	void OnTaskFailure(iconer::net::IoContext* context, std::uint64_t id, std::uint32_t bytes) const;

	void ReserveUser(class iconer::app::User& user) const noexcept;

	ServerFramework(const ServerFramework&) = delete;
	ServerFramework& operator=(const ServerFramework&) = delete;
	ServerFramework(ServerFramework&&) = delete;
	ServerFramework& operator=(ServerFramework&&) = delete;
};

