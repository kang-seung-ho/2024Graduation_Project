#pragma once
#include "SagaGame.h"

#include "Character/SagaCharacterBase.h"
#include "SagaGummyBearPlayer.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaGummyBearPlayer : public ASagaCharacterBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character|Bear")
	int32 DismThreshold;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character|Bear")
	TArray<class UStaticMesh*> TargetMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character|Bear")
	bool isCanRide;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character|Bear")
	class UNiagaraSystem * NiagaraSystemTemplate;

	ASagaGummyBearPlayer();

	// UFUNCTION()
	virtual void ExecuteGuardianAction(ASagaCharacterBase* target) override;
	// UFUNCTION()
	virtual void ExecuteAttackAnimation() override;
	// UFUNCTION()
	virtual void ExecuteAttack() override;
	// UFUNCTION()
	virtual float ExecuteHurt(const float dmg) override;
	// UFUNCTION()
	virtual void ExecuteDeath() override;

	UFUNCTION()
	void TryDismemberment(FVector Hitlocation, FVector HitNormal);

	UFUNCTION(BlueprintCallable)
	FTransform SpawnMorphSystem(class UGeometryCollectionComponent* TargetGC, int32 Index);

	UFUNCTION(BlueprintCallable)
	class UStaticMesh* GetTargetMesh(int32 Index);
	
	UFUNCTION()
	int32 GetBearId(int32 Index) const noexcept;

protected:
	/* 곰의 고유 번호

	*  서버, 클라 모두 같게 동기화됨
	*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character|Bear")
	int32 bearUniqueId;

	/* 오버랩 박스 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Bear")
	class UBoxComponent* myInteractionBox;

	int32 DismPartID;
	class UBoxComponent* Dbox_Rarm;
	class UBoxComponent* Dbox_Larm;
	class UBoxComponent* Dbox_Rleg;
	class UBoxComponent* Dbox_Lleg;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Bear")
	TArray<class UBoxComponent*> DismCollisionBox;

	TArray<int32> ActiveIndx;
	class UGeometryCollectionComponent* r_arm;
	class UGeometryCollectionComponent* l_arm;
	class UGeometryCollectionComponent* r_leg;
	class UGeometryCollectionComponent* l_leg;
	TArray<class UGeometryCollectionComponent*> GeometryCollections;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character|Bear")
	class UNiagaraComponent* NiagaraComponentTemplate;

	void InitTargetMeshes();
	void ExplodeBodyParts(FName BoneName, const FVector& Impulse, int32 Index);
	void HideTargetPiece(class UGeometryCollectionComponent* TargetGC, int32 PieceIndex);
	void CheckValidBone(const FVector& Impulse, int32 Index);

	class UGeometryCollectionComponent* GetGeometryCollectionByName(const FString& CollectionName);
	FVector GetPieceWorldPosition(class UGeometryCollectionComponent* TargetGC, int32 PieceIndex);
	FQuat GetPieceWorldRotation(class UGeometryCollectionComponent* TargetGC, int32 PieceIndex);

	void OnBodyPartGetDamaged(FVector Location, FVector Normal);

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	static void InitializeTransform(class USceneComponent* component, FVector Location, FRotator Rotation, FVector Scale);
	static bool IsPointInsideBox(class UBoxComponent* Box, const FVector& Point);
};
