export module Iconer.Net.IoEvent;
export import Iconer.Net.IoContext;
import <cstdint>;

export namespace iconer::net
{
	struct IoEvent
	{
		bool isSucceed;
		std::uint64_t eventId;
		unsigned long ioBytes;
		IoContext* ioContext;
	};
}
