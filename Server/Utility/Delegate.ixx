module;
#include <type_traits>
#include <concepts>
#include <memory>
#include <vector>

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
}
