module;
#include <type_traits>
#include <memory>
#include <utility>
#include <tuple>
#include <variant>

export module Iconer.Utility.Invoker;
import Iconer.Utility.TypeTraits;
import Iconer.Utility.InvokeTraits;

namespace
{
	template<typename T, typename Tuple, size_t... Indices>
	constexpr
		auto
		ZipFrontImpl(T&& value, Tuple& pack, std::index_sequence<Indices...>)
	{
		return std::tuple{ std::forward<T>(value), std::move(std::get<Indices>(pack))... };
	}

	template<typename T, typename... Ts>
	constexpr std::tuple<T, Ts...> ZipFront(T&& value, std::tuple<Ts...>& tuple)
	{
		return ZipFrontImpl(std::forward<T>(value), tuple, std::index_sequence_for<Ts...>{});
	}

	template <typename Callable, typename Method, typename Tuple, size_t... Indices>
	constexpr decltype(auto) ApplyImpl(Callable&& context, const Method& method, Tuple&& params, std::index_sequence<Indices...>)
	{
		return ((context).*method)(std::get<Indices>(std::forward<Tuple>(params))...);
	}

	template <typename Context, typename Method, typename Tuple>
	constexpr decltype(auto) Apply(Context&& context, const Method& method, Tuple&& params)
	{
		return ApplyImpl(std::forward<Context>(context)
			, method
			, std::forward<Tuple>(params)
			, std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
	}
}

export namespace iconer::util
{
	template<typename R = void>
	class IInvoker
	{
	public:
		constexpr virtual ~IInvoker() noexcept = default;

		template<typename... Args>
			requires (not same_as<Args, void*> && ...)
		constexpr R Invoke(Args&&... args) const
		{
			if constexpr (0 < sizeof...(Args))
			{
				std::tuple<clean_t<Args>...> parameters = std::make_tuple(args...);

				return (*this)(static_cast<void*>(std::addressof(parameters)));
			}
			else
			{
				return (*this)(nullptr);
			}
		}

	protected:
		constexpr IInvoker() noexcept = default;

		constexpr virtual R operator()(void*) const = 0;
	};

	template<typename R = void, typename... Args>
	class NativeInvoker : public IInvoker<R>
	{
	public:
		using super = IInvoker<R>;
		using this_class = NativeInvoker<Args...>;
		using invoke_fun_t = R(*)(Args...);

		constexpr NativeInvoker() noexcept = default;

		constexpr NativeInvoker(const invoke_fun_t& fun) noexcept
			: super()
			, myFun(fun)
		{}

		using super::operator();

		[[nodiscard]]
		friend constexpr bool operator==(const this_class& lhs, const this_class& rhs) noexcept
		{
			return lhs.myFun == rhs.myFun;
		}

	private:
		constexpr R operator()(void* data) const override
		{
			if constexpr (0 < sizeof...(Args))
			{
				return std::apply(myFun, *static_cast<std::tuple<Args...>*>(data));
			}
			else // no parameter
			{
				return myFun();
			}
		}

		invoke_fun_t myFun{ nullptr };
	};

	template<classes Context, typename R = void, typename... Args>
	class MethodInvoker : public IInvoker<R>
	{
	public:
		using super = IInvoker<R>;
		using this_class = MethodInvoker<Context, R, Args...>;
		using context_type = std::add_pointer_t<Context>;
		using invoke_fun_t = method_t<Context, R, Args...>;
		using invoke_const_fun_t = const_method_t<Context, R, Args...>;

		constexpr MethodInvoker(context_type context, const invoke_fun_t& fun) noexcept
			: super()
			, myContext(context)
			, myFuns(fun)
		{}

		constexpr MethodInvoker(context_type context, const invoke_const_fun_t& fun) noexcept
			: super()
			, myContext(context)
			, myFuns(fun)
		{}

		using super::operator();

		template<typename D>
		[[nodiscard]]
		constexpr bool IsBoundTo(const this_class& lhs, const D* context) const noexcept
		{
			return context == lhs.myContext;
		}

		[[nodiscard]]
		friend constexpr bool operator==(const this_class& lhs, const this_class& rhs) noexcept
		{
			return lhs.myContext == rhs.myContext;
		}

	private:
		constexpr R operator()(void* data) const override
		{
			if constexpr (0 < sizeof...(Args))
			{
				const std::tuple<Args...>& tuple = *static_cast<std::tuple<Args...>*>(data);

				std::visit([&](const auto& fun) -> R
					{
						return Apply(*myContext, fun, tuple);
					}
				, myFuns);
			}
			else // no parameter
			{
				std::visit([&](const auto& fun) -> R
					{
						return ((*myContext).*fun)();
					}
				, myFuns);
			}
		}

		context_type myContext{ nullptr };
		std::variant<invoke_fun_t, invoke_const_fun_t> myFuns{ nullptr };
	};

	template<classes Context, typename R = void, typename... Args>
	class ImmutableMethodInvoker : public IInvoker<R>
	{
	public:
		using super = IInvoker<R>;
		using this_class = ImmutableMethodInvoker<Context, R, Args...>;
		using context_type = std::add_pointer_t<std::add_const_t<Context>>;
		using invoke_fun_t = const_method_t<Context, R, Args...>;

		constexpr ImmutableMethodInvoker(std::add_pointer_t<Context> context, const invoke_fun_t& fun) noexcept
			: super()
			, myContext(context)
			, myFun(fun)
		{}

		constexpr ImmutableMethodInvoker(std::add_pointer_t<std::add_const_t<Context>> context, const invoke_fun_t& fun) noexcept
			: super()
			, myContext(context)
			, myFun(fun)
		{}

		using super::operator();

		template<typename D>
		[[nodiscard]]
		constexpr bool IsBoundTo(const this_class& lhs, const D* context) const noexcept
		{
			return context == lhs.myContext;
		}

		[[nodiscard]]
		friend constexpr bool operator==(const this_class& lhs, const this_class& rhs) noexcept
		{
			return lhs.myContext == rhs.myContext and lhs.myFun == rhs.myFun;
		}

	private:
		constexpr R operator()(void* data) const override
		{
			if constexpr (0 < sizeof...(Args))
			{
				const std::tuple<Args...>& tuple = *static_cast<std::tuple<Args...>*>(data);

				return Apply(*myContext, myFun, tuple);
			}
			else // no parameter
			{
				return ((*myContext).*myFun)();
			}
		}

		context_type myContext{ nullptr };
		invoke_fun_t myFun{ nullptr };
	};

	template<typename Lambda>
	struct LambdaTraits : LambdaTraits<decltype(&Lambda::operator())>
	{};

	template<typename Lambda, typename R, typename ...Args>
	struct LambdaTraits<const_method_t<Lambda, R, Args...>>
	{
		using type = const_method_t<Lambda, R, Args...>;
	};

	template <typename Lambda>
	struct lambda_traits : lambda_traits<decltype(&Lambda::operator())>
	{};

	template <typename Lambda, typename Return, typename... Args>
	struct lambda_traits<Return(Lambda::*)(Args...)> : lambda_traits<Return(Lambda::*)(Args...) const>
	{};

	template <typename Lambda, typename Return, typename... Args>
	struct lambda_traits<Return(Lambda::*)(Args...) const>
	{
		using type = Lambda;
		using pointer = std::add_pointer_t<Return(Args...)>;

		template <typename Fn>
		[[nodiscard]]
		static pointer to_pointer(Fn&& lambda)
		{
			static Lambda static_lambda = Lambda{ std::forward<Fn>(lambda) };
			return [](Args&&... args)
				{
					return static_lambda(std::forward<Args>(args)...);
				};
		}
	};

	template <typename Lambda>
	[[nodiscard]]
	auto to_pointer(Lambda&& lambda)
	{
		return lambda_traits<clean_t<Lambda>>::to_pointer(std::forward<Lambda>(lambda));
	}

	template<typename R, typename ...Args>
	class LambdaInvoker : public IInvoker<R>
	{
	public:
		using super = IInvoker<R>;
		using this_class = LambdaInvoker<R, Args...>;
		using invoke_fun_t = std::add_pointer_t<R(Args...)>;

		template<invocable_r<R, Args...> Lambda>
		LambdaInvoker(const Lambda& lambda) noexcept
			: super()
			, myFun(to_pointer(lambda))
		{}

		using super::operator();

	private:
		constexpr virtual R operator()(void* data) const override
		{
			if constexpr (same_as<R, void>)
			{
				std::apply(myFun, *static_cast<std::tuple<Args...>*>(data));
			}
			else
			{
				return std::apply(myFun, *static_cast<std::tuple<Args...>*>(data));
			}
		}

		invoke_fun_t myFun;
	};

	template<typename R = void, typename... Args>
	NativeInvoker(function_t<R, Args...>) -> NativeInvoker<R, Args...>;

	template<classes Context, typename R = void, typename... Args>
	MethodInvoker(Context*, method_t<Context, R, Args...>) -> MethodInvoker<Context, R, Args...>;

	template<classes Context, typename R = void, typename... Args>
	ImmutableMethodInvoker(Context*, const_method_t<Context, R, Args...>) -> ImmutableMethodInvoker<Context, R, Args...>;

	template<classes Context, typename R = void, typename... Args>
	ImmutableMethodInvoker(const Context*, const_method_t<Context, R, Args...>) -> ImmutableMethodInvoker<Context, R, Args...>;

	template<typename R, typename ...Args>
	[[nodiscard]]
	constexpr IInvoker<R>*
		MakeSharedInvoker(const function_t<R, Args...>& fun)
	{
		return new NativeInvoker{ fun };
	}

	template<classes Context, typename R, typename... Args>
	[[nodiscard]]
	constexpr IInvoker<R>*
		MakeSharedInvoker(Context* ctx, const method_t<Context, R, Args...>& method)
	{
		return new MethodInvoker{ ctx, method };
	}

	template<classes Context, typename R, typename... Args>
	[[nodiscard]]
	constexpr IInvoker<R>*
		MakeSharedInvoker(Context* ctx, const const_method_t<Context, R, Args...>& method)
	{
		return new ImmutableMethodInvoker{ ctx, method };
	}

	template<classes Context, typename R, typename... Args>
	[[nodiscard]]
	constexpr IInvoker<R>*
		MakeSharedInvoker(const Context* ctx, const const_method_t<Context, R, Args...>& method)
	{
		return new ImmutableMethodInvoker{ ctx, method };
	}

	template<typename R, typename ...Args>
	[[nodiscard]]
	constexpr std::unique_ptr<IInvoker<R>>
		MakeInvoker(const function_t<R, Args...>& fun)
	{
		return std::unique_ptr<IInvoker<R>>(new NativeInvoker{ fun });
	}

	template<classes Context, typename R, typename... Args>
	[[nodiscard]]
	constexpr std::unique_ptr<IInvoker<R>>
		MakeInvoker(Context* ctx, const method_t<Context, R, Args...>& method)
	{
		return std::unique_ptr<IInvoker<R>>(new MethodInvoker{ ctx, method });
	}

	template<classes Context, typename R, typename... Args>
	[[nodiscard]]
	constexpr std::unique_ptr<IInvoker<R>>
		MakeInvoker(Context* ctx, const const_method_t<Context, R, Args...>& method)
	{
		return std::unique_ptr<IInvoker<R>>(new ImmutableMethodInvoker{ ctx, method });
	}

	template<classes Context, typename R, typename... Args>
	[[nodiscard]]
	constexpr std::unique_ptr<IInvoker<R>>
		MakeInvoker(const Context* ctx, const const_method_t<Context, R, Args...>& method)
	{
		return std::unique_ptr<IInvoker<R>>(new ImmutableMethodInvoker{ ctx, method });
	}

	template<typename ...Args>
	[[nodiscard]]
	constexpr auto MakeLambdaInvoker(auto&& lambda)
	{
		using lambda_t = decltype(lambda);
		using r = std::invoke_result_t<lambda_t, Args...>;

		return LambdaInvoker<r, Args...>{ std::forward<lambda_t>(lambda) };
	}
}

module:private;

namespace iconer::test
{
	using namespace iconer::util;

	void aAAA1(int) {}
	void aAAA2(int&) {}
	void aAAA3(const int&) {}
	void aAAA4(double) {}

	class AAA2
	{
	public:
		void AAA_m1(double) {}
		void AAA_m2(double) const {}

		void aAAA(int)
		{
			int val1{};
			constexpr int val2{};
			constexpr NativeInvoker<void, int&> invoker{};
			constexpr NativeInvoker<void, int&> invoker2{ aAAA2 };
			constexpr auto invoker3 = NativeInvoker<void, int>{ aAAA1 };
			constexpr auto invoker4 = NativeInvoker{ aAAA1 };
			const MethodInvoker<AAA2, void, double> invoker5{ this, &AAA2::AAA_m1 };
			const MethodInvoker<AAA2, void, double> invoker6{ this, &AAA2::AAA_m2 };
			const ImmutableMethodInvoker<AAA2, void, double> invoker7{ this, &AAA2::AAA_m2 };
			const ImmutableMethodInvoker<AAA2, void, double> invoker8{ this, &AAA2::AAA_m2 };

			invoker.Invoke(4);
			invoker.Invoke(val1);
			invoker.Invoke(val2);
			//invoker.
			//invoker(std::move(val1));

			const auto lambda_invoker1 = LambdaInvoker<void>{ []() {} };
			//const auto lambda_invoker2 = LambdaInvoker<void, int>{ [](int) {} };
			const auto lambda_invoker3 = LambdaInvoker<int>{ []() -> int { return 0; } };
			//const auto lambda_invoker4 = LambdaInvoker<int, int>{ [](int) -> int { return 0; } };

			const auto lambda1 = [this]() {};
			using lambda1_t = decltype(lambda1);

			const auto lambda_invoker7 = LambdaInvoker<void>{ lambda1 };
			//lambda_invoker7.operator()(4);
		}
	};
}