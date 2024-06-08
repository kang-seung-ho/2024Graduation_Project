#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>

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

	IoContext(IoCategory cat) noexcept
		: Super()
		, myCategory(cat)
	{}

	void Clear() noexcept
	{
		this->Internal = 0;
		this->InternalHigh = 0;
		this->Offset = 0;
		this->OffsetHigh = 0;
	}

	IoCategory myCategory;
};

