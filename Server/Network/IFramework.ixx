export module Iconer.Net.IFramework;
import Iconer.Net.ErrorCode;
import Iconer.Net.Socket;
import <expected>;

export namespace iconer::net
{
	class IFramework
	{
	public:
		constexpr IFramework() = default;
		virtual constexpr ~IFramework() = default;

		virtual std::expected<void, iconer::net::ErrorCode> Initialize() = 0;
		virtual void Startup() = 0;
		virtual void Cleanup();

	protected:
		iconer::net::Socket listenSocket;

	private:
		IFramework(const IFramework&) = delete;
		IFramework& operator=(const IFramework&) = delete;
		IFramework(IFramework&&) = default;
		IFramework& operator=(IFramework&&) = default;
	};
}
