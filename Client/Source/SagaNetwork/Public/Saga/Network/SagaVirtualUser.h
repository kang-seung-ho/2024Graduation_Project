#pragma once
#include "Saga/Network/SagaVirtualSession.h"
#include "SagaGame/Player/SagaUserTeam.h"
#include "SagaGame/Player/SagaPlayerWeaponTypes.h"

#include "SagaVirtualUser.generated.h"

USTRUCT(BlueprintType, Atomic, Category = "CandyLandSaga|Level|UI|Element")
struct SAGANETWORK_API FSagaVirtualUser : public FSagaVirtualSession
{
	GENERATED_BODY()

public:
	void SetCharacterHandle(class ASagaCharacterPlayer* character) noexcept
	{
		remoteCharacter = character;
	}

	constexpr void SetTeam(const EUserTeam& team) noexcept
	{
		myTeam = team;
	}

	constexpr void SetTeam(EUserTeam&& team) noexcept
	{
		myTeam = static_cast<EUserTeam&&>(team);
	}

	constexpr void SetWeapon(const EPlayerWeapon& weapon) noexcept
	{
		myWeapon = weapon;
	}

	constexpr void SetWeapon(EPlayerWeapon&& weapon) noexcept
	{
		myWeapon = static_cast<EPlayerWeapon&&>(weapon);
	}

	constexpr void SetHealth(const float& hp) noexcept
	{
		myHealth = hp;
	}

	constexpr void SetHealth(float&& hp) noexcept
	{
		myHealth = static_cast<float&&>(hp);
	}

	[[nodiscard]]
	class ASagaCharacterPlayer* GetCharacterHandle() const noexcept
	{
		return remoteCharacter;
	}

	[[nodiscard]]
	constexpr EUserTeam GetTeam() const noexcept
	{
		return myTeam;
	}

	[[nodiscard]]
	constexpr EPlayerWeapon GetWeapon() const noexcept
	{
		return myWeapon;
	}

	[[nodiscard]]
	constexpr float GetHealth() const noexcept
	{
		return myHealth;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Network|Session")
	TObjectPtr<class ASagaCharacterPlayer> remoteCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "CandyLandSaga|Network|Session")
	EUserTeam myTeam;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "CandyLandSaga|Network|Session")
	EPlayerWeapon myWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Network|Session")
	float myHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Network|Session")
	double myX;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Network|Session")
	double myY;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Network|Session")
	double myZ;
};
