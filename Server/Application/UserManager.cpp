module;
#include <algorithm>

module Iconer.App.UserManager;
import Iconer.Net.ErrorCode;
import Iconer.Net.Socket;
import Iconer.Net.IoContext;
import <utility>;

std::expected<void, iconer::net::ErrorCode>
iconer::app::UserManager::Initialize(iconer::net::IoCompletionPort& io_port)
{
	using namespace iconer::net;

	ioCompletionPort = std::addressof(io_port);

	everyUsers.reserve(maxUserCount);

	for (id_type id = minUserUid; id < maxUserUid; ++id)
	{
		auto socket = Socket::CreateTcpSocket(SocketCategory::Asynchronous);

		if (not socket)
		{
			return std::unexpected{ AcquireNetworkError() };
		}

		if (auto io = ioCompletionPort->Register(socket, id); not io)
		{
			return std::move(io);
		}

		iconer::app::User* user = new iconer::app::User{ id, std::move(socket) };
		user->mainContext.ClearIoStatus();
		user->mainContext.SetOperation(iconer::app::TaskCategory::OpReserve);
		user->recvContext.ClearIoStatus();

		everyUsers.emplace_back(user);
	}

	return {};
}

void
iconer::app::UserManager::Startup(iconer::net::Socket& listener)
{
	std::sort(everyUsers.begin(), everyUsers.end()
		, [](const pointer_type& lhs, const pointer_type& rhs) noexcept -> bool
		{
			return lhs->GetID() < rhs->GetID();
		}
	);

	for (const pointer_type& ptr : everyUsers)
	{
		auto& user = *ptr;
		auto& ctx = user.mainContext;

		ioCompletionPort->Schedule(user.mainContext, user.GetID());
	}
}

void
iconer::app::UserManager::Cleanup()
{}

void
iconer::app::UserManager::AddSession(iconer::app::UserManager::session_type* session)
{
	everyUsers.emplace_back(session);
}

iconer::app::User*
iconer::app::UserManager::FindSession(iconer::app::UserManager::id_type id)
const noexcept
{
	auto seek = std::lower_bound(everyUsers.cbegin(), everyUsers.cend()
		, id
		, [](const pointer_type& lhs, const id_type& nid) noexcept -> bool
		{
			return lhs->GetID() < nid;
		}
	);

	if (seek != everyUsers.cend())
	{
		return seek->get();
	}
	else
	{
		return nullptr;
	}
}
