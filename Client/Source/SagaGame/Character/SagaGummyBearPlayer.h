#pragma once
#include "SagaGame.h"
#include <Math/MathFwd.h>

#include "Character/SagaCharacterBase.h"
#include "SagaGummyBearPlayer.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|Character")
class SAGAGAME_API ASagaGummyBearPlayer : public ASagaCharacterBase
{
	GENERATED_BODY()

public:
	/* 곰의 고유 번호

	*  서버, 클라 모두 같게 동기화됨
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character|Bear", meta = (ClampMin = "0", ClampMax = "10"))
	int32 bearUniqueId;

	ASagaGummyBearPlayer();

	/* 데미지 처리

	* 온라인 / 오프라인 모드에 따라 분기
	*  온라인 모드	: RPC만 보냄. 나중에 RPC 돌려받았을 때 ASagaCharacterBase의 TakeDamage를 실행.
	*  오프라인 모드	: ASagaCharacterBase의 TakeDamage를 그대로 실행.
	*/
	virtual float TakeDamage(float dmg, struct FDamageEvent const& event, AController* instigator, AActor* causer) override;

	// UFUNCTION()
	/* 반드시 ASagaPlayableCharacter의 ExecuteGuardianAction보다 나중에 실행해야 함. */
	virtual void ExecuteGuardianAction(ASagaCharacterBase* target) override;
	// UFUNCTION()
	virtual void TerminateGuardianAction() override;
	// UFUNCTION()
	virtual void ExecuteAttackAnimation() override;
	// UFUNCTION()
	virtual void ExecuteAttack() override;
	// UFUNCTION()
	virtual float ExecuteHurt(const float dmg) override;
	// UFUNCTION()
	virtual void ExecuteDeath() override;

	UFUNCTION()
	float OnBodyPartGetDamaged(FVector Location, FVector Normal);

	UFUNCTION()
	int32 GetBearId() const noexcept;

protected:
	static inline constexpr int32 defaultChaosPartDestructiveHealthPoint = 2;
	static inline constexpr int32 chaosPartsNumber = 4;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Bear")
	TObjectPtr<class UArrowComponent> playerUnridePosition;

	/* 오버랩 박스 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Bear")
	class UBoxComponent* myInteractionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character|Bear")
	TArray<TObjectPtr<class UStaticMesh>> myChaosPartMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Bear")
	TArray<class UBoxComponent*> chaosParts;
	UPROPERTY()
	TObjectPtr<class UBoxComponent> chaosPartRightArm;
	UPROPERTY()
	TObjectPtr<class UBoxComponent> chaosPartLeftArm;
	UPROPERTY()
	TObjectPtr<class UBoxComponent> chaosPartRightLeg;
	UPROPERTY()
	TObjectPtr<class UBoxComponent> chaosPartLeftLeg;
	UPROPERTY()
	TArray<int32> chaosPartHealthPoints;
	UPROPERTY()
	int8 cachedDestructedPartData[4];

	UPROPERTY()
	TArray<class UGeometryCollectionComponent*> geometryCollections;
	UPROPERTY()
	TObjectPtr<class UGeometryCollectionComponent> geometryRightArm;
	UPROPERTY()
	TObjectPtr<class UGeometryCollectionComponent> geometryLeftArm;
	UPROPERTY()
	TObjectPtr<class UGeometryCollectionComponent> geometryRightLeg;
	UPROPERTY()
	TObjectPtr<class UGeometryCollectionComponent> geometryLeftLeg;

	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character|Bear")
	TObjectPtr<class UNiagaraSystem> NiagaraSystemTemplate;
	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character|Bear")
	TObjectPtr<class UNiagaraComponent> NiagaraComponentTemplate;
	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character|Bear")
	TObjectPtr<class UNiagaraSystem> HitEffect;

	virtual void BeginPlay() override;

	void InitTargetMeshes();
	void CheckValidBone(const FVector& Impulse, int32 Index);
	void ExplodeBodyParts(FName BoneName, const FVector& Impulse, int32 Index);

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character|Bear")
	FTransform SpawnMorphSystem(class UGeometryCollectionComponent* TargetGC, int32 Index);

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character|Bear")
	class UStaticMesh* GetTargetMesh(int32 Index) const;

	//UFUNCTION()
	//void OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//UFUNCTION()
	//void OnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	static void InitializeTransform(class USceneComponent* const component, const FVector& loc, const FRotator& rot, const FVector& scale);
	static bool IsPointInsideBox(const class UBoxComponent* const box, const FVector& point);
	static void HideBodyPartPiece(class UGeometryCollectionComponent* const target, int32 PieceIndex);

	static class UGeometryCollectionComponent* FindGeometryCollectionByName(const class AActor* const actor, const FString& name);
	static FVector GetPieceWorldLocation(const class UGeometryCollectionComponent* const target, int32 index);
	static FQuat GetPieceWorldRotation(const class UGeometryCollectionComponent* const target, int32 index);
};
