export module Iconer.Coroutine.Task;
export import Iconer.Coroutine;
import <stdexcept>;
import <utility>;
import <coroutine>;

namespace iconer::coroutine
{
	void __ExecuteCoroutine(std::coroutine_handle<void> handle);
	void __ExecuteCoroutineAsync(std::coroutine_handle<void> handle);
}

export namespace iconer::coroutine
{
	template<typename T = void>
	class Task;

	template<typename T>
	class [[nodiscard]] Task final
	{
	public:
		struct promise_type;
		using handle_type = std::coroutine_handle<promise_type>;

		struct promise_type
		{
			promise_type() noexcept = default;
			~promise_type() noexcept = default;

			[[nodiscard]]
			Task<T> get_return_object() noexcept
			{
				return Task<T>(handle_type::from_promise(*this));
			}

			template<typename U>
			void return_value(U&& value)
			{
				myValue = std::forward<U>(value);
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
			static void unhandled_exception()
			{
				throw;
			}

			T myValue;
		};

		constexpr Task(const handle_type& handle) noexcept
			: myHandle(handle)
		{
		}

		constexpr Task(handle_type&& handle) noexcept
			: myHandle(std::move(handle))
		{
		}

		~Task() noexcept(noexcept(std::declval<handle_type>().destroy()))
		{
			if (myHandle)
			{
				myHandle.destroy();
			}
		}

		void Start() const
		{
			__ExecuteCoroutine(myHandle);
		}

		void StartAsync() const
		{
			__ExecuteCoroutineAsync(myHandle);
		}

		T operator()()
		{
			__ExecuteCoroutine(myHandle);

			return std::move(myHandle.promise().myValue);
		}

		auto operator co_await()
		{
			struct awaiter
			{
				static constexpr bool await_ready() noexcept { return true; }
				static constexpr void await_suspend(std::coroutine_handle<void>) noexcept
				{
				}

				T await_resume()
				{
					coHandle();
					return std::move(coHandle.promise().myValue);
				}

				handle_type coHandle;
			};

			return awaiter{ .coHandle = myHandle };
		}

		[[nodiscard]]
		bool IsDone() const noexcept
		{
			return myHandle.done();
		}

		[[nodiscard]]
		constexpr bool operator==(const Task& other) const noexcept
		{
			return myHandle.address() == other.myHandle.address();
		}

	private:
		const static inline std::runtime_error reservedError{ "Cannot acquire a value from the null promise" };

		handle_type myHandle;
	};

	template<>
	class [[nodiscard]] Task<void> final
	{
	public:
		struct promise_type;
		using handle_type = std::coroutine_handle<promise_type>;

		struct promise_type
		{
			promise_type() noexcept = default;
			~promise_type() noexcept = default;

			[[nodiscard]]
			Task<void> get_return_object() noexcept;

			static constexpr void return_void() noexcept {}

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

		struct Awaiter
		{
			static constexpr bool await_ready() noexcept { return true; }
			static constexpr void await_suspend(std::coroutine_handle<void>) noexcept
			{
			}

			void await_resume() const
			{
				coHandle();
			}

			handle_type coHandle;
		};

		constexpr Task(const handle_type& handle) noexcept
			: myHandle(handle)
		{
		}

		constexpr Task(handle_type&& handle) noexcept
			: myHandle(std::move(handle))
		{
		}

		constexpr Task(Task&& other) noexcept
			: Task(std::exchange(other.myHandle, nullptr))
		{
		}
		
		constexpr Task& operator=(Task&& other) noexcept
		{
			myHandle = std::exchange(other.myHandle, nullptr);
			return *this;
		}

		~Task() noexcept;

		void Start() const;
		void StartAsync() const;

		void operator()() const;
		Awaiter operator co_await() const noexcept;

		[[nodiscard]]
		bool IsDone() const noexcept;

		[[nodiscard]]
		bool operator==(const Task& other) const noexcept;

	private:
		const static inline std::runtime_error reservedError{ "Cannot acquire a value from the null promise" };

		Task(const Task& other) = delete;
		Task& operator=(const Task& other) = delete;

		handle_type myHandle;
	};
}
