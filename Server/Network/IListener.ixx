module;
#include <type_traits>

#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

export module Iconer.Net.IListener;
import Iconer.Net.ErrorCode;
import Iconer.Net.Socket;
import Iconer.Net.IoContext;
import Iconer.Net.IoCompletionPort;
import <cstdint>;
import <expected>;
import <array>;
import <unordered_map>;
import <thread>;
import <latch>;

export namespace iconer::net
{
	class IListener
	{
	public:
		IListener() = default;
		~IListener() = default;

		std::expected<void, iconer::net::ErrorCode> Initialize();
		void Startup();
		void Cleanup();

	private:
		iconer::net::Socket listenSocket;

		IListener(const IListener&) = delete;
		IListener& operator=(const IListener&) = delete;
		IListener(IListener&&) = default;
		IListener& operator=(IListener&&) = default;
	};
}
