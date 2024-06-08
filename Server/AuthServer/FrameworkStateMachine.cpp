#include "Framework.hpp"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>

import <print>;

void
auth::ReceiveHandler(std::stop_token&& canceller, auth::Server& instance)
{
	std::println("Receive worker is generated.");

	instance.ArriveWorkersIntialized();

	if (canceller.stop_requested())
	{
		goto finished;
	}

	if (auto first_recv = instance.BeginReceive(); not first_recv)
	{
		std::println("A receive error occured at first, error code: {}", first_recv.error());

		goto finished;
	}

	instance.EndReceive();

	while (true)
	{
		if (canceller.stop_requested()) UNLIKELY
		{
			goto finished;
		};

		// continue
		const auto recv = instance.BeginReceive();

		if (not recv)
		{
			const auto error_code = recv.error();
			std::println("A receive error occured, error code: {}", error_code);

			instance.EndReceive();

			goto finished;
		}

		instance.EndReceive();
	}

finished:
	std::println("Receive worker is terminated.");
}

void
auth::DatabaseHandler(std::stop_token&& canceller, auth::Server& instance)
{
	std::println("Database worker is generated.");

	instance.ArriveWorkersIntialized();

	while (true)
	{
		if (canceller.stop_requested()) UNLIKELY
		{
			goto finished;
		}


	}

finished:
	std::println("Database worker is terminated.");
}
