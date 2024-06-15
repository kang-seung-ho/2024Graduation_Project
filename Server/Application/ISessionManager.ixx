export module Iconer.App.ISessionManager;
import Iconer.Utility.TypeTraits;
import Iconer.App.ISession;

export namespace iconer::app
{
	template<std::derived_from<ISession> Session>
	class [[nodiscard]] ISessionManager
	{
	public:
		using id_type = ISession::id_type;

		constexpr ISessionManager() = default;
		virtual constexpr ~ISessionManager() = default;

		virtual bool Initialize() = 0;
		virtual void Startup() = 0;
		virtual void Cleanup() = 0;

		virtual void AddSession(Session* session) = 0;
		virtual Session* FindSession(id_type id) const = 0;

	private:
		ISessionManager(const ISessionManager&) = delete;
		ISessionManager& operator=(const ISessionManager&) = delete;
		ISessionManager(ISessionManager&&) = delete;
		ISessionManager& operator=(ISessionManager&&) = delete;
	};
}
