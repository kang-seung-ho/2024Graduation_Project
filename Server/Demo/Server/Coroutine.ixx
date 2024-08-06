export module Iconer.Coroutine;
import Iconer.Utility.Handler;
import <utility>;
import <atomic>;
export import <coroutine>;

export namespace iconer::coroutine
{
	using std::suspend_never;
	using std::suspend_always;
	using std::coroutine_handle;

	class Coroutine
	{
	public:
		struct promise_type;
		using handle_type = std::coroutine_handle<promise_type>;

		struct promise_type
		{
			promise_type() noexcept = default;
			~promise_type() noexcept = default;

			[[nodiscard]]
			Coroutine get_return_object() noexcept
			{
				return Coroutine(handle_type::from_promise(*this));
			}

			static constexpr void return_void() noexcept
			{
			}

			static constexpr std::suspend_always initial_suspend() noexcept
			{
				return {};
			}

			static constexpr std::suspend_always final_suspend() noexcept
			{
				return {};
			}

			[[noreturn]]
			void unhandled_exception()
			{
				throw;
			}
		};

		explicit constexpr Coroutine(const handle_type& handle) noexcept
			: myHandle(handle)
			, isTriggered()
		{
		}

		explicit constexpr Coroutine(handle_type&& handle) noexcept
			: myHandle(std::move(handle))
			, isTriggered()
		{
		}

		explicit Coroutine(Coroutine&& other) noexcept
			: myHandle(std::exchange(other.myHandle, nullptr))
			, isTriggered()
		{
			isTriggered = other.IsTriggered();
		}

		Coroutine& operator=(Coroutine&& other) noexcept
		{
			myHandle = std::move(std::exchange(other.myHandle, nullptr));
			isTriggered = other.IsTriggered();
			return *this;
		}

		~Coroutine() noexcept
		{
			if (myHandle)
			{
				myHandle.destroy();
			}
		}

		void Start();
		void StartAsync();
		void Resume();
		void operator()();

		[[nodiscard]]
		constexpr handle_type GetHandle() const noexcept
		{
			return myHandle;
		}

		[[nodiscard]]
		bool IsTriggered() const noexcept
		{
			return isTriggered.load(std::memory_order_relaxed);
		}

		[[nodiscard]]
		bool operator==(const Coroutine&) const noexcept = default;

	protected:
		Coroutine(const Coroutine&) = delete;
		Coroutine& operator=(const Coroutine&) = delete;

		handle_type myHandle;
		std::atomic_bool isTriggered;
	};
}
