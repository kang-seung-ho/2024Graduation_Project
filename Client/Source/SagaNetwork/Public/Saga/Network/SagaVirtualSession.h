#pragma once
#include "SagaNetwork.h"

#include "SagaVirtualSession.generated.h"

USTRUCT(BlueprintType, Atomic, Category = "CandyLandSaga|Network|Session")
struct SAGANETWORK_API FSagaVirtualSession
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Network|Session", meta = (NoResetToDefault, NoSpinbox = true, ClampMin = 0, UIMin = 0, ClampMax = 21, UIMax = 21))
	int32 myID = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Network|Session")
	FText myName = FText::GetEmpty();
};

struct SAGANETWORK_API FSagaSessionComparator final
{
	[[nodiscard]]
	constexpr bool
		operator()(const FSagaVirtualSession& lhs, const FSagaVirtualSession& rhs)
		const noexcept
	{
		return lhs.myID == rhs.myID;
	}

	[[nodiscard]]
	constexpr bool
		operator()(const FSagaVirtualSession& lhs, const int32& id)
		const noexcept
	{
		return lhs.myID == id;
	}
};

struct SAGANETWORK_API FSagaSessionIdComparator final
{
	explicit constexpr FSagaSessionIdComparator(int32 another_id) noexcept
		: cmpId(another_id)
	{}

	[[nodiscard]]
	constexpr bool
		operator()(const FSagaVirtualSession& lhs)
		const noexcept
	{
		return lhs.myID == cmpId;
	}

	int32 cmpId;
};
