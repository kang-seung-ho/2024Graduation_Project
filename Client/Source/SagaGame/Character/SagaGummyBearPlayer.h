#pragma once
#include <CoreMinimal.h>

#include "Character/SagaCharacterPlayer.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "SagaGummyBearPlayer.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaGummyBearPlayer : public ASagaCharacterPlayer
{
	GENERATED_BODY()

public:
	ASagaGummyBearPlayer();

	virtual void Attack() override;

	virtual float ExecuteHurt(const float dmg) override;
	virtual void ExecuteDeath() override;

	UFUNCTION()
	void TryDismemberment(FVector Hitlocation, FVector HitNormal);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool isCanRide = false;
	UFUNCTION(BlueprintCallable)
	FTransform SpawnMorphSystem(UGeometryCollectionComponent* TargetGC, int32 Index);

	UPROPERTY(EditAnywhere)
	int32 BearNum = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DismThreshold;
	TArray<UStaticMesh*> TargetMeshes;

	UFUNCTION(BlueprintCallable)
	UStaticMesh* GetTargetMesh(int32 Index);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* NiagaraSystemTemplate;



protected:
	// 오버랩 박스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* InteractionBox;

	UBoxComponent* Dbox_Rarm;
	UBoxComponent* Dbox_Larm;
	UBoxComponent* Dbox_Rleg;
	UBoxComponent* Dbox_Lleg;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TArray<UBoxComponent*> DismCollisionBox;

	UGeometryCollectionComponent* r_arm;
	UGeometryCollectionComponent* l_arm;
	UGeometryCollectionComponent* r_leg;
	UGeometryCollectionComponent* l_leg;

	TArray<UGeometryCollectionComponent*> GeometryCollections;

	int32 DismPartID;

	TArray<int32> ActiveIndx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* NiagaraComponentTemplate;

	template<typename T>
	void InitializeTransform(T component, FVector Location, FRotator Rotation, FVector Scale);

	void OnTakeDamage(FVector Location, FVector Normal);

	UGeometryCollectionComponent* GetGeometryCollectionByName(const FString& CollectionName);

	FVector GetPieceWorldPosition(UGeometryCollectionComponent* TargetGC, int32 PieceIndex);

	FQuat GetPieceWorldRotation(UGeometryCollectionComponent* TargetGC, int32 PieceIndex);

	bool IsPointInsideBox(UBoxComponent* Box, const FVector& Point);

	void CheckValidBone(const FVector& Impulse, int32 Index);

	void ExplodeBodyParts(FName BoneName, const FVector& Impulse, int32 Index);

	void HideTargetPiece(UGeometryCollectionComponent* TargetGC, int32 PieceIndex);

	void InitTargetMeshes();


	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void BeginPlay() override;

};
