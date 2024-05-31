#include "Saga/Interface/SagaMainMenuUiWidget.h"
#include <Internationalization/Text.h>
#include <Async/Future.h>
#include <Async/TaskGraphInterfaces.h>
#include <Templates/UnrealTemplate.h>
#include <Components/Button.h>
#include <Blueprint/WidgetTree.h>

#include "Saga/Interface/SagaMainMenuSettingsUiWidget.h"

#include "Saga/Network/SagaNetworkSubSystem.h"

USagaMainMenuUiWidget::USagaMainMenuUiWidget(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, myInputBox(), mySigninButton(), mySettingButton()
	, myConnectionPopup()
	, transientNickname()
	, isConnecting(false)
	, isSettingOpened()
	, settingUiClass(), settingUiInstance()
{
	static ConstructorHelpers::FClassFinder<USagaMainMenuSettingsUiWidget> setting_class(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Level/MainMenuLevel/SagaUiMainMenuSettingsWidget.SagaUiMainMenuSettingsWidget_C'"));

	if (setting_class.Succeeded())
	{
		settingUiClass = setting_class.Class;
	}
	else
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[ASagaMainMenuLevel][Ctor] Could not find the class of setting ui for main menu level."));
	}
}

void
USagaMainMenuUiWidget::OnConnected()
{
	UE_LOG(LogSagaFramework, Log, TEXT("[USagaMainMenuUiWidget] Client is connected to the server."));
}

void
USagaMainMenuUiWidget::OnFailedToConnect(ESagaConnectionContract reason)
{
	myInputBox->SetIsEnabled(true);
	mySigninButton->SetUserInteractable(true);

	isConnecting = false;
}

void
USagaMainMenuUiWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
			if (element == this) return;

			if (auto box = Cast<USagaUiEditableTextBox>(element))
			{
				myInputBox = box;
			}
			else if (auto sagabtn = Cast<USagaUiButton>(element))
			{
				const auto btn_name = sagabtn->GetName();

				if (btn_name == TEXT("LoginButton"))
				{
					mySigninButton = sagabtn;
				}
			}
			else if (auto btn = Cast<UButton>(element))
			{
				const auto btn_name = btn->GetName();

				if (btn_name == TEXT("SettingButton"))
				{
					mySettingButton = btn;
				}
			}
		}
	);

	const auto my_name = GetName();
	if (nullptr == myInputBox)
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaMainMenuUiWidget] '%s' found no text box in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaMainMenuUiWidget] '%s' found text box in children."), *my_name);
	}

	if (nullptr == mySigninButton)
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaMainMenuUiWidget] '%s' found no button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaMainMenuUiWidget] '%s' found button in children."), *my_name);
	}

	if (nullptr == mySettingButton)
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaMainMenuUiWidget] '%s' could not find the setting button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaMainMenuUiWidget] '%s' found the setting button in children."), *my_name);
	}

	myInputBox->OnTextChanged.AddDynamic(this, &USagaMainMenuUiWidget::HandleInputTextChanged);
	myInputBox->OnTextCommitted.AddDynamic(this, &USagaMainMenuUiWidget::HandleInputCommitted);

	mySigninButton->OnClicked.AddDynamic(this, &USagaMainMenuUiWidget::HandleButtonPressed);
	mySigninButton->SetUserInteractable(false);

	mySettingButton->OnClicked.AddDynamic(this, &USagaMainMenuUiWidget::HandleOpenSetting);

	const auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	system->OnConnected.AddDynamic(this, &USagaMainMenuUiWidget::OnConnected);
	system->OnFailedToConnect.AddDynamic(this, &USagaMainMenuUiWidget::OnFailedToConnect);
}

void
USagaMainMenuUiWidget::HandleSigningIn()
{
	if (transientNickname.IsEmpty())
	{
		UE_LOG(LogSagaFramework, Warning, TEXT("[USagaMainMenuUiWidget] Nickname is empty."));
		return;
	}

	if (isConnecting)
	{
		UE_LOG(LogSagaFramework, Warning, TEXT("[USagaMainMenuUiWidget] Client is already connecting."));
		return;
	}

	isConnecting = true;
	myInputBox->SetIsEnabled(false);
	mySigninButton->SetUserInteractable(false);

	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask,
		[this
		, system = USagaNetworkSubSystem::GetSubSystem(GetWorld())]()
		{
			const auto nickname_str = transientNickname.ToString();
			UE_LOG(LogSagaFramework, Warning, TEXT("[USagaMainMenuUiWidget] Nickname: %s"), *nickname_str);

			(void)system->ConnectToServer(transientNickname);
		}
	);
}

void
USagaMainMenuUiWidget::HandleButtonPressed()
{
	if (transientNickname.IsEmpty())
	{
		isConnecting = false;
	}
	else
	{
		HandleSigningIn();
	}
}

void
USagaMainMenuUiWidget::HandleOpenSetting()
{
	if (not isConnecting and not isSettingOpened)
	{
		settingUiInstance = CreateWidget<USagaMainMenuSettingsUiWidget>(GetWorld(), settingUiClass);

		if (IsValid(settingUiInstance))
		{
			settingUiInstance->AddToViewport(2);
			settingUiInstance->OpenNow();

			settingUiInstance->OnClosed.AddDynamic(this, &USagaMainMenuUiWidget::HandleCloseSetting);

			isSettingOpened = true;
		}
		else
		{
			UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaMainMenuUiWidget][HandleOpenSetting] Could not create setting ui for main menu."));
		}
	}
}

void
USagaMainMenuUiWidget::HandleCloseSetting()
{
	if (IsValid(settingUiInstance))
	{
		settingUiInstance->RemoveFromParent();
		settingUiInstance = nullptr;
	}

	isSettingOpened = false;
}

void
USagaMainMenuUiWidget::HandleInputTextChanged(const FText& nickname)
{
	transientNickname = FText::TrimPrecedingAndTrailing(nickname);

	auto str = transientNickname.ToString();
	if (16 < str.Len())
	{
		str = str.Left(16);

		transientNickname = FText::FromString(str);
	}

	if (not isConnecting)
	{
		mySigninButton->SetUserInteractable(not transientNickname.IsEmpty());
	}

	myInputBox->SetText(transientNickname);
}

void
USagaMainMenuUiWidget::HandleInputCommitted(const FText& nickname, ETextCommit::Type method)
{
	switch (method)
	{
	case ETextCommit::OnEnter:
	{
		transientNickname = FText::TrimPrecedingAndTrailing(nickname);
		myInputBox->SetText(transientNickname);

		HandleSigningIn();
	}
	break;

	default:
		break;
	}
}
