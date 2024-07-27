#pragma warning(push)
#pragma warning(disable : 4006)
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma warning(pop)

#include <cstdlib>
#include <cstring>
#include <ctime>

#include <bit>
#include <utility>
#include <string_view>
#include <string>
#include <print>

static inline constexpr std::uint16_t serverPort = 40000;

static inline constexpr std::size_t dummyCount = 7000;
constinit std::uintptr_t dummySockets[dummyCount]{};
constinit std::uintptr_t dummyClientIds[dummyCount]{};

constinit std::uintptr_t mySocket{ INVALID_SOCKET };

int main()
{
	std::println("Hello World!");

	::WSADATA version_data{};
	const int startup = ::WSAStartup(MAKEWORD(2, 2), std::addressof(version_data));
	if (0 != startup)
	{
		const auto error_code = ::WSAGetLastError();
		std::println("Error when startup the system: {}", error_code);

		return error_code;
	}

	mySocket = ::WSASocket(AF_INET, SOCK_DGRAM, ::IPPROTO::IPPROTO_UDP, nullptr, 0, 0);
	if (INVALID_SOCKET == mySocket)
	{
		const auto error_code = ::WSAGetLastError();
		std::println("Error when creating a socket: {}", error_code);

		return error_code;
	}

	std::println("The socket is created.");

	constexpr int reuse_address = true;
	if (0 != ::setsockopt(mySocket
		, SOL_SOCKET
		, SO_REUSEADDR
		, reinterpret_cast<const char*>(&reuse_address), sizeof(int)))
	{
		const auto error_code = ::WSAGetLastError();

		std::println("Error when setting up socket: {}", error_code);

		return error_code;
	}

	const ::SOCKADDR_IN server_address
	{
		.sin_family = AF_INET,
		.sin_port = ::htons(serverPort),
		.sin_addr = ::in4addr_loopback,
		.sin_zero{}
	};

	if (0 != WSAConnect(mySocket
		, reinterpret_cast<const SOCKADDR*>(&server_address), sizeof(SOCKADDR_IN)
		, nullptr, nullptr
		, nullptr, nullptr))
	{
		const auto error_code = ::WSAGetLastError();

		std::println("Error when connecting to the server: {}", error_code);

		return error_code;
	}

	std::println("Sending data.");

	std::srand((unsigned int)std::time(NULL));

	for (int i = 0; i < 100; ++i)
	{
		char send_data[33]{};
		::DWORD sent_bytes{};

		for (auto& data : send_data)
		{
			data = 'A' + std::rand() % 26;
		}
		send_data[32] = 0;

		::WSABUF send_wbuffer
		{
			.len = 32,
			.buf = send_data
		};

		const auto sent = ::WSASendTo(mySocket
			, &send_wbuffer, 1, &sent_bytes
			, 0
			, reinterpret_cast<const SOCKADDR*>(&server_address), sizeof(SOCKADDR_IN)
			, nullptr, nullptr);

		if (SOCKET_ERROR == sent)
		{
			std::println("(1) Error when sending data. Error code is {}.", WSAGetLastError());

			break;
		}
		else
		{
			std::println("(1) Data sent: {} ({} bytes)", send_data, sent_bytes);
		}

		char recv_data[512]{};
		::DWORD recv_bytes{};
		::DWORD recv_flags{ 0 };

		::WSABUF recv_wbuffer
		{
			.len = sizeof(recv_data) - 1,
			.buf = recv_data
		};

		::SOCKADDR_IN recv_address{};
		std::int32_t recv_address_len{ sizeof(SOCKADDR_IN) };

		const auto recv = ::WSARecvFrom(mySocket
			, &recv_wbuffer, 1, &recv_bytes
			, &recv_flags
			, reinterpret_cast<SOCKADDR*>(&recv_address), &recv_address_len
			, nullptr, nullptr);

		if (SOCKET_ERROR == recv)
		{
			std::println("(2) Error when receiving data. Error code is {}.", WSAGetLastError());

			break;
		}
		else
		{
			std::println("(2) Data recv: {} ({} bytes)", recv_data, recv_bytes);
		}

		if (0 < recv_bytes and
			sent_bytes == recv_bytes and
			0 == std::strcmp(send_data, recv_data))
		{
			std::println("(3) Data validated.");
		}
		else
		{
			std::println("(3) Not synchronized data.");
		}

		std::println("----------------------------------------");
	}

	std::system("pause");
}
