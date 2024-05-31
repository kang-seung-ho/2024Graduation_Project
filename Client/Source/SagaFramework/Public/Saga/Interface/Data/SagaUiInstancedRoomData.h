#pragma once
#include "Saga/Interface/Data/SagaUiDataObject.h"

#include "Saga/Network/SagaVirtualRoom.h"
#include "SagaUiInstancedRoomData.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|UI", meta = (DisplayName = "Saga Instanced Room Data C++ Class"))
class SAGAFRAMEWORK_API USagaUiInstancedRoomData final : public USagaUiDataObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintSetter = SetRoomData, BlueprintGetter = GetRoomData, Category = "CandyLandSaga|UI|Room Instance", meta = (ExposeOnSpawn = "true"))
	FSagaVirtualRoom virtualRoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintGetter = IsCursorHovered, Category = "CandyLandSaga|UI|Room Instance")
	bool isCursorHovered;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintGetter = IsSelected, Category = "CandyLandSaga|UI|Room Instance", meta = (ExposeOnSpawn = "true"))
	bool isSelected;

	USagaUiInstancedRoomData() noexcept;

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Room Instance", meta = (DisplayName = "Set User Data (Saga Instanced Room)"))
	void SetRoomData(const FSagaVirtualRoom& virtual_room) noexcept;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Room Instance", meta = (DisplayName = "Get User Data (Saga Instanced Room)"))
	FSagaVirtualRoom GetRoomData() const noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Room Instance", meta = (DisplayName = "Is Cursor Hovered (Saga Instanced Room)"))
	bool IsCursorHovered() const noexcept;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Room Instance", meta = (DisplayName = "Is Selected (Saga Instanced Room)"))
	bool IsSelected() const noexcept;
};
