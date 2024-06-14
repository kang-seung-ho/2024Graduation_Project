module;
#include <new>
#include <type_traits>
#include <memory>
#include <utility>
#include <variant>

export module Iconer.Utility.IProperty;
import Iconer.Utility.TypeTraits;

export namespace iconer::util::detail
{
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

	template<notvoids T, typename Context>
	class IProperty;

	template<notvoids T, typename Context>
	class IProperty
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

		template<invocable_mutable_methods<context_type, void, value_type> Method>
		constexpr IProperty(context_pointer ctx, const Method& method) requires (defaultInitializable)
			: IProperty(ctx, method, value_type{})
		{}

		template<invocable_immutable_methods<context_type, void, value_type> Method>
		constexpr IProperty(const_context_pointer ctx, const Method& method) requires (defaultInitializable)
			: IProperty(ctx, method, value_type{})
		{}

		template<invocable_mutable_methods<context_type, void, value_type> Method, typename U>
			requires (constructible_from<value_type, U>)
		constexpr IProperty(context_pointer owner, const Method& method, U&& init)
			: myValue(std::forward<U>(init))
			, myContext(owner)
			, myDelegates(method)
		{}

		template<invocable_immutable_methods<context_type, void, value_type> Method, typename U>
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

	template<typename L, typename R, typename CL, typename CR>
		requires std::totally_ordered_with<L, R>
	[[nodiscard]]
	constexpr auto operator<=>(const IProperty<L, CL>& lhs, const IProperty<R, CR>& rhs)
		noexcept(noexcept(std::declval<const L&>() <=> std::declval<const R&>()))
	{
		return static_cast<const L&>(lhs) <=> static_cast<const R&>(rhs);
	}

	template<typename L, typename CL, typename U>
		requires std::totally_ordered_with<L, U>
	[[nodiscard]]
	constexpr auto operator<=>(const IProperty<L, CL>& lhs, const U& rhs)
		noexcept(noexcept(std::declval<const L&>() <=> std::declval<const U&>()))
		-> decltype(std::declval<const L&>() <=> std::declval<const U&>())
	{
		return static_cast<const L&>(lhs) <=> rhs;
	}
}
