module;
#include <WinSock2.h>

export module Iconer.Net.IoContext;
import <cstdint>;
import <new>;
import <utility>;

export namespace iconer::net
{
	class [[nodiscard]] IoContext : public WSAOVERLAPPED
	{
	public:
		using Super = WSAOVERLAPPED;

		explicit constexpr IoContext() noexcept
			: Super()
		{
			ClearIoStatus();
		}

		constexpr void ClearIoStatus() noexcept
		{
			Internal = 0;
			InternalHigh = 0;
			Offset = 0;
			OffsetHigh = 0;
		}

		constexpr void ClearIoStatus() volatile noexcept
		{
			Internal = 0;
			InternalHigh = 0;
			Offset = 0;
			OffsetHigh = 0;
		}

		[[nodiscard]]
		constexpr bool operator==(const IoContext& other) const noexcept
		{
			return std::addressof(other) == this;
		}

	private:
		IoContext(const IoContext&) = delete;
		IoContext& operator=(const IoContext&) = delete;
	};
}
