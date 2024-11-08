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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Bear", meta = (ClampMin = "0", ClampMax = "10"))
	int32 bearUniqueId;

	ASagaGummyBearPlayer();

	/* 데미지 처리

	* 온라인 / 오프라인 모드에 따라 분기
	*  온라인 모드	: RPC만 보냄. 나중에 RPC 돌려받았을 때 ASagaCharacterBase의 TakeDamage를 실행.
	*  오프라인 모드	: ASagaCharacterBase의 TakeDamage를 그대로 실행.
	*/
	virtual float TakeDamage(float dmg, struct FDamageEvent const& event, AController* instigator, AActor* causer) override;

	/* 반드시 ASagaPlayableCharacter의 ExecuteGuardianAction보다 나중에 실행해야 함. */
	// UFUNCTION()
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
	class UGeometryCollectionComponent* ExecutePartDestruction(const int32 index);
	UFUNCTION()
	void ExecuteMorphingPart(class UGeometryCollectionComponent* const& gc, const int32 index);
	UFUNCTION()
	void ExecuteMorphingPartAt(const int32 index);

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character|Bear")
	void RegisterMiniBear(const int32 index, ASagaGummyBearSmall* const minibear);
	UFUNCTION()
	int32 OnBodyPartGetDamaged(FVector Location, FVector Normal);
	UFUNCTION()
	bool GiveDamageToPart(const int32 index, const int32 part_dmg = 1);
	UFUNCTION()
	bool IsPartDestructed(const int32 index) const;

	UFUNCTION()
	ASagaCharacterBase* GetStoredCharacterHandle() const noexcept;

	UFUNCTION()
	int32 GetBearId() const noexcept;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Bear")
	TObjectPtr<class UArrowComponent> playerUnridePosition;

	/* 오버랩 박스 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Bear")
	class UBoxComponent* myInteractionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|Character|Bear")
	TArray<TObjectPtr<class ASagaGummyBearSmall>> partedSmallBears;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character|Bear")
	TArray<class UStaticMesh*> TargetMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character|Bear")
	TArray<class UBoxComponent*> DismCollisionBox;
	UPROPERTY()
	class UBoxComponent* Dbox_Rarm;
	UPROPERTY()
	class UBoxComponent* Dbox_Larm;
	UPROPERTY()
	class UBoxComponent* Dbox_Rleg;
	UPROPERTY()
	class UBoxComponent* Dbox_Lleg;

	UPROPERTY()
	TArray<class UGeometryCollectionComponent*> GeometryCollections;
	UPROPERTY()
	class UGeometryCollectionComponent* r_arm;
	UPROPERTY()
	class UGeometryCollectionComponent* l_arm;
	UPROPERTY()
	class UGeometryCollectionComponent* r_leg;
	UPROPERTY()
	class UGeometryCollectionComponent* l_leg;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character|Bear")
	int32 DismThreshold;
	UPROPERTY()
	int32 DismPartID;
	UPROPERTY()
	TArray<int32> ActiveIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character|Bear")
	class UNiagaraSystem* NiagaraSystemTemplate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|Character|Bear")
	class UNiagaraComponent* NiagaraComponentTemplate;

	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|Character|Bear")
	class UNiagaraSystem* HitEffect;

	void InitTargetMeshes();

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character|Bear")
	FTransform SpawnMorphSystem(class UGeometryCollectionComponent* TargetGC, int32 Index);

	UFUNCTION(BlueprintCallable, Category = "CandyLandSaga|Game|Character|Bear")
	class UStaticMesh* GetTargetMesh(int32 Index);

	virtual void BeginPlay() override;

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
