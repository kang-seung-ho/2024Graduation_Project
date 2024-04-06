#pragma once
#include "CoreMinimal.h"
#include "Character/SagaCharacterBase.h"
#include "Interface/InteractionInterface.h"

#include "Interface/SagaMoveableCharacter.h"
#include "SagaCharacterNPC.generated.h"

UCLASS(BlueprintType, Blueprintable, Category = "CandyLandSaga|Game|NPC")
class SAGAGAME_API ASagaCharacterNPC : public ASagaCharacterBase, public IInteractionInterface
{
	GENERATED_BODY()

public:
	ASagaCharacterNPC();

	UFUNCTION(Category = "CandyLandSaga|Game|NPC")
	void ShowInteractionPrompt(bool bShow);

	UFUNCTION(Category = "CandyLandSaga|Game|NPC")
	void OnPlayerEnterRange();

	UFUNCTION(Category = "CandyLandSaga|Game|NPC")
	void OnPlayerExitRange();

	virtual void InteractWithMe() override;

	void DetachPlayer(APlayerController* PlayerController, APawn* OriginalPlayerPawn);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CandyLandSaga|Game|NPC")
	TSubclassOf<UUserWidget> NPCInteractionWidget;

	UPROPERTY(VisibleAnywhere, Category = "CandyLandSaga|Game|NPC")
	UUserWidget* NPCInteractionTEXTWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CandyLandSaga|Game|NPC")
	UCapsuleComponent* NPCInteractionCapsule;

protected:
	virtual void BeginPlay() override;

	void SetDead() override;

	UFUNCTION(Category = "CandyLandSaga|Game|NPC")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Category = "CandyLandSaga|Game|NPC")
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
