export module Iconer.Net.IoCompletionPort;
import Iconer.Utility.Handler;
import Iconer.Net.ErrorCode;
export import Iconer.Net.IoContext; 
export import Iconer.Net.IoEvent;
export import Iconer.Net.Socket;
import <cstdint>;
import <expected>;
import <span>;

export namespace iconer::net
{
	class IoCompletionPort final : public iconer::util::Handler<void*>
	{
	public:
		explicit constexpr IoCompletionPort() noexcept = default;
		~IoCompletionPort() noexcept = default;

		net::Socket::ActionResult Register(net::Socket& socket, std::uintptr_t id) noexcept;
		bool TryRegister(net::Socket& socket, std::uintptr_t id, net::ErrorCode& error_code) noexcept;
		bool Destroy() noexcept;
		bool Destroy(ErrorCode& error_code) noexcept;

		bool Schedule(IoContext& context, std::uintptr_t id, const unsigned long& infobytes) noexcept;
		bool Schedule(IoContext* const context, std::uintptr_t id, const unsigned long& infobytes) noexcept;
		bool Schedule(IoContext& context, std::uintptr_t id, unsigned long&& infobytes) noexcept;
		bool Schedule(IoContext* const context, std::uintptr_t id, unsigned long&& infobytes) noexcept;
		bool Schedule(volatile IoContext& context, std::uintptr_t id, const unsigned long& infobytes) noexcept;
		bool Schedule(volatile IoContext* const context, std::uintptr_t id, const unsigned long& infobytes) noexcept;
		bool Schedule(volatile IoContext& context, std::uintptr_t id, unsigned long&& infobytes) noexcept;
		bool Schedule(volatile IoContext* const context, std::uintptr_t id, unsigned long&& infobytes) noexcept;
		[[nodiscard]] IoEvent WaitForIoResult() noexcept;
		[[nodiscard]] net::Socket::AsyncResult WaitForMultipleIoResults(std::span<IoEvent> dest, const unsigned long& max_count) noexcept;

		[[nodiscard]]
		bool IsAvailable() const noexcept;

		using FactoryResult = std::expected<IoCompletionPort, net::ErrorCode>;
		[[nodiscard]] static FactoryResult Create() noexcept;
		[[nodiscard]] static FactoryResult Create(std::uint32_t concurrency_hint) noexcept;

		IoCompletionPort(IoCompletionPort&&) noexcept = default;
		IoCompletionPort& operator=(IoCompletionPort&&) noexcept = default;

	private:
		IoCompletionPort(void* handle) noexcept;

		IoCompletionPort(const IoCompletionPort&) = delete;
		IoCompletionPort& operator=(const IoCompletionPort&) = delete;
	};
}
