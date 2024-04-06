module;
#include <memory>

export module Iconer.Application.SendContextPool;
import Iconer.Application.IContext;
import Iconer.Application.BorrowedSendContext;

export namespace iconer::app
{
	class [[nodiscard]] SendContextPool final
	{
	public:
		using allocator_type = std::allocator<BorrowedSendContext>;
		using allocator_traits = std::allocator_traits<allocator_type>;
		using value_type = allocator_traits::value_type;
		using pointer = allocator_traits::pointer;
		using const_pointer = allocator_traits::const_pointer;

		static inline constexpr size_t initSendContextsNumber = 100000;
		static inline constexpr size_t maxSendContextsNumber = 200000;

		static void Awake();
		static void Add(Borrower&& context);
		static [[nodiscard]] Borrower Pop();
		static [[nodiscard]] bool TryPop(Borrower& out);

		[[nodiscard]]
		static constexpr SendContextPool& Instance() noexcept
		{
			return instance;
		}

	private:
		static constinit SendContextPool instance;

		constexpr SendContextPool() noexcept = default;
		constexpr ~SendContextPool() noexcept = default;
	};
}

constinit iconer::app::SendContextPool iconer::app::SendContextPool::instance{};
