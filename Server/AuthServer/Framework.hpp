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
		using Result = std::expected<void, int>;

		void* iocpHandle{ nullptr };

		Server() = default;

		Result Initialize();
		Result Startup();
		void Cleanup();

		Result InitializeNetwork();
		Result InitializeSockets();
		Result InitializeIocp();
		bool InitializeBuffers();
		Result InitializeWorkers();
		bool InitializeExitHandlers();
		void ArriveWorkersIntialized() noexcept;

		Result BeginReceive();
		void EndReceive();
		Result BeginSend();
		void EndSend(class SendContext* context);

		void OnReceived(unsigned long bytes);
		void OnSent(class SendContext* context);

		Result RegisterSocket(std::uintptr_t socket, std::uintptr_t key) noexcept;

		friend void ReceiveHandler(std::stop_token&& canceller, auth::Server& instance);
		friend void DatabaseHandler(std::stop_token&& canceller, auth::Server& instance);
		friend void ExitHandler() noexcept;

	private:
		void PrintReceivedData() const;

		std::uintptr_t serverSocket{ invalidSocket };
		std::uintptr_t fileIoSocket{ invalidSocket };
		std::uintptr_t databaseSocket{ invalidSocket };

		std::array<std::jthread, iocpWorkerCount> myThreads;
		static std::latch workerInitializationSynchronizer;

		class IoContext* recvContext;
		std::unique_ptr<char[]> recvBuffer;
		unsigned long recvBytes;
		struct ::sockaddr_in* recvAddress;
	};

	void ReceiveHandler(std::stop_token&& canceller, auth::Server& instance);
	void DatabaseHandler(std::stop_token&& canceller, auth::Server& instance);
	void ExitHandler() noexcept;
};

