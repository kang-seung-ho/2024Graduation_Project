#pragma once
#include <CoreMinimal.h>
#include "Engine/DataAsset.h"

#include "SagaWeaponData.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Drink,
	Gum,
	SmokeBomb
};

UCLASS()
class SAGAGAME_API USagaWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "ItemIcon")
	TObjectPtr<UTexture2D> ItemIcon;

public:
	UFUNCTION()
	void SetItemType(EItemType Type) { ItemType = Type; }

	UFUNCTION()
	void SetWeaponMesh(UTexture2D* Mesh) { ItemIcon = Mesh; }
};
