module;
#include <new>
#include <type_traits>
#include <memory>
#include <utility>
#include <variant>

export module Iconer.Utility.IProperty;
import Iconer.Utility.TypeTraits;

namespace iconer::util::detail
{
	template<typename Delegate>
	struct DelegateArgHelper;

	template<typename R, typename Arg0, typename... Rests>
	struct DelegateArgHelper<function_t<R, Arg0, Rests...>>
	{
		using type = Arg0;
	};

	template<typename R, typename Arg0, typename... Rests>
	struct DelegateArgHelper<nothrow_function_t<R, Arg0, Rests...>>
	{
		using type = Arg0;
	};

	template<typename T>
	struct IInvoker
	{
		using value_type = T;

		constexpr IInvoker() noexcept = default;
		constexpr virtual ~IInvoker() noexcept = default;

		constexpr virtual void operator()(const T& value) = 0;
	};

	template<typename Delegate>
	struct ILambdaInvoker : public Delegate
	{
		template<typename T>
		constexpr void operator()(const T& value)
		{
			Delegate::operator()(value);
		}
	};

	template<typename Delegate>
	ILambdaInvoker(Delegate) -> ILambdaInvoker<Delegate>;

	struct Empty
	{
		template<typename T>
		constexpr void operator()(T&&) noexcept {}
	};

	enum class InvokeStrategie : char
	{
		basic = 0,
		fun, ref_fun,
		method, const_method,
		custom
	};

	template<typename Fn, typename Ctx, typename R, typename T>
	concept MutMethodAssignable = any_type_of<Fn
		, method_t<Ctx, R, T>
		, method_t<Ctx, R, std::add_lvalue_reference_t<T>>
		, method_t<Ctx, R, std::add_lvalue_reference_t<std::add_const_t<T>>>
		, nothrow_method_t<Ctx, R, T>
		, nothrow_method_t<Ctx, R, std::add_lvalue_reference_t<T>>
		, nothrow_method_t<Ctx, R, std::add_lvalue_reference_t<std::add_const_t<T>>>
		, const_method_t<Ctx, R, T>
		, const_method_t<Ctx, R, std::add_lvalue_reference_t<std::add_const_t<T>>>
		, nothrow_const_method_t<Ctx, R, T>
		, nothrow_const_method_t<Ctx, R, std::add_lvalue_reference_t<std::add_const_t<T>>>>;

	template<typename Fn, typename Ctx, typename R, typename T>
	concept ImmutMethodAssignable = any_type_of<Fn
		, method_t<Ctx, R, T>
		, method_t<Ctx, R, std::add_lvalue_reference_t<std::add_const_t<T>>>
		, const_method_t<Ctx, R, T>
		, const_method_t<Ctx, R, std::add_lvalue_reference_t<std::add_const_t<T>>>>;
}

export namespace iconer::util
{
	template<notvoids T, typename Context>
	class IProperty;

	template<notvoids T>
	class IProperty<T, void>
	{
	public:
		using value_type = T;
		using reference_type = std::add_lvalue_reference_t<value_type>;
		using const_reference_type = std::add_lvalue_reference_t<std::add_const_t<value_type>>;
		using context_type = void;

	protected:
		using invoke_fun_t = function_t<void, value_type>;
		using invoke_cref_fun_t = function_t<void, const_reference_type>;
		using invoke_nt_fun_t = nothrow_function_t<void, value_type>;
		using invoke_nt_cref_fun_t = nothrow_function_t<void, const_reference_type>;
		using invoker_t = iconer::util::detail::IInvoker<value_type>;

		using storage_t = std::variant<std::monostate,
			invoke_fun_t, invoke_nt_fun_t,
			invoke_cref_fun_t, invoke_nt_cref_fun_t,
			std::unique_ptr<invoker_t>>;

		struct Visitor
		{
			constexpr void operator()(std::monostate) const noexcept {}

			constexpr void operator()(const auto& fun) const
			{
				fun(value);
			}

			constexpr void operator()(const std::unique_ptr<invoker_t>& fun) const
			{
				(*fun.get())(value);
			}

			value_type& value;
		};

		static inline constexpr bool defaultInitializable = default_initializable<value_type>;

	public:
		constexpr IProperty()
			noexcept(nothrow_default_constructibles<value_type, storage_t>)
			requires (defaultInitializable)
		: myValue(), myDelegates()
		{}

		constexpr ~IProperty()
			noexcept(nothrow_destructibles<value_type, storage_t>) = default;

		explicit constexpr IProperty(const invoke_fun_t& native_fn) requires (defaultInitializable)
			: IProperty(value_type{}, native_fn)
		{}

		explicit constexpr IProperty(const invoke_cref_fun_t& native_fn) requires (defaultInitializable)
			: IProperty(value_type{}, native_fn)
		{}

		template<typename U>
			requires (constructible_from<value_type, U>)
		constexpr IProperty(U&& init)
			: myValue(std::forward<U>(init))
			, myDelegates(std::monostate{})
		{}

		template<typename U>
			requires (constructible_from<value_type, U>)
		explicit constexpr IProperty(U&& init, const invoke_fun_t& native_fn)
			: myValue(std::forward<U>(init))
			, myDelegates(native_fn)
		{}

		template<typename U>
			requires (constructible_from<value_type, U>)
		explicit constexpr IProperty(U&& init, const invoke_cref_fun_t& native_fn)
			: myValue(std::forward<U>(init))
			, myDelegates(native_fn)
		{}

	protected:
		value_type myValue;
		storage_t myDelegates;

		constexpr void SetValue(const value_type& value) requires (copy_assignables<value_type>)
		{
			myValue = value;

			std::visit(Visitor{ myValue }, myDelegates);
		}

		constexpr void SetValue(value_type&& value) requires (move_assignables<value_type>)
		{
			myValue = std::move(value);

			std::visit(Visitor{ myValue }, myDelegates);
		}

		constexpr value_type& GetValue() & noexcept
		{
			return myValue;
		}

		constexpr value_type const& GetValue() const& noexcept
		{
			return myValue;
		}

		constexpr value_type&& GetValue() && noexcept
		{
			return std::move(myValue);
		}

		constexpr value_type const&& GetValue() const&& noexcept
		{
			return std::move(myValue);
		}
	};

	template<notvoids T, classes Context>
	class IProperty<T, Context>
	{
	public:
		using value_type = T;
		using reference_type = std::add_lvalue_reference_t<value_type>;
		using const_reference_type = std::add_lvalue_reference_t<std::add_const_t<value_type>>;
		using context_type = Context;
		using context_pointer = std::add_pointer_t<context_type>;
		using const_context_pointer = std::add_pointer_t<std::add_const_t<context_type>>;

	protected:
		using invoke_met_t = method_t<context_type, void, value_type>;
		using invoke_ref_met_t = method_t<context_type, void, reference_type>;
		using invoke_cref_met_t = method_t<context_type, void, const_reference_type>;
		using invoke_const_met_t = const_method_t<context_type, void, value_type>;
		using invoke_const_cref_met_t = const_method_t<context_type, void, const_reference_type>;

		using storage_t = std::variant<std::monostate,
			invoke_met_t, invoke_ref_met_t, invoke_cref_met_t,
			invoke_const_met_t, invoke_const_cref_met_t>;

		struct Visitor
		{
			constexpr void operator()(std::monostate) const noexcept {}

			constexpr void operator()(const auto& fun) const
			{
				((*context).*fun)(value);
			}

			Context* context;
			value_type& value;
		};

		static inline constexpr bool defaultInitializable = default_initializable<value_type>;

	public:
		constexpr ~IProperty()
			noexcept(nothrow_destructibles<value_type, storage_t>) = default;

		template<detail::MutMethodAssignable<context_type, void, value_type> Method>
		constexpr IProperty(context_pointer ctx, const Method& method) requires (defaultInitializable)
			: IProperty(ctx, method, value_type{})
		{}

		template<detail::ImmutMethodAssignable<context_type, void, value_type> Method>
		constexpr IProperty(const_context_pointer ctx, const Method& method) requires (defaultInitializable)
			: IProperty(ctx, method, value_type{})
		{}

		template<detail::MutMethodAssignable<context_type, void, value_type> Method, typename U>
			requires (constructible_from<value_type, U>)
		constexpr IProperty(context_pointer owner, const Method& method, U&& init)
			: myValue(std::forward<U>(init))
			, myContext(owner)
			, myDelegates(method)
		{}

		template<detail::ImmutMethodAssignable<context_type, void, value_type> Method, typename U>
			requires (constructible_from<value_type, U>)
		constexpr IProperty(const_context_pointer owner, const Method& method, U&& init)
			: myValue(std::forward<U>(init))
			, myContext(owner)
			, myDelegates(method)
		{}

	protected:
		value_type myValue;
		context_pointer myContext;
		storage_t myDelegates;

		constexpr void SetValue(const value_type& value) requires (copy_assignables<value_type>)
		{
			myValue = value;

			std::visit(Visitor{ myValue }, myDelegates);
		}

		constexpr void SetValue(value_type&& value) requires (move_assignables<value_type>)
		{
			myValue = std::move(value);

			std::visit(Visitor{ myContext, myValue }, myDelegates);
		}

		constexpr value_type& GetValue() & noexcept
		{
			return myValue;
		}

		constexpr value_type const& GetValue() const& noexcept
		{
			return myValue;
		}

		constexpr value_type&& GetValue() && noexcept
		{
			return std::move(myValue);
		}

		constexpr value_type const&& GetValue() const&& noexcept
		{
			return std::move(myValue);
		}
	};

	template<typename L, typename R, typename CL, typename CR>
		requires std::equality_comparable_with<L, R>
	[[nodiscard]]
	constexpr bool operator==(const IProperty<L, CL>& lhs, const IProperty<R, CR>& rhs) noexcept
	{
		return lhs.myValue == rhs.myValue;
	}

	template<typename L, typename CL, typename U>
		requires std::equality_comparable_with<L, U>
	[[nodiscard]]
	constexpr bool operator==(const IProperty<L, CL>& lhs, const U& rhs) noexcept
	{
		return lhs.myValue == rhs;
	}
}
