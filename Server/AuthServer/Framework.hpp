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

#define UNLIKELY [[unlikely]]

struct sockaddr_in;

namespace auth
{
	static inline constexpr std::uint16_t serverPort = 40000;
	static inline constexpr std::size_t iocpWorkerCount = 4;
	static inline constexpr std::uintptr_t iocpMasterKey = 0;
	static inline constexpr std::uint32_t maxRecvSize = 512;
	static inline constexpr std::uintptr_t invalidSocket = static_cast<std::uintptr_t>(-1);

	class Server
	{
	public:
		void* iocpHandle{ nullptr };

		Server() = default;

		std::expected<void, int> Initialize();
		std::expected<void, int> Startup();
		void Cleanup();

		void ArriveWorkersIntialized() noexcept;

		std::expected<void, int> BeginReceive();
		void EndReceive();

		friend void ReceiveHandler(std::stop_token&& canceller, auth::Server& instance);
		friend void DatabaseHandler(std::stop_token&& canceller, auth::Server& instance);
		friend void ExitHandler() noexcept;

	private:
		std::uintptr_t serverSocket{ invalidSocket };
		std::uintptr_t fileIoSocket{ invalidSocket };
		std::uintptr_t databaseSocket{ invalidSocket };

		std::array<std::jthread, iocpWorkerCount> myThreads;

		std::unique_ptr<char[]> recvBuffer;
		struct ::sockaddr_in* recvAddress;
	};

	void ReceiveHandler(std::stop_token&& canceller, auth::Server& instance);
	void DatabaseHandler(std::stop_token&& canceller, auth::Server& instance);
	void ExitHandler() noexcept;
};

