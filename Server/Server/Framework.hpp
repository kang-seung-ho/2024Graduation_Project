#pragma once
import Iconer.Utility.TypeTraits;
import Iconer.Net.ErrorCode;
import Iconer.Net.Socket;
import Iconer.Net.IoCompletionPort;
import Iconer.App.PacketProtocol;
import <cstdint>;
import <expected>;
import <array>;
import <unordered_map>;
import <thread>;
import <latch>;

#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

using EventDelegate = iconer::function_t<void, class Framework&, iconer::net::IoEvent&>;

class Framework
{
public:
	static inline constexpr std::uint16_t serverPort = 40000;

	static inline constexpr size_t workerCount = 4;

	Framework() = default;
	~Framework() = default;

	std::expected<void, iconer::net::ErrorCode> Initialize();
	void Startup();
	void Cleanup();

	void ArriveWorker() noexcept;
	[[nodiscard]] iconer::net::IoEvent AwaitForTask() noexcept;
	bool ProcessTask(iconer::net::IoEvent task);

private:
	iconer::net::Socket listenSocket;
	iconer::net::IoCompletionPort completionPort;

	std::array<std::jthread, workerCount> myWorkers;
	std::latch workerInitializationSynchronizer{ workerCount };

	std::unordered_map<iconer::app::PacketProtocol, EventDelegate> packetProcessors;

	Framework(const Framework&) = delete;
	Framework& operator=(const Framework&) = delete;
	Framework(Framework&&) = delete;
	Framework& operator=(Framework&&) = delete;
};

