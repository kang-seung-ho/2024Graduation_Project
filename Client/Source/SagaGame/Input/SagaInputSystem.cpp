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

	{
		ConstructorHelpers::FObjectFinder<UInputAction>Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Jump.IA_Jump'"));

		if (Asset.Succeeded())
			Jump = Asset.Object;
	}

	{
		ConstructorHelpers::FObjectFinder<UInputAction>Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Rotation.IA_Rotation'"));

		if (Asset.Succeeded())
			Rotate = Asset.Object;
	}

	{
		ConstructorHelpers::FObjectFinder<UInputAction>Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Sprint.IA_Sprint'"));

		if (Asset.Succeeded())
			Sprint = Asset.Object;
	}

	{
		ConstructorHelpers::FObjectFinder<UInputAction>Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Interact.IA_Interact'"));

		if (Asset.Succeeded())
			Interact = Asset.Object;
	}

	{
		ConstructorHelpers::FObjectFinder<UInputAction>Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Activate.IA_Activate'"));

		if (Asset.Succeeded())
			Activate = Asset.Object;
	}

	{
		ConstructorHelpers::FObjectFinder<UInputAction>Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Inventory.IA_Inventory'"));

		if (Asset.Succeeded())
			Inventory = Asset.Object;
	}

	{
		ConstructorHelpers::FObjectFinder<UInputAction>Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Skill1.IA_Skill1'"));

		if (Asset.Succeeded())
			Skill1 = Asset.Object;
	}

	{
		ConstructorHelpers::FObjectFinder<UInputAction>Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Item1.IA_Item1'"));

		if (Asset.Succeeded())
			Item1 = Asset.Object;
	}

	{
		ConstructorHelpers::FObjectFinder<UInputAction>Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Item2.IA_Item2'"));

		if (Asset.Succeeded())
			Item2 = Asset.Object;
	}

	{
		ConstructorHelpers::FObjectFinder<UInputAction>Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Item3.IA_Item3'"));

		if (Asset.Succeeded())
			Item3 = Asset.Object;
	}
}

USagaCharacterSelectInputSystem::USagaCharacterSelectInputSystem()
{
	{
		ConstructorHelpers::FObjectFinder<UInputMappingContext>Asset(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input/CharacterSelect/IMC_CharacterSelect.IMC_CharacterSelect'"));

		if (Asset.Succeeded())
			DefaultContext = Asset.Object;
	}

	{
		ConstructorHelpers::FObjectFinder<UInputAction>Asset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/CharacterSelect/IA_CharacterSelect.IA_CharacterSelect'"));

		if (Asset.Succeeded())
			Click = Asset.Object;
	}
}
