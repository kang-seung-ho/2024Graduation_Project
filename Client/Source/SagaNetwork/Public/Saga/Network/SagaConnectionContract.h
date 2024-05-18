#pragma once
#include "SagaNetwork.h"

#include "SagaConnectionContract.generated.h"

UENUM(BlueprintType)
enum class [[nodiscard]] ESagaConnectionContract : uint8
{
	Success = 0
	, NoSocket
	, SignInFailed
	, CannotCreateWorker
	, ConnectError
	, WrongAddress
	, OtherError
	, SubSystemError
};
