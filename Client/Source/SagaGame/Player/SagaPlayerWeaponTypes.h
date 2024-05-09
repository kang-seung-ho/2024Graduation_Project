#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SagaPlayerWeaponTypes.generated.h"

UCLASS()
class SAGAGAME_API ASagaPlayerWeaponTypes : public ACharacter
{
	GENERATED_BODY()

public:
	// 이 empty의 속성에 대한 기본값을 설정합니다.
	ASagaPlayerWeaponTypes();

protected:
	// 게임이 시작되거나 생성될 때 호출됩니다.
	virtual void BeginPlay() override;

public:
	// 매 프레임마다 호출됨
	virtual void Tick(float DeltaTime) override;

	// 기능을 입력에 바인딩하기 위해 호출됨
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
