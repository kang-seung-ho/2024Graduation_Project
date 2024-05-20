#include "Saga/Interface/Data/SagaUiInstancedRoomData.h"

USagaUiInstancedRoomData::USagaUiInstancedRoomData() noexcept
	: Super()
	, virtualRoom()
	, isCursorHovered(), isSelected()
{}

void
USagaUiInstancedRoomData::SetRoomData(const FSagaVirtualRoom& virtual_room)
noexcept
{
	virtualRoom = virtual_room;
}

FSagaVirtualRoom
USagaUiInstancedRoomData::GetRoomData()
const noexcept
{
	return virtualRoom;
}

bool
USagaUiInstancedRoomData::IsCursorHovered()
const noexcept
{
	return isCursorHovered;
}

bool
USagaUiInstancedRoomData::IsSelected()
const noexcept
{
	return isSelected;
}
