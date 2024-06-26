export module Iconer.App.UserContext;
import Iconer.Utility.ReadOnly;
import Iconer.App.TaskContext;

export namespace iconer::app
{
	class [[nodiscard]] UserContext : public TaskContext
	{
	public:
		iconer::util::ReadOnly<std::int32_t> ownerId;
		iconer::util::ReadOnly<class User*> ownerHandle;

		constexpr UserContext(std::int32_t id, class User* ptr) noexcept
			: ownerId(id), ownerHandle(ptr)
		{}
	};
}
