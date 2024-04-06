#pragma once
#include "SagaNetwork.h"

namespace saga
{
	class SAGANETWORK_API USagaNetwork final
	{
	public:
		[[nodiscard]] static bool Start(FStringView nickname) noexcept { return true; }
	};
}
