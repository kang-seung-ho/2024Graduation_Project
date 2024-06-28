#include "Framework.hpp"

import Iconer.Utility.Delegate;
import Iconer.App.User;
import Iconer.App.PacketContext;
import <print>;

std::expected<void, iconer::net::ErrorCode>
ServerFramework::Initialize()
{
	std::println("Starting server...");

	if (auto io = super::Initialize(); not io)
	{
		std::println("Could not initialize network system, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	if (auto io = super::listenSocket.BindToHost(serverPort); io)
	{
		std::println("The listen socket is bound to host:({}).", serverPort);
	}
	else
	{
		std::println("The listen socket could not be bound to host, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	if (auto io = super::listenSocket.ReusableAddress(true); io)
	{
		std::println("The listen socket now would recycle its address. ({})", super::listenSocket.ReusableAddress());
	}
	else
	{
		std::println("The listen socket could not be set to re-use its address, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	if (auto io = super::listenSocket.SetTcpNoDelay(false); io)
	{
		std::println("The listen socket would not use Nagle algorithm. ({})", super::listenSocket.IsTcpNoDelay());
	}
	else
	{
		std::println("The listen socket could not set Nagle algorithm, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	if (auto io = myTaskPool.Initialize(); io)
	{
		std::println("The task pool is created.");
	}
	else
	{
		std::println("Could not create task pool, due to {}.", std::to_string(io.error()));

		return std::unexpected{ io.error() };
	}

	if (auto io = myTaskPool.Register(super::listenSocket, 0); io)
	{
		std::println("The listen socket is registered to the task pool.");
	}
	else
	{
		std::println("The listen socket was not able to be registered to the task pool, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	for (unsigned i = 0; i < storedPacketContexts.capacity(); ++i)
	{
		storedPacketContexts.push(new iconer::app::PacketContext{});
	}

	using iconer::util::MakeInvoker;
	using iconer::util::MakeSharedInvoker;

	myTaskPool.eventOnTaskSucceed.Add(MakeInvoker(this, &ServerFramework::OnTaskSucceed));
	myTaskPool.eventOnTaskFailure.Add(MakeInvoker(this, &ServerFramework::OnTaskFailure));

	if (not userManager.Initialize(myTaskPool.ioCompletionPort))
	{
		std::println("The user manager has failed to initialize.");

		return std::unexpected{ iconer::net::ErrorCode::WSAEUSERS };
	}

	userManager.Foreach([this](iconer::app::User& user)
		{
			user.onDisconnected.Add(MakeInvoker(this, &ServerFramework::CleanupUser));
		}
	);

	return {};
}
