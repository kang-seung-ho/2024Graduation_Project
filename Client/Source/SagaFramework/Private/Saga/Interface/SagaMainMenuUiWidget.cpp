#include "Saga/Interface/SagaMainMenuUiWidget.h"
#include <Internationalization/Text.h>
#include <Async/Future.h>
#include <Async/TaskGraphInterfaces.h>
#include <Templates/UnrealTemplate.h>
#include <Blueprint/WidgetTree.h>

#include "Saga/Network/SagaNetworkSubSystem.h"

USagaMainMenuUiWidget::USagaMainMenuUiWidget(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, myInputBox(), mySigninButton()
	, transientNickname()
	, isConnecting(false)
{}

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
USagaMainMenuUiWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	auto system = USagaNetworkSubSystem::GetSubSystem(GetWorld());
	//RegisterView(system, this);
	system->OnConnected.AddDynamic(this, &USagaMainMenuUiWidget::OnConnected);
	system->OnFailedToConnect.AddDynamic(this, &USagaMainMenuUiWidget::OnFailedToConnect);

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
			if (element == this) return;

			if (auto box = Cast<USagaUiEditableTextBox>(element))
			{
				myInputBox = box;
			}
			else if (auto btn = Cast<USagaUiButton>(element))
			{
				mySigninButton = btn;
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

		myInputBox->OnTextChanged.AddUniqueDynamic(this, &USagaMainMenuUiWidget::HandleInputTextChanged);
		myInputBox->OnTextCommitted.AddUniqueDynamic(this, &USagaMainMenuUiWidget::HandleInputCommitted);

		transientNickname = myInputBox->GetText();
	}

	if (nullptr == mySigninButton)
	{
		UE_LOG(LogSagaFramework, Error, TEXT("[USagaMainMenuUiWidget] '%s' found no button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaMainMenuUiWidget] '%s' found button in children."), *my_name);

		mySigninButton->OnClicked.AddUniqueDynamic(this, &USagaMainMenuUiWidget::HandleButtonPressed);
		mySigninButton->SetUserInteractable(false);
	}
}

void
USagaMainMenuUiWidget::HandleSigningIn()
{
	if (isConnecting)
	{
		UE_LOG(LogSagaFramework, Warning, TEXT("[USagaMainMenuUiWidget] Client is already connecting."));
		return;
	}

	isConnecting = true;
	myInputBox->SetIsEnabled(false);
	mySigninButton->SetUserInteractable(false);

	if (transientNickname.IsEmpty())
	{
		UE_LOG(LogSagaFramework, Warning, TEXT("[USagaMainMenuUiWidget] Nickname is empty."));
		return;
	}

	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask,
		[this
		, nickname = transientNickname.ToString()
		, system = USagaNetworkSubSystem::GetSubSystem(GetWorld())]()
		{
			if (system)
			{
				UE_LOG(LogSagaFramework, Warning, TEXT("[USagaMainMenuUiWidget] Nickname: %s"), *nickname);

				(void)system->ConnectToServer(FName{ nickname });
			}
			else
			{
				UE_LOG(LogSagaFramework, Error, TEXT("[USagaMainMenuUiWidget] Network subsystem is not ready."));

				AsyncTask(ENamedThreads::GameThread, [this]()
					{
						myInputBox->SetIsEnabled(true);
						mySigninButton->SetUserInteractable(true);
						isConnecting = false;
					}
				);

				(void)ESagaConnectionContract::SubSystemError;
			}
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
