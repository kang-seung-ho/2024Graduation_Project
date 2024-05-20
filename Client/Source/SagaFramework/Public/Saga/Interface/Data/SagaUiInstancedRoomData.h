#pragma once
#include "Saga/Interface/Data/SagaUiDataObject.h"
#include <Delegates/DelegateCombinations.h>

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

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|UI|Room Instance")
	void SetRoomData(const FSagaVirtualRoom& virtual_room) noexcept;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Room Instance")
	FSagaVirtualRoom GetRoomData() const noexcept;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Room Instance")
	bool IsCursorHovered() const noexcept;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CandyLandSaga|UI|Room Instance")
	bool IsSelected() const noexcept;
};
