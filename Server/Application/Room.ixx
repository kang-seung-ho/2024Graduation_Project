export module Iconer.App.Room;
import Iconer.Utility.ReadOnly;
import Iconer.Utility.Delegate;
import Iconer.Utility.Container.AtomicQueue;
import Iconer.Net.ErrorCode;
import Iconer.Net.Socket;
import Iconer.App.ISession;
import Iconer.App.TaskContext;
import Iconer.App.Settings;
import <array>;
import <atomic>;

export namespace iconer::app
{
	class [[nodiscard]] User;

	class [[nodiscard]] Member
	{
	public:
		iconer::app::User* userHandle{};
		std::atomic_bool isReady{};

		Member() noexcept = default;

		friend class Room;
	};

	class [[nodiscard]] Room : public ISession
	{
	public:
		using super = ISession;
		using this_class = Room;
		using id_type = super::id_type;

		static inline constexpr size_t userLimits = Settings::roomMembersLimit;

		std::array<Member, userLimits> myMembers{};



		//iconer::util::AtomicQueue2<Member, userLimits> myMembers{};
	};
}
