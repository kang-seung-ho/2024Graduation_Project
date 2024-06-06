#pragma once
#include "SagaGame.h"
#include "AI/SagaMonsterAIPawn.h"

#include "SagaGummyBearSmall.generated.h"

UCLASS()
class SAGAGAME_API ASagaGummyBearSmall : public ASagaMonsterAIPawn
{
	GENERATED_BODY()

public:
	ASagaGummyBearSmall();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;


};
