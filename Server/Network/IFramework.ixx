export module Iconer.Net.IFramework;
import Iconer.Utility.Delegate;
import Iconer.Net.ErrorCode;
import Iconer.Net.IoResult;
import <new>;

export namespace iconer::net
{
	class [[nodiscard]] Socket;

	class IFramework
	{
	public:
		iconer::util::Delegate<void> eventOnInitialied;
		iconer::util::Delegate<void> eventOnPostInitialied;
		iconer::util::Delegate<void> eventOnPreStarted;
		iconer::util::Delegate<void> eventOnPostStarted;
		iconer::util::Delegate<void> eventOnDestructed;

		constexpr IFramework() = default;
		virtual constexpr ~IFramework() = default;

		virtual iconer::net::IoResult Initialize();
		virtual void Startup();
		virtual void PostStartup();
		virtual void Cleanup();

		[[nodiscard]]
		constexpr iconer::net::Socket& GetListenSocket() noexcept
		{
			return *listenSocket;
		}

		[[nodiscard]]
		constexpr const iconer::net::Socket& GetListenSocket() const noexcept
		{
			return *listenSocket;
		}

	protected:
		alignas(std::hardware_constructive_interference_size) iconer::net::Socket* listenSocket;

	private:
		IFramework(const IFramework&) = delete;
		IFramework& operator=(const IFramework&) = delete;
		IFramework(IFramework&&) = delete;
		IFramework& operator=(IFramework&&) = delete;
	};
}
