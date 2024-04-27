// Fill out your copyright notice in the Description page of Project Settings.


#include "SagaInputSystem.h"

USagaInputSystem::USagaInputSystem()
{
	{
		ConstructorHelpers::FObjectFinder<UInputMappingContext>Asset(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input/IMC_Default.IMC_Default'"));

		if (Asset.Succeeded())
			DefaultContext = Asset.Object;
	}

	{
		ConstructorHelpers::FObjectFinder<UInputAction>Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Move.IA_Move'"));

		if (Asset.Succeeded())
			ForwardBackMove = Asset.Object;
	}

	{
		ConstructorHelpers::FObjectFinder<UInputAction>Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Strafe.IA_Strafe'"));

		if (Asset.Succeeded())
			StrafeMove = Asset.Object;
	}
	
	{
		ConstructorHelpers::FObjectFinder<UInputAction>Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Attack.IA_Attack'"));

		if (Asset.Succeeded())
			Attack = Asset.Object;
	}

}
