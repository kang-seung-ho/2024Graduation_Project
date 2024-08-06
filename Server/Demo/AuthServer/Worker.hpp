#pragma once
import <stop_token>;

namespace auth
{
	class Server;
}

void Worker(std::stop_token&& canceller, const size_t index, class auth::Server& instance);
