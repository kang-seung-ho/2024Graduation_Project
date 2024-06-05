#include "Saga/Interface/Data/SagaUiInstancedUserData.h"

USagaUiInstancedUserData::USagaUiInstancedUserData()
noexcept
	: Super()
	, virtualMember()
	, isCursorHovered(), isSelected()
{}

void
USagaUiInstancedUserData::SetUserData(const FSagaVirtualUser& virtual_user)
noexcept
{
	virtualMember = virtual_user;
}

FSagaVirtualUser
USagaUiInstancedUserData::GetUserData()
const noexcept
{
	return virtualMember;
}

bool
USagaUiInstancedUserData::IsCursorHovered()
const noexcept
{
	return isCursorHovered;
}

bool
USagaUiInstancedUserData::IsSelected()
const noexcept
{
	return isSelected;
}
