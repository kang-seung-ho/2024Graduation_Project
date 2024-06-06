#include "Saga/Interface/SagaMainMenuSettingsUiWidget.h"
#include <Internationalization/Text.h>
#include <Containers/UnrealString.h>
#include <Templates/Casts.h>
#include <Components/Widget.h>
#include <Components/ComboBoxKey.h>
#include <Components/CheckBox.h>
#include <Blueprint/WidgetTree.h>

#include "Saga/Interface/SagaUiButton.h"
#include "Saga/Interface/SagaUiEditableTextBox.h"

#include "Saga/Network/SagaNetworkSubSystem.h"
#include "Saga/Network/SagaNetworkSettings.h"

USagaMainMenuSettingsUiWidget::USagaMainMenuSettingsUiWidget(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, myResetButton(), myOkButton()
	, myCheckBox()
	, myComboBox()
	, myIpAddressInputBox()
	, myPortInputBox()
	, myOfflineModeResetButton()
	, myConnectOptionResetButton()
	, myIpAddressResetButton()
	, mPortResetButton()
	, storedIpAddress(), storedPortNumber()
{
	SetTransitionPeriod(ESagaLiveUserWidgetStates::Opening, 0.3f);
	SetTransitionPeriod(ESagaLiveUserWidgetStates::Closing, 0.7f);
}

void
USagaMainMenuSettingsUiWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
			if (auto checker = Cast<UCheckBox>(element))
			{
				// myCheckBox in editor
				if (GetOfflineModeChecker())
				{
					checker->SetCheckedState(ECheckBoxState::Checked);
				}
				else
				{
					checker->SetCheckedState(ECheckBoxState::Unchecked);
				}
			}
			else if (auto combo = Cast<UComboBoxKey>(element))
			{
				// myComboBox in editor
				const ESagaNetworkConnectionCategory category = AcquireConnectionOption();

				if (category == ESagaNetworkConnectionCategory::Local)
				{
					combo->SetSelectedOption(TEXT("Local"));
				}
				else if (category == ESagaNetworkConnectionCategory::Host)
				{
					combo->SetSelectedOption(TEXT("Host"));
				}
				else if (category == ESagaNetworkConnectionCategory::Remote)
				{
					combo->SetSelectedOption(TEXT("Remote"));
				}
			}
			else if (auto input = Cast<USagaUiEditableTextBox>(element))
			{
				const auto box_name = input->GetName();

				if (box_name == TEXT("IpInputBox"))
				{
					// myIpAddressInputBox in editor
					input->SetHintText(GetIpAddressHintText());
				}
				else if (box_name == TEXT("PortInputBox"))
				{
					// myPortInputBox in editor
					input->SetHintText(GetPortHintText());
				}
			}
		}
	);
}

void
USagaMainMenuSettingsUiWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WidgetTree->ForEachWidget(
		[this](UWidget* const element) -> void
		{
			if (auto checker = Cast<UCheckBox>(element))
			{
				myCheckBox = checker;
			}
			else if (auto combo = Cast<UComboBoxKey>(element))
			{
				myComboBox = combo;
			}
			else if (auto input = Cast<USagaUiEditableTextBox>(element))
			{
				const auto box_name = input->GetName();

				if (box_name == TEXT("IpInputBox"))
				{
					myIpAddressInputBox = input;
				}
				else if (box_name == TEXT("PortInputBox"))
				{
					myPortInputBox = input;
				}
			}
			else if (auto btn = Cast<USagaUiButton>(element))
			{
				const auto btn_name = btn->GetName();

				if (btn_name == TEXT("ResetButton"))
				{
					myResetButton = btn;
				}
				else if (btn_name == TEXT("CloseButton"))
				{
					myOkButton = btn;
				}
				else if (btn_name == TEXT("ResetOfflineModeButton"))
				{
					myOfflineModeResetButton = btn;
				}
				else if (btn_name == TEXT("ResetConnectionOptionButton"))
				{
					myConnectOptionResetButton = btn;
				}
				else if (btn_name == TEXT("ResetIpAddressButton"))
				{
					myIpAddressResetButton = btn;
				}
				else if (btn_name == TEXT("ResetPortButton"))
				{
					mPortResetButton = btn;
				}
			}
		}
	);

	const auto my_name = GetName();
	if (nullptr == myResetButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaMainMenuSettingsUiWidget] '%s' could not found the reset button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaMainMenuSettingsUiWidget] '%s' found the reset button in children."), *my_name);
	}

	if (nullptr == myOkButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaMainMenuSettingsUiWidget] '%s' could not found close button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaMainMenuSettingsUiWidget] '%s' found the close button in children."), *my_name);
	}

	if (nullptr == myCheckBox)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaMainMenuSettingsUiWidget] '%s' could not found the offline mode check box in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaMainMenuSettingsUiWidget] '%s' found the offline mode check box in children."), *my_name);
	}

	if (nullptr == myComboBox)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaMainMenuSettingsUiWidget] '%s' could not found the option combo box in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaMainMenuSettingsUiWidget] '%s' found the option combo box in children."), *my_name);

		const ESagaNetworkConnectionCategory category = AcquireConnectionOption();

		if (category == ESagaNetworkConnectionCategory::Local)
		{
			myComboBox->SetSelectedOption(TEXT("Local"));
		}
		else if (category == ESagaNetworkConnectionCategory::Host)
		{
			myComboBox->SetSelectedOption(TEXT("Host"));
		}
		else if (category == ESagaNetworkConnectionCategory::Remote)
		{
			myComboBox->SetSelectedOption(TEXT("Remote"));
		}
		else
		{
			UE_LOG(LogSagaFramework, Error, TEXT("[USagaMainMenuSettingsUiWidget] '%s' has failed to set proper option."), *my_name);
		}
	}

	if (nullptr == myIpAddressInputBox)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaMainMenuSettingsUiWidget] '%s' could not found the ip address input box in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaMainMenuSettingsUiWidget] '%s' found the the ip address input box in children."), *my_name);
	}

	if (nullptr == myPortInputBox)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaMainMenuSettingsUiWidget] '%s' could not found the port number input box in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaMainMenuSettingsUiWidget] '%s' found the the port number input box in children."), *my_name);
	}

	if (nullptr == myOfflineModeResetButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaMainMenuSettingsUiWidget] '%s' could not found the reset offline mode button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaMainMenuSettingsUiWidget] '%s' found the reset offline mode button in children."), *my_name);
	}

	if (nullptr == myConnectOptionResetButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaMainMenuSettingsUiWidget] '%s' could not found the reset connection option button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaMainMenuSettingsUiWidget] '%s' found the reset connection option button in children."), *my_name);
	}

	if (nullptr == myIpAddressResetButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaMainMenuSettingsUiWidget] '%s' could not found the reset ip address button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaMainMenuSettingsUiWidget] '%s' found the reset ip address button in children."), *my_name);
	}

	if (nullptr == mPortResetButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaMainMenuSettingsUiWidget] '%s' could not found the reset port button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaMainMenuSettingsUiWidget] '%s' found the reset port button in children."), *my_name);
	}

	myResetButton->OnClicked.AddDynamic(this, &USagaMainMenuSettingsUiWidget::HandleOnResetEverything);
	myOkButton->OnClicked.AddDynamic(this, &USagaMainMenuSettingsUiWidget::HandleClose);

	myCheckBox->OnCheckStateChanged.AddDynamic(this, &USagaMainMenuSettingsUiWidget::SetOfflineModeChecker);

	myComboBox->OnSelectionChanged.AddDynamic(this, &USagaMainMenuSettingsUiWidget::HandleConnectionOption);

	myIpAddressInputBox->OnTextChanged.AddDynamic(this, &USagaMainMenuSettingsUiWidget::HandleOnIpAddressModified);
	myIpAddressInputBox->OnTextCommitted.AddDynamic(this, &USagaMainMenuSettingsUiWidget::HandleOnIpAddressCommitted);
	myPortInputBox->OnTextChanged.AddDynamic(this, &USagaMainMenuSettingsUiWidget::HandleOnPortNumberModified);
	myPortInputBox->OnTextCommitted.AddDynamic(this, &USagaMainMenuSettingsUiWidget::HandleOnPortNumberCommitted);

	myOfflineModeResetButton->OnClicked.AddDynamic(this, &USagaMainMenuSettingsUiWidget::HandleOnResetOfflineMode);
	myConnectOptionResetButton->OnClicked.AddDynamic(this, &USagaMainMenuSettingsUiWidget::HandleOnResetConnectionOption);
	myIpAddressResetButton->OnClicked.AddDynamic(this, &USagaMainMenuSettingsUiWidget::HandleOnResetRemoteIpAddress);
	mPortResetButton->OnClicked.AddDynamic(this, &USagaMainMenuSettingsUiWidget::HandleOnResetRemotePortNumber);

	OnClosed.AddDynamic(this, &USagaMainMenuSettingsUiWidget::HandleOnClosed);
}

void
USagaMainMenuSettingsUiWidget::HandleConnectionOption(FName item, ESelectInfo::Type method)
{
	if (method == ESelectInfo::Direct)
	{
		return;
	}

	const auto name = item.ToString();

	if (name == TEXT("Local"))
	{
		const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		net->SetConnectionOption(ESagaNetworkConnectionCategory::Local);
	}
	else if (name == TEXT("Host"))
	{
		const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		net->SetConnectionOption(ESagaNetworkConnectionCategory::Host);
	}
	else if (name == TEXT("Remote"))
	{
		const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

		net->SetConnectionOption(ESagaNetworkConnectionCategory::Remote);
	}
	else
	{
		UE_LOG(LogSagaFramework, Warning, TEXT("[USagaMainMenuSettingsUiWidget] Unknown option '%s' detected."), *name);
	}
}

void
USagaMainMenuSettingsUiWidget::HandleOnIpAddressModified(const FText& text)
{
	storedIpAddress = text;
}

void
USagaMainMenuSettingsUiWidget::HandleOnIpAddressCommitted(const FText& text, ETextCommit::Type method)
{
	if (method == ETextCommit::OnEnter)
	{
		const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());
		net->SetRemoteAddress(text.ToString());
	}
}

void
USagaMainMenuSettingsUiWidget::HandleOnPortNumberModified(const FText& text)
{
	if (text.IsEmpty())
	{
		myPortInputBox->SetText(storedPortNumber);
		return;
	}

	if (not text.IsNumeric())
	{
		myPortInputBox->SetText(storedPortNumber);
		return;
	}

	const auto str = text.ToString();
	if (6 <= str.Len())
	{
		myPortInputBox->SetText(storedPortNumber);
		return;
	}

	const auto port = FCString::Atoi(*str);
	if (port <= 0 or 0xffff <= port)
	{
		myPortInputBox->SetText(storedPortNumber);
		return;
	}

	storedPortNumber = text;
}

void
USagaMainMenuSettingsUiWidget::HandleOnPortNumberCommitted(const FText& text, ETextCommit::Type method)
{
	if (method == ETextCommit::OnEnter)
	{
		const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());
		if (text.IsEmpty())
		{
			UE_LOG(LogSagaFramework, Warning, TEXT("[USagaMainMenuSettingsUiWidget] The port label is cleared."));
		}
		else if (not text.IsNumeric())
		{
			UE_LOG(LogSagaFramework, Error, TEXT("[USagaMainMenuSettingsUiWidget] The port label is not a number."));

			return;
		}
		else
		{
			const auto str = text.ToString();
			const auto port = FCString::Atoi(*str);

			net->SetRemotePort(port);
		}
	}
}

void
USagaMainMenuSettingsUiWidget::HandleOnResetEverything()
{
	storedIpAddress = FText::FromString(TEXT("127.0.0.1"));
	storedPortNumber = FText::FromString(TEXT("40000"));

	myIpAddressInputBox->SetText(FText::GetEmpty());
	myPortInputBox->SetText(FText::GetEmpty());

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	net->SetOfflineMode(false);
	net->SetConnectionOption(ESagaNetworkConnectionCategory::Remote);
	net->SetRemoteAddress(TEXT("127.0.0.1"));
	net->SetRemotePort(40000);
}

void
USagaMainMenuSettingsUiWidget::HandleOnResetOfflineMode()
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	net->SetOfflineMode(false);
}

void
USagaMainMenuSettingsUiWidget::HandleOnResetConnectionOption()
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	net->SetConnectionOption(ESagaNetworkConnectionCategory::Remote);
}

void
USagaMainMenuSettingsUiWidget::HandleOnResetRemoteIpAddress()
{
	storedIpAddress = FText::FromString(TEXT("127.0.0.1"));
	myIpAddressInputBox->SetText(FText::GetEmpty());

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	net->SetRemoteAddress(TEXT("127.0.0.1"));
}

void
USagaMainMenuSettingsUiWidget::HandleOnResetRemotePortNumber()
{
	storedPortNumber = FText::FromString(TEXT("40000"));
	myPortInputBox->SetText(FText::GetEmpty());

	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	net->SetRemotePort(40000);
}

void
USagaMainMenuSettingsUiWidget::HandleClose()
{
	Close();
}

void
USagaMainMenuSettingsUiWidget::HandleOnClosed()
{
	RemoveFromParent();
}

void
USagaMainMenuSettingsUiWidget::SetOfflineModeChecker(bool is_checked)
{
	const auto net = USagaNetworkSubSystem::GetSubSystem(GetWorld());

	UE_LOG(LogSagaFramework, Log, TEXT("[USagaMainMenuSettingsUiWidget] Offline Mode: %s"), is_checked ? TEXT("True") : TEXT("False"));
	net->SetOfflineMode(is_checked);
}

bool
USagaMainMenuSettingsUiWidget::GetOfflineModeChecker()
const noexcept
{
	return AcquireOfflineMode();
}

FText
USagaMainMenuSettingsUiWidget::GetIpAddressHintText()
const
{
	const auto ip = AcquireIpAddress();
	const auto fmt = FString::Printf(TEXT("IP 주소 입력 (%s)"), *ip);

	return FText::FromString(fmt);
}

FText
USagaMainMenuSettingsUiWidget::GetPortHintText()
const
{
	const auto port = AcquirePort();
	const auto fmt = FString::Printf(TEXT("포트 번호 입력 (%d)"), port);

	return FText::FromString(fmt);
}

bool
USagaMainMenuSettingsUiWidget::AcquireOfflineMode()
const
{
	const auto world = GetWorld();
	if (nullptr == world)
	{
		const auto settings = GetDefault<USagaNetworkSettings>();
		return settings->IsOfflineMode;
	}

	const auto singleton = world->GetGameInstance();
	if (nullptr == singleton)
	{
		const auto settings = GetDefault<USagaNetworkSettings>();
		return settings->IsOfflineMode;
	}

	const auto net = singleton->GetSubsystem<USagaNetworkSubSystem>();
	if (nullptr == net)
	{
		const auto settings = GetDefault<USagaNetworkSettings>();
		return settings->IsOfflineMode;
	}
	else
	{
		return net->IsOfflineMode();
	}
}

ESagaNetworkConnectionCategory
USagaMainMenuSettingsUiWidget::AcquireConnectionOption()
const
{
	const auto world = GetWorld();
	if (nullptr == world)
	{
		const auto settings = GetDefault<USagaNetworkSettings>();
		return settings->ConnectionCategory;
	}

	const auto singleton = world->GetGameInstance();
	if (nullptr == singleton)
	{
		const auto settings = GetDefault<USagaNetworkSettings>();
		return settings->ConnectionCategory;
	}

	const auto net = singleton->GetSubsystem<USagaNetworkSubSystem>();
	if (nullptr == net)
	{
		const auto settings = GetDefault<USagaNetworkSettings>();
		return settings->ConnectionCategory;
	}
	else
	{
		return net->GetConnectionOption();
	}
}

FString
USagaMainMenuSettingsUiWidget::AcquireIpAddress()
const
{
	const auto world = GetWorld();
	if (nullptr == world)
	{
		const auto settings = GetDefault<USagaNetworkSettings>();
		return settings->RemoteAddress;
	}

	const auto singleton = world->GetGameInstance();
	if (nullptr == singleton)
	{
		const auto settings = GetDefault<USagaNetworkSettings>();
		return settings->RemoteAddress;
	}

	const auto net = singleton->GetSubsystem<USagaNetworkSubSystem>();
	if (nullptr == net)
	{
		const auto settings = GetDefault<USagaNetworkSettings>();
		return settings->RemoteAddress;
	}
	else
	{
		return net->GetRemoteAddress();
	}
}

int32
USagaMainMenuSettingsUiWidget::AcquirePort()
const
{
	const auto world = GetWorld();
	if (nullptr == world)
	{
		const auto settings = GetDefault<USagaNetworkSettings>();
		return settings->RemotePort;
	}

	const auto singleton = world->GetGameInstance();
	if (nullptr == singleton)
	{
		const auto settings = GetDefault<USagaNetworkSettings>();
		return settings->RemotePort;
	}

	const auto net = singleton->GetSubsystem<USagaNetworkSubSystem>();
	if (nullptr == net)
	{
		const auto settings = GetDefault<USagaNetworkSettings>();
		return settings->RemotePort;
	}
	else
	{
		return net->GetRemotePort();
	}
}
