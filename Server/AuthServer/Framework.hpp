#pragma once
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

class Framework
{
public:
	static inline constexpr std::uint16_t serverPort = 40000;
	static inline constexpr std::size_t iocpWorkerCount = 4;
	static inline constexpr std::uintptr_t iocpMasterKey = 0;
	static inline constexpr std::uint32_t maxRecvSize = 512;

	void* iocpHandle;

	Framework() noexcept = default;
	~Framework() noexcept = default;

	std::expected<void, int> Initialize();
	std::expected<void, int> Startup();
	void Cleanup();

	void ArriveWorkersIntialized() noexcept;
	void AwaitWork() noexcept;
	void NotifyWork() noexcept;

	std::expected<void, int> BeginReceive();
	void EndReceive();

	friend void ExitHandler() noexcept;

private:
	std::array<std::jthread, iocpWorkerCount> myThreads;

	std::latch workerInitializationSynchronizer{ iocpWorkerCount };
	std::atomic_bool iocpBarrier;

	std::uintptr_t myListener;
	struct sockaddr_in* recvAddress;
	std::unique_ptr<char[]> recvBuffer;
	std::atomic_int recvBarrier;

	Framework(const Framework&) = delete;
	Framework& operator=(const Framework&) = delete;
	Framework(Framework&&) = delete;
	Framework& operator=(Framework&&) = delete;
};

void ExitHandler() noexcept;
