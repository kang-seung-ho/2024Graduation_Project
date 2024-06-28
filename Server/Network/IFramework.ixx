export module Iconer.Net.IFramework;
import Iconer.Utility.Delegate;
import Iconer.Net.ErrorCode;
import Iconer.Net.Socket;
import <new>;

export namespace iconer::net
{
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

	protected:
		alignas(std::hardware_constructive_interference_size) iconer::net::Socket listenSocket;

	private:
		IFramework(const IFramework&) = delete;
		IFramework& operator=(const IFramework&) = delete;
		IFramework(IFramework&&) = delete;
		IFramework& operator=(IFramework&&) = delete;
	};
}
