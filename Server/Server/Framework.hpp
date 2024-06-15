#pragma once
#include "ThreadPool.hpp"

import Iconer.Utility.TypeTraits;
import Iconer.Net.ErrorCode;
import Iconer.Net.Socket;
import Iconer.App.PacketProtocol;
import <cstdint>;
import <expected>;
import <array>;
import <unordered_map>;
import <thread>;
import <latch>;

#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

using EventDelegate = iconer::function_t<void, class ServerFramework&, iconer::net::IoEvent&>;

class ServerFramework
{
public:
	static inline constexpr std::uint16_t serverPort = 40000;

	ServerFramework() = default;
	~ServerFramework() = default;

	std::expected<void, iconer::net::ErrorCode> Initialize();
	void Startup();
	void Cleanup();

	bool ProcessTask(iconer::net::IoEvent task);

private:
	iconer::net::Socket listenSocket;

	ServerThreadPool myTaskPool;

	std::unordered_map<iconer::app::PacketProtocol, EventDelegate> packetProcessors;

	ServerFramework(const ServerFramework&) = delete;
	ServerFramework& operator=(const ServerFramework&) = delete;
	ServerFramework(ServerFramework&&) = delete;
	ServerFramework& operator=(ServerFramework&&) = delete;
};

