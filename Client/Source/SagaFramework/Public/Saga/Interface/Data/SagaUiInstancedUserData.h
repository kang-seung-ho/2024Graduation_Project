#pragma once
#include "Saga/Interface/Data/SagaUiDataObject.h"

#include "Saga/Network/SagaVirtualUser.h"
#include "SagaUiInstancedUserData.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Instanced User Data C++ Class"))
class SAGAFRAMEWORK_API USagaUiInstancedUserData : public USagaUiDataObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintSetter = SetUserData, BlueprintGetter = GetUserData, Category = "CandyLandSaga|UI|User Instance", meta = (ExposeOnSpawn = "true"))
	FSagaVirtualUser virtualMember;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintGetter = IsCursorHovered, Category = "CandyLandSaga|UI|User Instance")
	bool isCursorHovered;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintGetter = IsSelected, Category = "CandyLandSaga|UI|User Instance", meta = (ExposeOnSpawn = "true"))
	bool isSelected;

	USagaUiInstancedUserData() noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|User Instance", meta = (DisplayName = "Set User Data (Saga Instanced User)"))
	void SetUserData(const FSagaVirtualUser& virtual_user) noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|User Instance", meta = (DisplayName = "Get User Data (Saga Instanced User)"))
	FSagaVirtualUser GetUserData() const noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|User Instance", meta = (DisplayName = "Is Cursor Hovered (Saga Instanced User)"))
	bool IsCursorHovered() const noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|User Instance", meta = (DisplayName = "Is Selected (Saga Instanced User)"))
	bool IsSelected() const noexcept;
};
