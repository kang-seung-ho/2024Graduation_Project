#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>

import <cstddef>;
import <cstdint>;
import <memory>;
import <optional>;
import <expected>;
import <array>;
import <vector>;
import <thread>;
import <stop_token>;
import <mutex>;
import <latch>;

class [[nodiscard]] Pipeline
{
public:
	std::unique_ptr<_OVERLAPPED> myContext;

	std::unique_ptr<char[]> recvBuffer;

	std::uint32_t recvOffset;
};

class Framework
{
public:
	static inline constexpr std::uint16_t serverPort = 40000;
	static inline constexpr std::size_t iocpWorkerCount = 4;
	static inline constexpr std::uintptr_t iocpMasterKey = 0;
	static inline constexpr std::uint32_t maxRecvSize = 512;

	void* iocpHandle;

	std::uintptr_t myListener;

	Framework() noexcept = default;
	~Framework() noexcept = default;

	std::expected<void, int> Initialize();

	std::expected<void, int> Startup();

	void ArriveWorkersIntialized() noexcept;
	void AwaitWork() noexcept;
	void NotifyWork() noexcept;

	std::expected<void, int> ReceiveFrom(Pipeline& pipeline, sockaddr_in& address);

	void Cleanup();

	friend void ExitHandler() noexcept;

private:
	std::array<std::jthread, iocpWorkerCount> myThreads;
	std::array<Pipeline, iocpWorkerCount> myPipelines;

	std::latch workerInitializationSynchronizer{ iocpWorkerCount };
	std::atomic_bool iocpBarrier;

	Framework(const Framework&) = delete;
	Framework& operator=(const Framework&) = delete;
	Framework(Framework&&) = delete;
	Framework& operator=(Framework&&) = delete;
};

void ExitHandler() noexcept;
