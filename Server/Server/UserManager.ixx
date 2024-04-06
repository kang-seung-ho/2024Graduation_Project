module;
#include <concurrent_unordered_map.h>

export module Iconer.Application.UserManager;
import Iconer.Application.ISessionManager;
export import Iconer.Application.User;
import <vector>;
import <mutex>;

template<>
struct std::hash<iconer::app::User*>
{
	[[nodiscard]]
	auto operator()(const iconer::app::User* const ptr) const noexcept
	{
		if (nullptr == ptr)
		{
			return std::_Hash_representation(ptr);
		}
		else
		{
			return std::_Hash_representation(ptr->GetID());
		}
	}
};

export namespace iconer::app
{
	class [[nodiscard]] UserManager final : public ISessionManager<User>
	{
	public:
		using Super = ISessionManager<User>;
		using IdType = Super::key_type;

		void AddActiveMember(const IdType id)
		{
			std::lock_guard lock{ activatedUsersLock };
			activeUsers.insert(std::make_pair(id, this->operator[](id)));
		}

		bool RemoveActiveMember(const IdType id)
		{
			std::lock_guard lock{ activatedUsersLock };
			return 0 < activeUsers.unsafe_erase(id);
		}

		[[nodiscard]]
		std::vector<User*> GetActiveUsers() const
		{
			std::vector<User*> result{};
		}

		bool IsActivated(const IdType id) const
		{
			auto&& it = activeUsers.find(id);
			if (activeUsers.cend() == it)
			{
				return false;
			}
			else
			{
				auto&& user = (*it).second;

				if (user->GetState() != UserStates::None)
				{
					return true;
				}
			}

			return false;
		}

	private:
		//iconer::collection::ConcurrentMap<IdType, User*> activeUsers;
		concurrency::concurrent_unordered_map<IdType, User*> activeUsers;
		std::recursive_mutex activatedUsersLock;
	};
}
