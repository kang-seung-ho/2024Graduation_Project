#pragma once
#include "SagaGame.h"

#include "Character/SagaCharacterBase.h"
#include "SagaGummyBearPlayer.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaGummyBearPlayer : public ASagaCharacterBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int32 BearNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DismThreshold;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UStaticMesh*> TargetMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool isCanRide;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNiagaraSystem * NiagaraSystemTemplate;

	ASagaGummyBearPlayer();

	virtual void Attack() override;

	virtual float ExecuteHurt(const float dmg) override;
	virtual void ExecuteDeath() override;

	UFUNCTION()
	void TryDismemberment(FVector Hitlocation, FVector HitNormal);

	UFUNCTION(BlueprintCallable)
	FTransform SpawnMorphSystem(class UGeometryCollectionComponent* TargetGC, int32 Index);

	UFUNCTION(BlueprintCallable)
	class UStaticMesh* GetTargetMesh(int32 Index);

protected:
	virtual void BeginPlay() override;

	virtual void PlayAttackAnimation() override;

	// 오버랩 박스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* InteractionBox;

	class UBoxComponent* Dbox_Rarm;
	class UBoxComponent* Dbox_Larm;
	class UBoxComponent* Dbox_Rleg;
	class UBoxComponent* Dbox_Lleg;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TArray<class UBoxComponent*> DismCollisionBox;

	class UGeometryCollectionComponent* r_arm;
	class UGeometryCollectionComponent* l_arm;
	class UGeometryCollectionComponent* r_leg;
	class UGeometryCollectionComponent* l_leg;

	TArray<class UGeometryCollectionComponent*> GeometryCollections;

	int32 DismPartID;

	TArray<int32> ActiveIndx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNiagaraComponent* NiagaraComponentTemplate;

	template<typename T>
	void InitializeTransform(T component, FVector Location, FRotator Rotation, FVector Scale);

	void OnTakeDamage(FVector Location, FVector Normal);

	class UGeometryCollectionComponent* GetGeometryCollectionByName(const FString& CollectionName);

	FVector GetPieceWorldPosition(class UGeometryCollectionComponent* TargetGC, int32 PieceIndex);

	FQuat GetPieceWorldRotation(class UGeometryCollectionComponent* TargetGC, int32 PieceIndex);

	bool IsPointInsideBox(class UBoxComponent* Box, const FVector& Point);

	void CheckValidBone(const FVector& Impulse, int32 Index);

	void ExplodeBodyParts(FName BoneName, const FVector& Impulse, int32 Index);

	void HideTargetPiece(class UGeometryCollectionComponent* TargetGC, int32 PieceIndex);

	void InitTargetMeshes();

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
