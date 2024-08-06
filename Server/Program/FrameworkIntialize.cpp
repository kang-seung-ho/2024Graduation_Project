module;
#include <print>

module Iconer.Framework;
import Iconer.Utility.Delegate;
import Iconer.Net.Socket;
import Iconer.App.User;
import Iconer.App.Room;
import Iconer.App.PacketContext;
import Iconer.App.SendContext;

iconer::net::IoResult
ServerFramework::Initialize()
{
	PrintLn("Starting server...");

	if (auto io = super::Initialize(); not io)
	{
		PrintLn("Could not initialize network system, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	if (auto io = super::GetListenSocket().BindToHost(serverPort); io)
	{
		PrintLn("The listen socket is bound to host:({}).", serverPort);
	}
	else
	{
		PrintLn("The listen socket could not be bound to host, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	if (auto io = super::GetListenSocket().ReusableAddress(true); io)
	{
		PrintLn("The listen socket now would recycle its address. ({})", super::GetListenSocket().ReusableAddress());
	}
	else
	{
		PrintLn("The listen socket could not be set to re-use its address, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	if (auto io = super::GetListenSocket().SetTcpNoDelay(false); io)
	{
		PrintLn("The listen socket would not use Nagle algorithm. ({})", super::GetListenSocket().IsTcpNoDelay());
	}
	else
	{
		PrintLn("The listen socket could not set Nagle algorithm, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	if (auto io = myTaskPool.Initialize(); io)
	{
		PrintLn("The task pool is created.");
	}
	else
	{
		PrintLn("Could not create task pool, due to {}.", std::to_string(io.error()));

		return std::unexpected{ io.error() };
	}

	if (auto io = myTaskPool.Register(super::GetListenSocket(), 0); io)
	{
		PrintLn("The listen socket is registered to the task pool.");
	}
	else
	{
		PrintLn("The listen socket was not able to be registered to the task pool, due to {}.", std::to_string(io.error()));

		return std::move(io);
	}

	using iconer::util::MakeInvoker;
	using iconer::util::MakeSharedInvoker;

	myTaskPool.eventOnTaskSucceed.Add(MakeInvoker(this, &ServerFramework::OnTaskSucceed));
	myTaskPool.eventOnTaskFailure.Add(MakeInvoker(this, &ServerFramework::OnTaskFailure));

	PrintLn("Allocating necessary memory...");

	for (unsigned i = 0; i < reservedPacketContextCount; ++i)
	{
		AddPacketContext(new iconer::app::PacketContext{});
	}

	for (unsigned i = 0; i < reservedSendContextCount; ++i)
	{
		AddSendContext(new iconer::app::SendContext{});
	}

	packetProcessors.reserve(100);

	using enum iconer::app::PacketProtocol;
	AddPacketProcessor(CS_SIGNIN, &ServerFramework::EventOnSignIn);

	AddPacketProcessor(CS_ROOM_CREATE, &ServerFramework::EventOnMakeRoom);
	AddPacketProcessor(CS_ROOM_JOIN, &ServerFramework::EventOnJoinRoom);
	AddPacketProcessor(CS_ROOM_LEAVE, &ServerFramework::EventOnExitRoom);
	AddPacketProcessor(CS_ROOM_MATCH, &ServerFramework::EventOnSeekRoom);
	AddPacketProcessor(CS_REQUEST_ROOMS, &ServerFramework::EventOnRoomList);

	AddPacketProcessor(CS_REQUEST_USERS, &ServerFramework::EventOnUserList);
	AddPacketProcessor(CS_SET_TEAM, &ServerFramework::EventOnChangeTeam);
	AddPacketProcessor(CS_GAME_START, &ServerFramework::EventOnGameStartSignal);
	AddPacketProcessor(CS_GAME_LOADED, &ServerFramework::EventOnGameReadySignal);

	AddPacketProcessor(CS_RPC, &ServerFramework::EventOnRpc);

	for (auto& rpc_processor : rpcProcessors)
	{
		rpc_processor = &ServerFramework::RpcEventDefault;
	}

	using enum iconer::app::RpcProtocol;
	AddRpcProcessor(RPC_DESTROY_ITEM_BOX, &ServerFramework::RpcEventOnItemBoxDestroyed);
	AddRpcProcessor(RPC_GRAB_ITEM, &ServerFramework::RpcEventOnItemGrabbed);
	AddRpcProcessor(RPC_USE_ITEM_0, &ServerFramework::RpcEventOnItemUsed);

	PrintLn("Generating {} users...", iconer::app::UserManager::maxUserCount);

	if (auto io = userManager.Initialize(myTaskPool.ioCompletionPort); not io)
	{
		PrintLn("The user manager has failed to initialize.");

		return iconer::util::Unexpected{ io.error() };
	}

	userManager.Foreach([this](iconer::app::User& user)
		{
			user.onDisconnected.Add(MakeInvoker(this, &ServerFramework::OnUserDisconnected));
		}
	);

	PrintLn("Generating {} rooms...", iconer::app::RoomManager::maxRoomCount);

	roomManager.Initialize();

	roomManager.Foreach([this](iconer::app::Room& room)
		{
			room.onUserLeft.Add(MakeInvoker(this, &ServerFramework::OnUserLeft));
			room.onDestroyed.Add(MakeInvoker(this, &ServerFramework::OnRoomClosed));
		}
	);

	return {};
}
