#include "Saga/Interface/SagaRoomSessionLevelUiWidget.h"
#include <Components/Widget.h>
#include <Components/ListView.h>
#include <Templates/Casts.h>
#include <Blueprint/WidgetTree.h>

#include "Saga/Interface/SagaUiButton.h"
#include "Saga/Interface/SagaRoomMemberViewerUiWidget.h"

#include "Saga/Interface/Data/SagaUiInstancedUserData.h"

USagaRoomSessionLevelUiWidget::USagaRoomSessionLevelUiWidget(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, myRedTeamMemberListView(), myBlueTeamMemberListView()
	, myStartButton(), myQuitButton()
{}

void
USagaRoomSessionLevelUiWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
			if (element == this) return;

			if (auto viwer = Cast<USagaRoomMemberViewerUiWidget>(element))
			{
				const auto name = viwer->GetName();

				if (name == TEXT("RedTeamListViewerPanel"))
				{
					myRedTeamMemberListView = viwer;
				}
				else if (name == TEXT("BlueTeamListViewerPanel"))
				{
					myBlueTeamMemberListView = viwer;
				}
			}
			else if (auto btn = Cast<USagaUiButton>(element))
			{
				const auto name = btn->GetName();

				if (name == TEXT("StartButton"))
				{
					myStartButton = btn;
				}
				else if (name == TEXT("QuitButton"))
				{
					myQuitButton = btn;
				}
			}
		}
	);

	const auto my_name = GetName();
	if (nullptr == myRedTeamMemberListView)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaRoomSessionLevelUiWidget] '%s' could not found the red team panel in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaRoomSessionLevelUiWidget] '%s' found the red team panel in children."), *my_name);
	}

	if (nullptr == myBlueTeamMemberListView)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaRoomSessionLevelUiWidget] '%s' could not found the red team panel in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaRoomSessionLevelUiWidget] '%s' found the red team panel in children."), *my_name);
	}
	
	if (nullptr == myStartButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaRoomSessionLevelUiWidget] '%s' could not found the starting game button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaRoomSessionLevelUiWidget] '%s' found the starting game button in children."), *my_name);
	}

	if (nullptr == myQuitButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaRoomSessionLevelUiWidget] '%s' could not found the quit button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaRoomSessionLevelUiWidget] '%s' found the quit button in children."), *my_name);
	}
}

void
USagaRoomSessionLevelUiWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}
