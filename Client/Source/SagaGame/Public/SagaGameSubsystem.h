#pragma once
#include "SagaGame.h"
#include <Subsystems/GameInstanceSubsystem.h>

#include "SagaGameSubsystem.generated.h"

UCLASS(Category = "CandyLandSaga|Game")
class SAGAGAME_API USagaGameSubsystem final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USagaGameSubsystem();

	/* State Machines */
#pragma region =========================
	virtual void Initialize(class FSubsystemCollectionBase& collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(class UObject* outer) const override;
#pragma endregion

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game", meta = (UnsafeDuringActorConstruction))
	static USagaGameSubsystem* GetSubSystem(const class UWorld* world);

	UFUNCTION()
	void AssignLocalPlayerSpawner(class AActor* spawner) noexcept;

	UFUNCTION()
	const AActor* GetLocalPlayerSpawner() const noexcept;

private:
	UPROPERTY()
	TObjectPtr<class AActor> localPlayerSpawner;
};
