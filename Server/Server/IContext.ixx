export module Iconer.Application.IContext;
import Iconer.Net.IoContext;
export import Iconer.Application.AsyncOperation;

export namespace iconer::app
{
	class IContext : public iconer::net::IoContext
	{
	public:
		using Super = iconer::net::IoContext;

		explicit constexpr IContext() noexcept = default;
		~IContext() noexcept = default;

		explicit constexpr IContext(const AsyncOperations& op) noexcept
			: Super(), lastOperation(op)
		{
		}

		explicit constexpr IContext(AsyncOperations&& op) noexcept
			: Super(), lastOperation(static_cast<AsyncOperations&&>(op))
		{
		}

		constexpr void SetOperation(const AsyncOperations& op) noexcept
		{
			lastOperation = op;
		}

		constexpr void SetOperation(AsyncOperations&& op) noexcept
		{
			lastOperation = static_cast<AsyncOperations&&>(op);
		}

		constexpr void SetOperation(const AsyncOperations& op) volatile noexcept
		{
			lastOperation = op;
		}

		constexpr void SetOperation(AsyncOperations&& op) volatile noexcept
		{
			lastOperation = static_cast<AsyncOperations&&>(op);
		}

		[[nodiscard]]
		constexpr const AsyncOperations& GetOperation() const& noexcept
		{
			return lastOperation;
		}

		[[nodiscard]]
		constexpr AsyncOperations&& GetOperation() && noexcept
		{
			return static_cast<AsyncOperations&&>(lastOperation);
		}

		[[nodiscard]]
		constexpr const volatile AsyncOperations& GetOperation() const volatile& noexcept
		{
			return lastOperation;
		}

		[[nodiscard]]
		constexpr volatile AsyncOperations&& GetOperation() volatile&& noexcept
		{
			return static_cast<volatile AsyncOperations&&>(lastOperation);
		}

		IContext(IContext&&) noexcept = default;
		IContext& operator=(IContext&&) noexcept = default;

		AsyncOperations lastOperation;

	private:
		IContext(const IContext&) = delete;
		IContext& operator=(const IContext&) = delete;
	};
}
