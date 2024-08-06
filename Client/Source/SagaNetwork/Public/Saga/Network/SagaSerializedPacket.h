#pragma once
#include "SagaNetwork.h"
#include <HAL/Platform.h>
#include <UObject/ObjectMacros.h>
#include <Templates/UniquePtr.h>
#include <Templates/UnrealTemplate.h>

#include "SagaSerializedPacket.generated.h"

USTRUCT()
struct SAGANETWORK_API FSagaSerializedPacket
{
	GENERATED_BODY()

public:
	void* ptr;
};
