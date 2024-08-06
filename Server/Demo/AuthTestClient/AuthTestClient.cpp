#pragma warning(push)
#pragma warning(disable : 4006)
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <WS2tcpip.h>
#undef socket
#pragma warning(pop)

#include <cstdlib>
#include <cstring>
#include <ctime>

#include <bit>
#include <utility>
#include <string_view>
#include <string>
#include <print>

namespace
{
	inline constexpr std::uint16_t serverPort = 40000;

	inline constexpr std::size_t dummyCount = 6000;
	inline constexpr std::size_t dummyRoomCount = dummyCount / 4;

	constinit std::uintptr_t dummySockets[dummyCount]{};
	constinit std::int32_t dummyClientIds[dummyCount]{};
	constinit std::int32_t dummyRoomIds[dummyRoomCount]{};

	constinit char recv_buffer_g[513]{};

	constinit ::WSABUF recv_wbuffer_g
	{
		.len = sizeof(recv_buffer_g) - 1,
		.buf = recv_buffer_g
	};

	constinit DWORD recv_g_bytes{};
	constinit DWORD recv_g_flags{ 0 };

	u_long nonBlockingMode = 1;
	u_long blockingMode = 0;

	void Pause() noexcept
	{
		std::system("pause");
	}
}

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

	for (auto& socket : dummySockets)
	{
		socket = ::WSASocket(AF_INET, SOCK_STREAM, ::IPPROTO::IPPROTO_TCP, nullptr, 0, 0);

		if (INVALID_SOCKET == socket)
		{
			const auto error_code = ::WSAGetLastError();

			std::println("Error when creating a socket: {}", error_code);

			Pause();
			return error_code;
		}
	}

	std::println("{} sockets are created.", dummyCount);
	std::println("----------------------------------------");

	constexpr int reuse_address = true;

	const ::SOCKADDR_IN server_address
	{
		.sin_family = AF_INET,
		.sin_port = ::htons(serverPort),
		.sin_addr = ::in4addr_loopback,
		.sin_zero{}
	};

	for (auto& socket : dummySockets)
	{
		if (0 != WSAConnect(socket
			, reinterpret_cast<const SOCKADDR*>(&server_address), sizeof(SOCKADDR_IN)
			, nullptr, nullptr
			, nullptr, nullptr))
		{
			const auto error_code = ::WSAGetLastError();

			std::println("Error when socket {} is connecting to the server: {}", socket, error_code);

			Pause();
			return error_code;
		}
	}
	std::println("----------------------------------------");
	std::println("(1) Sending sign-in data...");

	std::srand((unsigned int)std::time(NULL));

	char signin_buffer[35] =
	{
		1, 35, 0, 'n', 0, 'i', 0, 'c', 0, 'k', 0, 'n', 0, 'a', 0, 'm', 0, 'e', 0
	};

	::WSABUF send_wbuffer
	{
		.len = 35,
		.buf = signin_buffer
	};

	DWORD sent_bytes{};

	for (auto& socket : dummySockets)
	{
		WSASend(socket, &send_wbuffer, 1, &sent_bytes, 0, nullptr, nullptr);
	}
	std::println("(1) Complete sending sign-in data");
	std::println("----------------------------------------");

	size_t index = 0;
	for (auto& socket : dummySockets)
	{
		::SOCKADDR_IN recv_address{};
		std::int32_t recv_address_len{ sizeof(SOCKADDR_IN) };

		const auto recv = ::WSARecv(socket
			, &recv_wbuffer_g, 1, &recv_g_bytes
			, &recv_g_flags
			, nullptr, nullptr);

		if (SOCKET_ERROR == recv)
		{
			std::println("(2) Error when receiving data at {}. Error code is {}.", socket, WSAGetLastError());

			Pause();
			break;
		}
		else
		{
			auto& curr_id = dummyClientIds[index];

			curr_id = *reinterpret_cast<const std::int32_t*>(recv_buffer_g + 3);

			//std::println("(2) Id recv from {}: {}", socket, curr_id);
		}

		(void)++index;
	}
	index = 0;

	std::println("----------------------------------------");
	std::println("(3) Creating {} rooms...", dummyRoomCount);

	char room_creation_pk_buffer[35] =
	{
		7, 35, 0, 't', 0, 'e', 0, 's', 0, 't', 0, 'r', 0, 'o', 0, 'o', 0, 'm', 0
	};

	send_wbuffer.buf = room_creation_pk_buffer;

	for (size_t i = 0; i < dummyRoomCount; ++i)
	{
		auto& socket = dummySockets[index];

		WSASend(socket, &send_wbuffer, 1, &sent_bytes, 0, nullptr, nullptr);

		const auto recv = ::WSARecv(socket
			, &recv_wbuffer_g, 1, &recv_g_bytes
			, &recv_g_flags
			, nullptr, nullptr);

		if (SOCKET_ERROR == recv)
		{
			std::println("(3) Error when receiving room data at {}. Error code is {}.", socket, WSAGetLastError());

			Pause();
			break;
		}
		else
		{
			auto& curr_id = dummyRoomIds[i];

			curr_id = *reinterpret_cast<const std::int32_t*>(recv_buffer_g + 3);

			//std::println("(3) Room {} created by {}", curr_id, socket);
		}

		index += 4;
	}

	index = 0;

	std::println("----------------------------------------");
	std::println("(4) Entering to the rooms...");

	char room_enter_pk_buffer[7] =
	{
		9, 7, 0,
	};

	send_wbuffer.buf = room_enter_pk_buffer;
	send_wbuffer.len = 7;

	for (size_t i = 0; i < dummyRoomCount;)
	{
		auto& socket = dummySockets[index];
		auto& room_id = dummyRoomIds[i];

		if (index % 4 == 0)
		{
			memcpy(room_enter_pk_buffer + 3, &room_id, 4);

			(void)++i;
			(void)++index;
			continue;
		}
		else
		{
			(void)++index;
		}

		WSASend(socket, &send_wbuffer, 1, &sent_bytes, 0, nullptr, nullptr);

		//std::println("(4) User {} is entering to room {}", socket, room_id);

		const auto recv = ::WSARecv(socket
			, &recv_wbuffer_g, 1, &recv_g_bytes
			, &recv_g_flags
			, nullptr, nullptr);

		if (SOCKET_ERROR == recv)
		{
			std::println("(4) Error when receiving join result data at {}. Error code is {}.", socket, WSAGetLastError());

			Pause();
			break;
		}
		else
		{
			//auto& curr_id = dummyRoomIds[index];

			//curr_id = *reinterpret_cast<const std::int32_t*>(recv_buffer_g + 3);

			//std::println("(4) User {} joined to room {}", socket, room_id);
		}
	}

	index = 0;

	Sleep(1000);
	std::println("----------------------------------------");
	std::println("(5) Starting games...");

	char game_start_pk_buffer[3] =
	{
		14, 3, 0,
	};

	send_wbuffer.buf = game_start_pk_buffer;
	send_wbuffer.len = 3;

	for (size_t i = 0; i < dummyRoomCount;)
	{
		auto& socket = dummySockets[index];
		auto& room_id = dummyRoomIds[i];

		if (index % 4 == 0)
		{
			//std::println("(5) User {} is starting game at room {}", socket, room_id);

			WSASend(socket, &send_wbuffer, 1, &sent_bytes, 0, nullptr, nullptr);

			(void)++i;
			(void)++index;
			continue;
		}
		else
		{
			(void)++index;
		}
	}

	index = 0;

	std::println("----------------------------------------");
	std::println("(6) Getting game tickets...");

	for (auto& socket : dummySockets)
	{
		ioctlsocket(socket, FIONBIO, &nonBlockingMode);
	}

	for (auto& socket : dummySockets)
	{
		bool done = false;

		//while (true)
		{
			const auto recv = ::WSARecv(socket
				, &recv_wbuffer_g, 1, &recv_g_bytes
				, &recv_g_flags
				, nullptr, nullptr);

			if (SOCKET_ERROR == recv)
			{
				const auto err = WSAGetLastError();

				if (10035 == err)
				{
					//continue;
				}
				else
				{
					std::println("(6) Error when receiving data at {}. Error code is {}.", socket, err);
					//break;
				}
			}
			else
			{
				//std::println("(6) User {} got a ticket ({} bytes)", socket, recv_g_bytes);

				//break;
			}
		}
	}

	for (auto& socket : dummySockets)
	{
		ioctlsocket(socket, FIONBIO, &blockingMode);
	}

	index = 0;

	std::println("----------------------------------------");
	std::println("(7) Preparing games...");

	char game_load_pk_buffer[3] =
	{
		15, 3, 0,
	};

	send_wbuffer.buf = game_load_pk_buffer;

	for (auto& socket : dummySockets)
	{
		WSASend(socket, &send_wbuffer, 1, &sent_bytes, 0, nullptr, nullptr);
	}

	std::println("----------------------------------------");
	std::println("(8) Launching games...");

	for (auto& socket : dummySockets)
	{
		ioctlsocket(socket, FIONBIO, &nonBlockingMode);
	}

	for (size_t i = 0; i < dummyCount; ++i)
	{
		auto& socket = dummySockets[i];

		while (true)
		{
			const auto recv = ::WSARecv(socket
				, &recv_wbuffer_g, 1, &recv_g_bytes
				, &recv_g_flags
				, nullptr, nullptr);

			if (SOCKET_ERROR == recv)
			{
				const auto err = WSAGetLastError();

				if (10035 == err)
				{
					continue;
				}
				else
				{
					std::println("(8) Error when receiving game state data at {}. Error code is {}.", socket, WSAGetLastError());

					break;
				}
			}
			else
			{
				std::println("(8) User {} got game messages ({} bytes)", socket, recv_g_bytes);

				break;
			}
		}
	}

	for (auto& socket : dummySockets)
	{
		//ioctlsocket(socket, FIONBIO, &blockingMode);
	}

	std::println("----------------------------------------");
	std::println("(9) Games Phase");

	while (true)
	{
		auto& socket = dummySockets[std::rand() % dummyCount];

		std::uint8_t rpc{};
		std::int64_t pa1{};
		std::int32_t pa2{};
	}

	std::println("----------------------------------------");
	Pause();
}
