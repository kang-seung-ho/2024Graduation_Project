#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>

import <new>;
import <memory>;

namespace auth
{
	enum class IoCategory
	{
		None = 0,

		Recv,
		Send,

		CheckUser,
	};

	class IoContext : public _OVERLAPPED
	{
	public:
		using Super = _OVERLAPPED;

		IoContext() noexcept = default;

		constexpr IoContext(IoCategory cat) noexcept
			: Super()
			, myCategory(cat)
		{}

		constexpr void Clear() noexcept
		{
			this->Internal = 0;
			this->InternalHigh = 0;
			this->Offset = 0;
			this->OffsetHigh = 0;
		}

		IoCategory myCategory;
	};

	class SendContext : public IoContext
	{
	public:
		using Super = IoContext;

		constexpr SendContext() noexcept
			: IoContext(IoCategory::Send)
			, myBuffer(nullptr)
		{}

		std::unique_ptr<char[]> myBuffer;
	};
}
