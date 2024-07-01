#include "Framework.hpp"

import Iconer.Utility.Delegate;
import Iconer.App.User;
import Iconer.App.PacketContext;
import Iconer.App.SendContext;
import <print>;

iconer::net::IoResult
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

	std::println("Allocating necessary memory...");

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

	for (unsigned i = 0; i < storedSendContexts.capacity(); ++i)
	{
		storedSendContexts.push(new iconer::app::SendContext{});
	}

	packetProcessors.reserve(100);

	packetProcessors.insert(std::make_pair(iconer::app::PacketProtocol::CS_SIGNIN, &ServerFramework::EventOnSignIn));

	using iconer::util::MakeInvoker;
	using iconer::util::MakeSharedInvoker;

	myTaskPool.eventOnTaskSucceed.Add(MakeInvoker(this, &ServerFramework::OnTaskSucceed));
	myTaskPool.eventOnTaskFailure.Add(MakeInvoker(this, &ServerFramework::OnTaskFailure));

	if (auto io = userManager.Initialize(myTaskPool.ioCompletionPort); not io)
	{
		std::println("The user manager has failed to initialize.");

		return iconer::util::Unexpected{ io.error() };
	}

	userManager.Foreach([this](iconer::app::User& user)
		{
			user.onDisconnected.Add(MakeInvoker(this, &ServerFramework::CleanupUser));
		}
	);

	return {};
}
