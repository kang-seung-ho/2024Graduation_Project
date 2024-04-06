module;
#include <string_view>
module Demo.Framework;

struct EndToEndRemover final : public iconer::app::Room::MemberRemover
{
	using Super = iconer::app::Room::MemberRemover;

	constexpr EndToEndRemover(demo::Framework& framework) noexcept
		: Super(), myFramework(framework)
	{
	}

	void operator()(volatile iconer::app::Room& room, const size_t& members_count) const noexcept override
	{
		if (0 == members_count)
		{
			room.BeginClose();
			room.SetOperation(iconer::app::AsyncOperations::OpCloseRoom);

			if (not myFramework.Schedule(room, room.GetID()))
			{
				room.Cleanup();
			}
		}
	}

	demo::Framework& myFramework;
};

demo::Framework::AcceptResult
demo::Framework::OnUserDisconnected(iconer::app::User& user)
{
	static EndToEndRemover remover{ *this };

	// Reserve the user again
	if (user.EndClose())
	{
		// Make room out now
		if (auto room_id = user.myRoomId.Exchange(-1); -1 != room_id)
		{
			if (auto room = FindRoom(room_id); nullptr != room)
			{
				if (room->RemoveMember(user.GetID(), remover))
				{
					SetRoomModifiedFlag();
				}
			}
		}

		user.Cleanup();

		user.SetOperation(iconer::app::AsyncOperations::OpReserveSession);
		if (Schedule(user, user.GetID()))
		{
			return std::nullopt;
		}
	}

	return iconer::net::ErrorCode::OPERATION_ABORTED;
}
