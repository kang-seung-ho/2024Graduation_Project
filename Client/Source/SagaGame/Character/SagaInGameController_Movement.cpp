#include "SagaInGamePlayerController.h"
#include "Math/UnrealMathUtility.h"

void
ASagaInGamePlayerController::SerializePosition(const FVector& vector, int64& arg0, int32& arg1)
{
	const auto x = static_cast<float>(vector.X);
	const auto y = static_cast<float>(vector.Y);
	const auto z = static_cast<float>(vector.Z);

	memcpy(reinterpret_cast<char*>(&arg0), &x, 4);
	memcpy(reinterpret_cast<char*>(&arg0) + 4, &y, 4);
	memcpy(reinterpret_cast<char*>(&arg1), &z, 4);
}

FVector
ASagaInGamePlayerController::DeserializePosition(const int64& arg0, const int32& arg1)
{
	float x{};
	float y{};
	float z{};

	std::memcpy(&x, &arg0, 4);
	std::memcpy(&y, reinterpret_cast<const char*>(&arg0) + 4, 4);
	std::memcpy(&z, &arg1, 4);

}
