#pragma once
#include "CoreMinimal.h"

#include "SagaGameContract.generated.h"

UENUM(BlueprintType)
enum class [[nodiscard]] ESagaGameContract : uint8
{
	Unknown = 0
	, NotInRoom
	, ClientIsBusy // The client's state is unmatched
	, LackOfMember
	, InvalidRoom
	, InvalidOperation // Room task is invalid
	, UnstableRoom // Room's state is changed at time
	, OtherClientFailed // Other member(s) in the room could not prepare the game
	, ServerError // Unknown internal error
};
