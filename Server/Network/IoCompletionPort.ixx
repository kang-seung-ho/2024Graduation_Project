export module Iconer.Net.IoCompletionPort;
export import Iconer.Net.ErrorCode;
export import Iconer.Net.IoEvent;
import Iconer.Net.IoResult;
import <cstdint>;
import <span>;

export namespace iconer::net
{
	class Socket;
	class IoContext;

	class [[nodiscard]] IoCompletionPort final
	{
	public:
		explicit constexpr IoCompletionPort() noexcept = default;
		~IoCompletionPort() noexcept = default;

		bool Destroy() noexcept;
		bool Destroy(ErrorCode& error_code) noexcept;

		iconer::net::IoResult Register(iconer::net::Socket& socket, std::uintptr_t id) const noexcept;
		bool TryRegister(net::Socket& socket, std::uintptr_t id, iconer::net::ErrorCode& error_code) const noexcept;

		bool Schedule(IoContext& context, std::uintptr_t id, unsigned long infobytes = 0) const noexcept;

		bool Schedule(IoContext* const context, std::uintptr_t id, unsigned long infobytes = 0) const noexcept;

		bool Schedule(volatile IoContext& context, std::uintptr_t id, unsigned long infobytes = 0) const noexcept;

		bool Schedule(volatile IoContext* const context, std::uintptr_t id, unsigned long infobytes = 0) const noexcept;

		[[nodiscard]] IoEvent WaitForIoResult() const noexcept;
		[[nodiscard]] iconer::net::IoResult WaitForMultipleIoResults(std::span<IoEvent> dest, unsigned long max_count) const;

		[[nodiscard]]
		constexpr bool IsAvailable() const noexcept
		{
			return nullptr != myHandle;
		}
		
		[[nodiscard]]
		constexpr operator bool() const noexcept
		{
			return nullptr != myHandle;
		}

		using FactoryResult = iconer::util::Expected<IoCompletionPort, net::ErrorCode>;
		[[nodiscard]] static FactoryResult Create() noexcept;
		[[nodiscard]] static FactoryResult Create(std::uint32_t concurrency_hint) noexcept;

		IoCompletionPort(IoCompletionPort&&) noexcept = default;
		IoCompletionPort& operator=(IoCompletionPort&&) noexcept = default;

	private:
		IoCompletionPort(void* handle) noexcept;

		IoCompletionPort(const IoCompletionPort&) = delete;
		IoCompletionPort& operator=(const IoCompletionPort&) = delete;

		void* myHandle;
	};
}
