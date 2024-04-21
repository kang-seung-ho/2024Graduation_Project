// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "EngineGlobals.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "SagaSpawnPoint.generated.h"

UENUM(BlueprintType)
enum class ESpawnType : uint8
{
	Normal, //생성된 객체가 제거되면 지정된 시간이 지나면 생성 ->캐릭터에 리스폰에 사용
	Once,	//한번생성 후 스폰포인트 제거 ->AI에 사용
	Loop   //지정된 횟수만큼 생성
};

UCLASS()
class SAGAGAME_API ASagaSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASagaSpawnPoint();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> mRoot;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UArrowComponent> mArrowComponent;
#endif


	//생성하려는 클래스 정보 저장
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> mSpawnClass;

	//위의 클래스 이용해 생성된 실제 객체의 메모리주소 저장
	TObjectPtr<AActor> mSpawnActor;

	UPROPERTY(EditAnywhere)
	ESpawnType mSpawnType;


	//스폰시간 0이면 즉시 스폰
	UPROPERTY(EditAnywhere)
	float mSpawnTime = 0.f;

	float mSpawnAccTime;

	//카운트 기본값은 -1로 설정해줌(무한생성)
	UPROPERTY(EditAnywhere)
	int32 mCount = -1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void Spawn();
};
