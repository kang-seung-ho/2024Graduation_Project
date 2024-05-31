#pragma once
#include "Saga/Network/SagaVirtualSession.h"

#include "SagaVirtualUser.generated.h"

enum class [[nodiscard]] EUserTeam : uint8;
enum class [[nodiscard]] EPlayerWeapon : uint8;

USTRUCT(BlueprintType, Atomic, Category = "CandyLandSaga|Network|Session")
struct SAGANETWORK_API FSagaVirtualUser : public FSagaVirtualSession
{
	GENERATED_BODY()

public:
	void SetCharacterHandle(class ASagaCharacterPlayer* const character) noexcept
	{
		remoteCharacter = character;
	}

	[[nodiscard]]
	class ASagaCharacterPlayer* GetCharacterHandle() const noexcept
	{
		return remoteCharacter;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Network|Session")
	TObjectPtr<class ASagaCharacterPlayer> remoteCharacter{};
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "CandyLandSaga|Network|Session")
	EUserTeam myTeam{};
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "CandyLandSaga|Network|Session")
	EPlayerWeapon myWeapon{};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Network|Session")
	float myHealth{ 100.0f };
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Network|Session")
	double myX{};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Network|Session")
	double myY{};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Network|Session")
	double myZ{};
};
