#pragma once
#include "Containers/UnrealString.h"
#include "Saga/Network/SagaNetworkConnectionCategory.h"

namespace saga
{
	inline constexpr ESagaNetworkConnectionCategory ConnectionCategory = ESagaNetworkConnectionCategory::Remote;
	inline const FString RemoteAddress = TEXT("61.84.90.149");
	inline constexpr int32 RemotePort = 40000U;
	inline constexpr int32 LocalPort = 40001U;
	inline constexpr bool IsOfflineMode = false;
};
