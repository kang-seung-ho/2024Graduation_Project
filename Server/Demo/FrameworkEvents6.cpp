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

using namespace iconer::app;

bool
demo::Framework::OnCreatingCharacters(Room& room)
{
	auto number = room.GetMembersCount();
	if (number == 0)
	{
		return false;
	}

	auto& cnt_ref = room.proceedMemberCount;

	room.ForEach
	(
		[&](User& member)
		{
			auto [io, ctx] = member.SendCreateCharactersPacket(); 
			if (not io)
			{
				ctx.Complete();
			}

			cnt_ref.FetchAdd(1);
		}
	);

#if false
	if (cnt == 0)
	{
		return false;
	}
#else
	if (0 < cnt_ref)
	{
		return true;
	}
#endif

	return false;
}

void
demo::Framework::OnFailedToCreateCharacters(Room& room)
noexcept
{

}
