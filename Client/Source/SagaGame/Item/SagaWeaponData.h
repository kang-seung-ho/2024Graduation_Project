#pragma once
#include "SagaGame.h"
#include <Engine/DataAsset.h>

#include "SagaWeaponData.generated.h"

UCLASS(BlueprintType, Category = "CandyLandSaga|Game|Item")
class SAGAGAME_API USagaWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type")
	ESagaItemTypes ItemType;

	UPROPERTY(EditAnywhere, Category = "ItemIcon")
	TObjectPtr<UTexture2D> ItemIcon;

public:
	UFUNCTION()
	void SetItemType(ESagaItemTypes Type) { ItemType = Type; }

	UFUNCTION()
	void SetWeaponMesh(UTexture2D* Mesh) { ItemIcon = Mesh; }
};
