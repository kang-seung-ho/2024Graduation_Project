module;
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]
#include <chrono>

module Iconer.Framework;
import Iconer.App.SendContext;
import Iconer.App.PacketSerializer;

namespace
{
	inline constexpr std::chrono::seconds weaponPhasePeriod{ 31 };
	inline constexpr std::chrono::seconds gamePhasePeriod{ 301 };
}

void
ServerFramework::EventOnUserList(iconer::app::User& user, const std::byte* data)
{
	iconer::app::Room* const room = user.GetRoom();

	if (nullptr != room) LIKELY
	{
		user.SendGeneralData(*AcquireSendContext(), room->MakeMemberListPacket());
	};
}

void
ServerFramework::EventOnChangeTeam(iconer::app::User& user, const std::byte* data)
{
	using enum iconer::app::TaskCategory;

	iconer::app::Room* const room = user.GetRoom();

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

	iconer::app::Room* const room = user.GetRoom();

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
ServerFramework::EventOnGameStartSignal(iconer::app::User& user, const std::byte* data)
{
	using enum iconer::app::PacketProtocol;
	using enum iconer::app::TaskCategory;

	iconer::app::Room* const room = user.GetRoom();

	if (nullptr != room) LIKELY
	{
		auto & ctx = user.mainContext;

		if (ctx->TryChangeOperation(OpSignIn, OpCreateGame))
		{
			if (not myTaskPool.Schedule(ctx, user.GetID(), static_cast<unsigned long>(room->GetID())))
			{
				ctx->TryChangeOperation(OpCreateGame, OpSignIn);
			}
		}
	}
}

void
ServerFramework::EventOnMakeGame(iconer::app::User& user)
{
	auto& master_ctx = user.mainContext;

	using enum iconer::app::PacketProtocol;
	using enum iconer::app::TaskCategory;

	iconer::app::Room* const room = user.GetRoom();
	bool cancelled = false;

	if (nullptr != room) LIKELY
	{
		if (room->TryStartGame()) LIKELY
		{
			room->startingMemberProceedCount = 0;
			room->readyMemberProceedCount = 0;
			room->gameLoadedMemberProceedCount = 0;

			room->selectionPhaseTime = std::chrono::system_clock::now() + weaponPhasePeriod;

			if (0 < room->GetMemberCount()) LIKELY
			{
				master_ctx->TryChangeOperation(OpCreateGame, OpSignIn);

				auto& cnt = room->startingMemberProceedCount;

				room->Foreach([&](iconer::app::User& mem)
					{
						auto& ctx = mem.mainContext;

						if (ctx->TryChangeOperation(OpSignIn, OpSpreadGameTicket)) LIKELY
						{
							const auto mem_room = mem.GetRoom();

							if (nullptr == mem_room) UNLIKELY
							{
								ctx->TryChangeOperation(OpSpreadGameTicket, None);

								if (not cancelled) UNLIKELY
								{
									cancelled = true;
									cnt.fetch_add(1);
								};
							}
							else if (not mem.IsConnected()) UNLIKELY
							{
								ctx->TryChangeOperation(OpSpreadGameTicket, None);
								mem.myRoom = nullptr;

								if (not cancelled) UNLIKELY
								{
									cancelled = true;
									cnt.fetch_add(1);
								};
							}
							else if (mem_room != room) UNLIKELY
							{
								ctx->TryChangeOperation(OpSpreadGameTicket, OpSignIn);

								if (not cancelled) UNLIKELY
								{
									cancelled = true;
									cnt.fetch_add(1);
								};
							}
							else if (cancelled) UNLIKELY
							{
								cnt.fetch_add(1);
							}
							else LIKELY
							{
								mem.SendGeneralData(ctx, ctx->GetGameReadyPacketData());
							};
						}
						else
						{
							cnt.fetch_add(1);
						}
					}
				);
			}
			else UNLIKELY
			{
				cancelled = true;
			};
		};
	};

	if (cancelled) UNLIKELY
	{
		room->Foreach([&](iconer::app::User& mem)
			{
				auto& ctx = mem.mainContext;

				if (user.IsConnected())
				{
					ctx->TryChangeOperation(OpSpreadGameTicket, OpSignIn);
				}
				else
				{
					ctx->TryChangeOperation(OpSpreadGameTicket, None);
				}
			}
		);

		room->CancelStartGame();
	};
}

void
ServerFramework::EventOnSpreadGameTickets(iconer::app::User& user)
{
	using enum iconer::app::PacketProtocol;
	using enum iconer::app::TaskCategory;

	iconer::app::Room* const room = user.GetRoom();

	auto& ctx = user.mainContext;

	if (not user.IsConnected()) UNLIKELY
	{
		ctx->TryChangeOperation(OpSpreadGameTicket, None);
		user.myRoom = nullptr;
	}
	else
	{
		if (nullptr != room) LIKELY
		{
			auto & cnt = room->startingMemberProceedCount;
			cnt.fetch_add(1);
		}
		else UNLIKELY
		{
			ctx->TryChangeOperation(OpSpreadGameTicket, OpSignIn);
		};
	};
}

void
ServerFramework::EventOnFailedToSpreadGameTickets(iconer::app::User& user)
{
	using enum iconer::app::PacketProtocol;
	using enum iconer::app::TaskCategory;

	iconer::app::Room* const room = user.GetRoom();

	auto& ctx = user.mainContext;

	if (not user.IsConnected()) UNLIKELY
	{
		ctx->TryChangeOperation(OpSpreadGameTicket, None);
		user.myRoom = nullptr;
	}
	else if (room != nullptr) LIKELY
	{
		auto & cnt = room->startingMemberProceedCount;
		cnt.fetch_add(1);

		ctx->TryChangeOperation(OpSpreadGameTicket, OpSignIn);
	}
	else
	{

	};
}

void
ServerFramework::EventOnGameReadySignal(iconer::app::User& user, const std::byte* data)
{
	using enum iconer::app::PacketProtocol;
	using enum iconer::app::TaskCategory;
	using enum iconer::app::RoomState;

	iconer::app::Room* const room = user.GetRoom();

	if (nullptr != room) LIKELY
	{
		auto & cnt = room->readyMemberProceedCount;

		cnt.fetch_add(1);

		if (room->GetMemberCount() <= cnt)
		{
			iconer::app::RoomState state = PrepareGame;

			// now let's start a game
			room->gamePhaseTime = std::chrono::system_clock::now() + gamePhasePeriod;

			if (room->myState.compare_exchange_strong(state, Gaming))
			{
				bool cancelled = false;

				room->Foreach([&](iconer::app::User& member)
					{
						auto& ctx = member.mainContext;

						if (not ctx->TryChangeOperation(OpSpreadGameTicket, OpStartGame))
						{
							cancelled = true;
						}
					}
				);

				if (not cancelled)
				{
					room->Foreach([&](iconer::app::User& member)
						{
							// OpStartGame
							auto& ctx = member.mainContext;

							member.SendGeneralData(ctx, ctx->GetGameStartPacketData());
						}
					);
				}
				else
				{
					room->Foreach([&](iconer::app::User& member)
						{
							auto& ctx = member.mainContext;

							ctx->TryChangeOperation(OpStartGame, OpSignIn);
						}
					);

					iconer::app::RoomState gaming_state = Gaming;

					room->myState.compare_exchange_strong(gaming_state, Idle);
				}
			}
			else
			{
				// TODO
			}
		}
	};
}

void
ServerFramework::EventOnGameStarted(iconer::app::User& user)
{
	using enum iconer::app::PacketProtocol;
	using enum iconer::app::TaskCategory;
	using enum iconer::app::RoomState;

	iconer::app::Room* const room = user.GetRoom();

	if (nullptr != room) LIKELY
	{
		auto & cnt = room->gameLoadedMemberProceedCount;

		cnt.fetch_add(1);

		if (room->GetMemberCount() <= cnt)
		{
			room->Foreach
			(
				[&](iconer::app::User& member)
				{
					const auto ctx = AcquireSendContext();

					auto& mem_ctx = member.mainContext;

					member.SendGeneralData(*ctx, mem_ctx->GetCreatCharactersPacketData());
				}
			);
		}
	};
}
