module;
#include <string_view>

#define SEND(user_var, method, ...) \
auto [io, ctx] = ((user_var).method)(__VA_ARGS__); \
if (not io) \
{ \
	ctx.Complete(); \
}
#define SEND_RESULT(captures, user_var, method, ...) \
[captures](){ \
	auto [io, ctx] = ((user_var).method)(__VA_ARGS__); \
	if (not io) \
	{ \
		ctx.Complete(); \
		return false; \
	} \
	return true; \
}()
module Demo.Framework;

bool
demo::Framework::OnCreatingCharacters(iconer::app::Room& room)
{
	auto number = room.GetMembersCount();
	if (number == 0)
	{
		return false;
	}

	auto& cnt_ref = room.proceedMemberCount;
	if (not cnt_ref.CompareAndSet(number - 1, 0, std::memory_order_acq_rel))
	{
		return false;
	}

	auto cnt = cnt_ref.Load(std::memory_order_acquire);

	room.ForEach
	(
		[&](iconer::app::User& member)
		{
			auto [io, ctx] = member.SendCreateCharactersPacket(); 
			if (not io)
			{
				ctx.Complete();
			}

			cnt = cnt_ref.FetchAdd(1, std::memory_order_release) + 1;
		}
	);

#if false
	if (cnt == 0)
	{
		return false;
	}
#else
	if (0 < cnt)
	{
		return true;
	}
#endif

	return false;
}

void
demo::Framework::OnFailedToCreateCharacters(iconer::app::Room& room)
noexcept
{

}
