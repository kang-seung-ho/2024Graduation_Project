#pragma once
#include "SagaNetwork.h"

#include "SagaVirtualSession.generated.h"

USTRUCT(BlueprintType, Atomic, Category = "CandyLandSaga|Level|UI|Element")
struct SAGANETWORK_API FSagaVirtualSession
{
	GENERATED_BODY()

public:
	void Name(const FString& name)
	{
		if (name.Len() == 0)
		{
			MyName.Reset();
		}
		else
		{
			MyName = name;
		}
	}
	
	void Name(FStringView name)
	{
		if (name.Len() == 0)
		{
			MyName.Reset();
		}
		else
		{
			MyName = name;
		}
	}

	[[nodiscard]]
	constexpr int32 ID() const noexcept
	{
		return MyID;
	}

	[[nodiscard]]
	const FString& Name() const noexcept
	{
		return MyName;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Network|Session", meta = (NoResetToDefault, NoSpinbox = true, ClampMin = 0, UIMin = 0, ClampMax = 21, UIMax = 21))
	int32 MyID = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Network|Session")
	FString MyName = TEXT("Empty Client");
};

struct SAGANETWORK_API FSagaSessionComparator final
{
	[[nodiscard]]
	constexpr bool
		operator()(const FSagaVirtualSession& lhs, const FSagaVirtualSession& rhs)
		const noexcept
	{
		return lhs.MyID == rhs.MyID;
	}

	[[nodiscard]]
	constexpr bool
		operator()(const FSagaVirtualSession& lhs, const int32& id)
		const noexcept
	{
		return lhs.MyID == id;
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
		return lhs.MyID == cmpId;
	}

	int32 cmpId;
};
