module;
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

module Iconer.Framework;
import Iconer.App.SendContext;
import Iconer.App.PacketSerializer;

void
ServerFramework::EventOnUserList(iconer::app::User& user, std::byte* data)
{
	const auto room = user.myRoom.load();

	if (nullptr != room) LIKELY
	{
		user.SendGeneralData(*AcquireSendContext(), room->MakeMemberListPacket());
	};
}

void
ServerFramework::EventOnChangeTeam(iconer::app::User& user, std::byte* data)
{
	using enum iconer::app::TaskCategory;

	const auto room = user.myRoom.load();

	if (nullptr != room) LIKELY
	{
		auto & ctx = user.roomContext;

		const auto team_id = static_cast<std::int8_t>(*data);

		if (user.IsConnected() and ctx->TryChangeOperation(None, OpNotifyTeam)) LIKELY
		{
			if (team_id == 0) // red team
			{
				room->SetMemberTeam(user, true);
			}
			else if (team_id == 1) // blue team
			{
				room->SetMemberTeam(user, false);
			}
			else UNLIKELY
			{
				ctx->TryChangeOperation(OpNotifyTeam, None);
				return;
			};

			if (not myTaskPool.Schedule(ctx, user.GetID(), team_id)) UNLIKELY
			{
				ctx->TryChangeOperation(OpNotifyTeam, None);
			};
		};
	};
}

void
ServerFramework::EventOnNotifyTeamChanged(iconer::app::User& user, std::uint32_t team_id)
{
	using enum iconer::app::PacketProtocol;
	using enum iconer::app::TaskCategory;

	auto& ctx = user.roomContext;

	const auto room = user.myRoom.load();

	if (nullptr != room) LIKELY
	{
		// broadcast
		room->Foreach(
			[sender = AcquireSendContext()
			, id = static_cast<std::int32_t>(user.GetID())
			, team = (1 == team_id) ? (char)0 : (char)1](iconer::app::User& mem)
			{
				auto [pk, length] = iconer::app::Serialize(SC_SET_TEAM, id, team);

				sender->myBuffer = std::move(pk);

				mem.SendGeneralData(*sender, static_cast<size_t>(length));
			}
		);
	};

	ctx->TryChangeOperation(OpNotifyTeam, None);
}

void
ServerFramework::EventOnGameStartSignal(iconer::app::User& user, std::byte* data)
{
	using enum iconer::app::PacketProtocol;
	using enum iconer::app::TaskCategory;

	const auto room = user.myRoom.load();

	if (nullptr != room) LIKELY
	{
		if (room->TryStartGame())
		{
			room->Foreach([&](iconer::app::User& mem)
			{
				auto [pk, length] = iconer::app::Serialize(SC_SET_TEAM, id, team);

				sender->myBuffer = std::move(pk);

				mem.SendGeneralData(*sender, static_cast<size_t>(length));
			}
		}
				};
}

void
ServerFramework::EventOnGameReadySignal(iconer::app::User& user, std::byte* data)
{
	const auto room = user.myRoom.load();

	if (nullptr != room) LIKELY
	{

	};
}
