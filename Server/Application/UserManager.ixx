export module Iconer.App.UserManager;
import Iconer.Utility.TypeTraits;
import Iconer.Net.ErrorCode;
import Iconer.Net.Socket;
import Iconer.Net.IoCompletionPort;
import Iconer.App.ISession;
import Iconer.App.User;
import <memory>;
import <expected>;
import <vector>;

export namespace iconer::app
{
	class [[nodiscard]] UserManager
	{
	public:
		using id_type = ISession::id_type;
		using session_type = iconer::app::User;
		using pointer_type = std::unique_ptr<iconer::app::User>;

		static inline constexpr size_t maxUserCount = 10;
		static inline constexpr id_type minUserUid = 1;
		static inline constexpr id_type maxUserUid = minUserUid + maxUserCount;

		constexpr UserManager() noexcept = default;
		constexpr ~UserManager() noexcept = default;

		std::expected<void, iconer::net::ErrorCode> Initialize(iconer::net::IoCompletionPort& io_port);
		void Startup(iconer::net::Socket& listener);
		void Cleanup();

		void AddSession(session_type* session);
		session_type* FindSession(id_type id) const noexcept;

	private:
		std::vector<pointer_type> everyUsers{};
		iconer::net::IoCompletionPort* ioCompletionPort{ nullptr };

		UserManager(const UserManager&) = delete;
		UserManager& operator=(const UserManager&) = delete;
		UserManager(UserManager&&) = delete;
		UserManager& operator=(UserManager&&) = delete;
	};
}
