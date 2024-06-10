import Iconer.Net;
import Iconer.Net.EndPoint;
import Iconer.Net.Socket;
import Iconer.Net.IoCompletionPort;
import <cstdio>;
import <cstdlib>;
import <print>;

using namespace iconer;
using namespace iconer::net;

constinit Socket serverSocket;
constinit IoCompletionPort ioPort;

inline constexpr std::uint16_t serverPort = 40000;

int main()
{
	std::println("Starting server...");

	if (const auto startup_state = Startup(); startup_state)
	{
		std::println("Server is Initiated.");
	}
	else
	{
		std::println("Could not initialize this server, due to {}.", std::to_string(startup_state.error()));

		return 1;
	}

	//

	serverSocket = Socket::CreateUdpSocket(SocketCategory::Asynchronous, IpAddressFamily::IPv4);

	if (serverSocket)
	{
		std::println("The listen socket is created.");
	}
	else
	{
		std::println("The listen socket was not created, due to {}.", std::to_string(AcquireNetworkError()));

		return 1;
	}

	if (const auto io = serverSocket.BindToHost(serverPort); io)
	{
		std::println("The listen socket is bound to host:({}).", serverPort);
	}
	else
	{
		std::println("The listen socket could not be bound to host, due to {}.", std::to_string(io.error()));

		return 1;
	}

	if (const auto io = serverSocket.ReusableAddress(true); io)
	{
		std::println("The listen socket now would recycle its address.");
	}
	else
	{
		std::println("The listen socket could not be set to re-use its address, due to {}.", std::to_string(io.error()));

		return 1;
	}
	
	if (auto io = IoCompletionPort::Create(); io)
	{
		std::println("The iocp is created.");

		ioPort = std::move(io.value());
	}
	else
	{
		std::println("Could not create iocp, due to {}.", std::to_string(io.error()));

		return 1;
	}

	if (auto io = ioPort.Register(serverSocket, 0); io)
	{
		std::println("The listen socket is registered to the iocp.");
	}
	else
	{
		std::println("The listen socket was not able to be registered to the iocp, due to {}.", std::to_string(io.error()));

		return 1;
	}

	std::println("Server is started.");

	char input_buffer[128]{};
	constexpr uint32_t input_length = sizeof(input_buffer);

	while (true)
	{
		const auto input = scanf_s("%s", input_buffer, input_length);

		if (EOF != input)
		{
			if (input_buffer[0] == 'q')
			{
				break;
			}
		}
	}

	std::println("Closing server...");

	Cleanup();

	return 0;
}
