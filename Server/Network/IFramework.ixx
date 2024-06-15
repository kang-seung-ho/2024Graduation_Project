module;
#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

export module Iconer.Net.IFramework;
import Iconer.Utility.TypeTraits;
import Iconer.Net.ErrorCode;
import Iconer.Net.Socket;
import Iconer.Net.IoContext;
import Iconer.Net.IoCompletionPort;
import <cstdint>;
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
		virtual void Cleanup() = 0;

	protected:
		iconer::net::Socket listenSocket;

	private:
		IFramework(const IFramework&) = delete;
		IFramework& operator=(const IFramework&) = delete;
		IFramework(IFramework&&) = default;
		IFramework& operator=(IFramework&&) = default;
	};
}
