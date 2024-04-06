#include "Saga/String/SagaStringBpFunctionLibrary.h"

FString
USagaStringBpFunctionLibrary::SubString(const FString& string, int32 first_index, int32 last_index)
{
	if (first_index < 0)
	{
		UE_LOG(LogScript, Error, TEXT("The first index is less than zero in `SubString`."));
		return FString{};
	}
	
	if (last_index <= 0)
	{
		UE_LOG(LogScript, Error, TEXT("The last index is less than/equal to zero in `SubString`."));
		return FString{};
	}

	const int32 length = string.Len();
	if (length <= first_index)
	{
		UE_LOG(LogScript, Error, TEXT("The first index is greater than input string in `SubString`."));
		return FString{};
	}

	const int32 sublength = last_index - first_index;
	if (sublength <= 0)
	{
		UE_LOG(LogScript, Error, TEXT("Invalid subscript index found at `SubString`."));
		return FString{};
	}

	FString result{};
	result.Reserve(sublength + 1);

	for (int32 i = first_index; i < last_index; ++i)
	{
		if (length <= i) break;

		const auto& chr = string[i];
		result += chr;
	}

	return MoveTemp(result);
}
