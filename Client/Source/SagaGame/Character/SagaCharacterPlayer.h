#pragma once
#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Components/BoxComponent.h"
#include "SagaCharacterNPC.h"
#include "Character/SagaCharacterBase.h"
#include "Interface/SagaMoveableCharacter.h"
#include "SagaCharacterPlayer.generated.h"

UCLASS()
class SAGAGAME_API ASagaCharacterPlayer : public ASagaCharacterBase, public ISagaMoveableCharacter
{
	GENERATED_BODY()
	
public:
	ASagaCharacterPlayer();
	//void InteractWithNPC();

	UPROPERTY()
	uint8 bIsNPCAround = 0;

public:
	UPROPERTY()
	uint8 bIsRiding = false;

protected:
	virtual void BeginPlay() override;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, Category = InteractionBox)
	UBoxComponent* InteractionBox;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health)
	float PlayerHP;

	UFUNCTION(BlueprintCallable, Category = Item)
	void UseItem(class UItems* Item);

	UFUNCTION(BlueprintCallable, Category = NPC)
	void GetOnNPC();
	

	//캐릭터 컨트롤 부분
protected:
	void ChangeCharacterControl();
	void SetCharacterControl(ECharacterControlType NewCharacterControlType);
	virtual void SetCharacterControlData(const class USagaCharacterControlData* CharacterControlData) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

	//인벤토리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	class UInventoryComponent* Inventory;

//input
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ChangeControlAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> QuaterMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InteractionWithNPC;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = NPC)
	ASagaCharacterNPC* NearbyNPC;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SprintReleaseAction;*/

	virtual void StartShoulderMove(const FInputActionValue& Value) override;
	void EndShoulderMove(const FInputActionValue& Value);

	void StartQuaterMove(const FInputActionValue& Value);
	void EndQuaterMove(const FInputActionValue& Value);

	void ExecuteMove();
	void TerminateMove();

	virtual void ShoulderLook(const FInputActionValue& Value) override;
	

	ECharacterControlType CurrentCharacterControlType;

	virtual void StartAttack() override;
	void EndAttack();

	void ExecuteAttack();
	void TerminateAttack();

	void StartSprinting();
	void StopSprinting();

	void ExecuteSprinting();
	void TerminateSprinting();
};
