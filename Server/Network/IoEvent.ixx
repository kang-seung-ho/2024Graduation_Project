export module Iconer.Net.IoEvent;
import <cstdint>;

export namespace iconer::net
{
	struct [[nodiscard]] IoEvent
	{
		class IoContext* ioContext;
		std::uint64_t eventId;
		unsigned long ioBytes;
		bool isSucceed;
	};
}
