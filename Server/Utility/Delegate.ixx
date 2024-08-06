module;
#include <type_traits>
#include <concepts>
#include <memory>
#include <vector>
#include <set>
#include <algorithm>

export module Iconer.Utility.Delegate;
import Iconer.Utility.TypeTraits;
import Iconer.Utility.InvokeTraits;
export import Iconer.Utility.Invoker;

export namespace iconer::util
{
	template<typename R, typename... Args>
	class Delegate
	{
	public:
		using this_class = Delegate<R, Args...>;

		static inline constexpr size_t argumentNumber = sizeof...(Args);

		constexpr Delegate() noexcept = default;
		constexpr ~Delegate() noexcept = default;

		constexpr void Add(IInvoker<R>* invoker)
		{
			invocationList.emplace_back(invoker);
		}

		constexpr void Add(std::unique_ptr<IInvoker<R>>&& handle)
		{
			invocationList.push_back(std::move(handle));
		}

		constexpr void Clear() noexcept
		{
			invocationList.clear();
		}

		constexpr R Broadcast(Args... args)
		{
			for (std::unique_ptr<IInvoker<R>>& handle : invocationList)
			{
				IInvoker<R>& invoker = *handle.get();

				if constexpr (std::same_as<void, R>)
				{
					invoker.Invoke(args...);
				}
				else
				{
					return invoker.Invoke(args...);
				}
			}
		}

		constexpr void Unbound(const function_t<R, Args...>& fun)
		{
			auto it = invocationList.begin();
			for (; it != invocationList.end(); ++it)
			{
				std::unique_ptr<IInvoker<R>>& invoker = *it;
				NativeInvoker<R, Args...>* native = dynamic_cast<NativeInvoker<R, Args...>*>(invoker.get());

				if (nullptr != native and *native == fun)
				{
					it = std::remove(it, invocationList.end(), invoker);
				}
			}

			invocationList.erase(it, invocationList.end());
		}

		template<classes Context>
		constexpr void Unbound(const Context* context)
		{
			auto it = invocationList.begin();
			for (; it != invocationList.end(); ++it)
			{
				std::unique_ptr<IInvoker<R>>& invoker = *it;
				MethodInvoker<Context, R, Args...>* method = dynamic_cast<MethodInvoker<Context, R, Args...>*>(invoker.get());

				if (nullptr != method and method->IsBoundTo(context))
				{
					it = std::remove(it, invocationList.end(), invoker);
				}
			}

			invocationList.erase(it, invocationList.end());
		}

		[[nodiscard]]
		constexpr bool IsBound() const noexcept
		{
			return not invocationList.empty();
		}

	protected:
		std::vector<std::unique_ptr<IInvoker<R>>> invocationList;
	};

	template<typename R, typename... Args>
	class SharedDelegate
	{
	protected:
		using data_type = std::vector<IInvoker<R>*>;
		data_type invocationList;

	public:
		using this_class = SharedDelegate<R, Args...>;
		using iterator = data_type::iterator;
		using const_iterator = data_type::const_iterator;

		constexpr SharedDelegate() noexcept = default;

		constexpr ~SharedDelegate() noexcept
		{
			//while (0 < invocationList.size())
			//{
				//iterator jit = invocationList.begin();
				//IInvoker<R>* ptr = *jit;

				//jit = std::remove(invocationList.begin(), invocationList.end(), ptr);
				//delete ptr;

				//invocationList.erase(jit, invocationList.end());
			//}

			invocationList.erase(invocationList.begin(), invocationList.end());
		}

		constexpr void Add(IInvoker<R>* invoker)
		{
			invocationList.emplace_back(invoker);
		}

		constexpr void Clear() noexcept
		{
			invocationList.clear();
		}

		constexpr R Broadcast(Args... args)
		{
			for (IInvoker<R>*& invoker : invocationList)
			{
				if constexpr (std::same_as<void, R>)
				{
					invoker->Invoke(args...);
				}
				else
				{
					return invoker->Invoke(args...);
				}
			}
		}

		constexpr void Unbound(const function_t<R, Args...>& fun)
		{
			auto it = invocationList.begin();
			for (; it != invocationList.end(); ++it)
			{
				IInvoker<R>* invoker = *it;
				NativeInvoker<R, Args...>* native = dynamic_cast<NativeInvoker<R, Args...>*>(invoker);

				if (nullptr != native and *native == fun)
				{
					it = std::remove(it, invocationList.end(), invoker);
				}
			}

			invocationList.erase(it, invocationList.end());
		}

		template<classes Context>
		constexpr void Unbound(const Context* context)
		{
			auto it = invocationList.begin();
			for (; it != invocationList.end(); ++it)
			{
				IInvoker<R>* invoker = *it;
				MethodInvoker<Context, R, Args...>* method = dynamic_cast<MethodInvoker<Context, R, Args...>*>(invoker);

				if (nullptr != method and method->IsBoundTo(context))
				{
					it = std::remove(it, invocationList.end(), invoker);
				}
			}

			invocationList.erase(it, invocationList.end());
		}

		[[nodiscard]]
		constexpr bool IsBound() const noexcept
		{
			return not invocationList.empty();
		}
	};
}
