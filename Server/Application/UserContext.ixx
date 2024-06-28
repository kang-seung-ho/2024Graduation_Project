export module Iconer.App.UserContext;
import Iconer.Utility.ReadOnly;
import Iconer.App.TaskContext;
import Iconer.App.ISession;
import <new>;

export namespace iconer::app
{
	class [[nodiscard]] UserContext : public TaskContext
	{
	public:
		using id_type = iconer::app::ISession::id_type;

		iconer::util::ReadOnly<id_type> ownerId;
		iconer::util::ReadOnly<class User*> ownerHandle;

		constexpr UserContext(id_type id, class User* ptr) noexcept
			: ownerId(id), ownerHandle(ptr)
		{}
	};
}
