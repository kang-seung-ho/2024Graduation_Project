#pragma once
#include "SagaNetwork.h"

#include "SagaNetworkConnectionCategory.generated.h"

UENUM(BlueprintType)
enum class [[nodiscard]] ESagaNetworkConnectionCategory : uint8
{
	Unknown = 0,
	Local,
	Host,
	Remote,
};

namespace saga
{
	using SagaNetworkConnectionCategory = ::ESagaNetworkConnectionCategory;
}
