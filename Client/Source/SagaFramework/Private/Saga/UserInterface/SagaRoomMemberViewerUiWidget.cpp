#include "Saga/Interface/SagaRoomMemberViewerUiWidget.h"
#include <Components/Widget.h>
#include <Components/ListView.h>
#include <Templates/Casts.h>
#include <Blueprint/WidgetTree.h>

#include "Saga/Interface/SagaUiButton.h"

USagaRoomMemberViewerUiWidget::USagaRoomMemberViewerUiWidget(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
	, myListView(), myJoinButton()
{}

void
USagaRoomMemberViewerUiWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WidgetTree->ForEachWidget([this](UWidget* const element) -> void
		{
			if (element == this) return;

			if (auto listview = Cast<UListView>(element))
			{
				myListView = listview;
			}
			else if (auto btn = Cast<USagaUiButton>(element))
			{
				myJoinButton = btn;
			}
		}
	);

	const auto my_name = GetName();
	if (nullptr == myListView)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaRoomMemberViewerUiWidget] '%s' could not found the team member list in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaRoomMemberViewerUiWidget] '%s' found the team member list in children."), *my_name);
	}

	if (nullptr == myJoinButton)
	{
		UE_LOG(LogSagaFramework, Fatal, TEXT("[USagaRoomMemberViewerUiWidget] '%s' could not found the team join button in children."), *my_name);
	}
	else
	{
		UE_LOG(LogSagaFramework, Log, TEXT("[USagaRoomMemberViewerUiWidget] '%s' found the team join button in children."), *my_name);
	}
}
