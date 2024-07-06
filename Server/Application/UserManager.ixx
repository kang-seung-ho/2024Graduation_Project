export module Iconer.App.UserManager;
import Iconer.Utility.TypeTraits;
import Iconer.Net.ErrorCode;
import Iconer.Net.IoResult;
import Iconer.Net.SocketPool;
import Iconer.App.ISession;
import Iconer.App.Settings;
import <vector>;

export namespace iconer::net
{
	class [[nodiscard]] Socket;
	class [[nodiscard]] IoCompletionPort;
}

export namespace iconer::app
{
	class User;

	class [[nodiscard]] UserManager
	{
	public:
		using id_type = iconer::app::ISession::id_type;
		using session_type = iconer::app::User;
		using pointer_type = iconer::app::User*;

		static inline constexpr size_t maxUserCount = iconer::app::Settings::usersLimit;
		static inline constexpr id_type minUserUid = 1000;
		static inline constexpr id_type maxUserUid = minUserUid + maxUserCount;

		explicit constexpr UserManager() noexcept = default;
		constexpr ~UserManager() noexcept = default;

		iconer::net::IoResult Initialize(iconer::net::IoCompletionPort& io_port);
		void Startup(class iconer::net::Socket& listener);
		void Cleanup();

		template<invocable<session_type&> Callable>
		constexpr void Foreach(Callable&& fun) const noexcept(nothrow_invocable<Callable, session_type&>)
		{
			for (const pointer_type& ptr : everyUsers)
			{
				std::forward<Callable>(fun)(*ptr);
			}
		}

		template<invocable<const session_type&> Callable>
		constexpr void Foreach(Callable&& fun) const noexcept(nothrow_invocable<Callable, const session_type&>)
		{
			for (const pointer_type& ptr : everyUsers)
			{
				std::forward<Callable>(fun)(*ptr);
			}
		}

		template<invocable<pointer_type> Callable>
		constexpr void Foreach(Callable&& fun) const noexcept(nothrow_invocable<Callable, pointer_type>)
		{
			for (const pointer_type& ptr : everyUsers)
			{
				std::forward<Callable>(fun)(ptr);
			}
		}

		[[nodiscard]]
		pointer_type FindUser(id_type id) const noexcept;

	private:
		alignas(std::hardware_constructive_interference_size) iconer::net::SocketPool everySockets{};
		alignas(std::hardware_constructive_interference_size) std::vector<pointer_type> everyUsers{};
		alignas(std::hardware_constructive_interference_size) class iconer::net::IoCompletionPort* ioCompletionPort{ nullptr };

		void AddUser(pointer_type session);

		UserManager(const UserManager&) = delete;
		UserManager& operator=(const UserManager&) = delete;
		UserManager(UserManager&&) = delete;
		UserManager& operator=(UserManager&&) = delete;
	};
}
