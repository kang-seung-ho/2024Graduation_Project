module Iconer.Framework;

import Iconer.App.User;
import Iconer.App.TaskCategory;

void
ServerFramework::EventOnSignIn(iconer::app::User& user, std::byte* data)
{
	using enum iconer::app::TaskCategory;

	auto& ctx = user.mainContext;

	if (ctx->TryChangeOperation(OpValidation, OpAssignID))
	{
		constexpr auto len = iconer::app::Settings::nickNameLength;

		wchar_t nickname[len + 1]{};

		std::memcpy(nickname, data, len * 2);

		user.myName = nickname;

		user.SendSignInPacket();
	}
	else
	{
		ctx->SetOperation(OpSignInFailed);
		user.SendFailedSignInPacket(iconer::app::ConnectionContract::SignInFailed);
	}
}
