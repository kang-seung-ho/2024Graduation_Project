module;
module Iconer.Net.IFramework;
import Iconer.Net;
import <print>;

std::expected<void, iconer::net::ErrorCode>
iconer::net::IFramework::Initialize()
{
	if (auto io = iconer::net::Startup(); io)
	{
		std::println("The network system is Initiated.");
	}
	else
	{
		std::println("Could not initialize network system, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	listenSocket = Socket::CreateTcpSocket(SocketCategory::Asynchronous, IpAddressFamily::IPv4);

	if (listenSocket)
	{
		std::println("The listen socket is created.");
	}
	else
	{
		const auto error = AcquireNetworkError();

		std::println("The listen socket was not created, due to {}.", std::to_string(error));

		return std::unexpected{ error };
	}

	if (eventOnInitialied.IsBound())
	{
		eventOnInitialied.Broadcast();
	}

	if (eventOnPostInitialied.IsBound())
	{
		eventOnPostInitialied.Broadcast();
	}

	return {};
}

void
iconer::net::IFramework::Startup()
{
	if (eventOnPreStarted.IsBound())
	{
		eventOnPreStarted.Broadcast();
	}
}

void
iconer::net::IFramework::PostStartup()
{
	if (eventOnPostStarted.IsBound())
	{
		eventOnPostStarted.Broadcast();
	}
}

void
iconer::net::IFramework::Cleanup()
{
	listenSocket.Close();

	if (eventOnDestructed.IsBound())
	{
		eventOnDestructed.Broadcast();
	}
}
