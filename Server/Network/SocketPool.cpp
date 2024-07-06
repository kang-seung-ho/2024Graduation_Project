module Iconer.Net.SocketPool;
import Iconer.Net;
import Iconer.Net.Socket;

iconer::net::SocketPool::~SocketPool()
noexcept
{
	Cleanup();
}

iconer::net::IoResult
iconer::net::SocketPool::Initialize(size_t socket_cnt)
{
	mySockets.reserve(socket_cnt);

	for (size_t i = 0; i < socket_cnt; ++i)
	{
		const auto socket_ptr = new Socket{ Socket::CreateTcpSocket(SocketCategory::Asynchronous) };

		if (nullptr == socket_ptr)
		{
			return std::unexpected{ iconer::net::ErrorCode::NOT_ENOUGH_MEMORY };
		}

		if (not *socket_ptr)
		{
			return std::unexpected{ AcquireNetworkError() };
		}

		mySockets.push_back(socket_ptr);
	}

	return iconer::net::IoResult();
}

void
iconer::net::SocketPool::Cleanup()
noexcept
{
	if (0 < mySockets.size())
	{
		for (auto& socket_ptr : mySockets)
		{
			socket_ptr->Close();

			delete socket_ptr;
		}

		mySockets.clear();
	}
}
