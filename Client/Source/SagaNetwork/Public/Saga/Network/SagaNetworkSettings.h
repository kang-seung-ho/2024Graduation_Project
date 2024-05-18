#pragma once
#include "Containers/UnrealString.h"
#include "Saga/Network/SagaNetworkConnectionCategory.h"

namespace saga
{
	inline constexpr ESagaNetworkConnectionCategory ConnectionCategory = ESagaNetworkConnectionCategory::Host;
	inline const FString RemoteAddress = TEXT("127.0.0.1");
	inline constexpr int32 RemotePort = 40000U;
	inline constexpr int32 LocalPort = 40001U;
	inline constexpr bool IsOfflineMode = false;
};
