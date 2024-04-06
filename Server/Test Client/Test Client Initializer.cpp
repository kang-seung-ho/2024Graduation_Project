module;
#include <print>
module TestClient;

int
test::Awake()
{
	std::println("Client initiated");

	std::println("Creating a socket...");
	auto startup_r = iconer::net::Startup();
	if (startup_r.has_value())
	{
		return 5;
	}

	app_socket = iconer::net::Socket::CreateTcpSocket(iconer::net::IoCategory::Asynchronous);

	std::println("Binding...");

	app_socket.IsAddressReusable = true;

	auto client_address = iconer::net::IpAddress{ iconer::net::IpAddressFamily::IPv4, "127.0.0.1" };
	auto client_ep = iconer::net::EndPoint{ client_address, 40001U };

	// NOTICE: 클라이언트는 바인드 금지
	return 0;
}

int
test::Start()
{
	std::println("Connecting to host...");
	server_address = iconer::net::IpAddress{ iconer::net::IpAddressFamily::IPv4, "127.0.0.1" };
	server_ep = iconer::net::EndPoint{ server_address, server_port };

	//auto connect_r = app_socket.Connect(server_ep);
	//auto connect_r = app_socket.ConnectToAny(server_port);
	auto connect_r = app_socket.ConnectToHost(server_port);
	if (connect_r.has_value())
	{
		return 3;
	}

	std::println("Sending nickname...");

	if (auto sent_signin_r = SendSignInPacket(); not sent_signin_r.has_value())
	{
		return 2;
	}

	return 0;
}
