module;
#include <new>
#include <type_traits>
#include <memory>
#include <utility>
#include <variant>

export module Iconer.Utility.IProperty;
import Iconer.Utility.TypeTraits;
import Iconer.Utility.InvokeTraits;
//import Iconer.Utility.Invoker;

export namespace iconer::util::detail
{
	template<notvoids T, typename Context, typename Traits = InvokeTraits<T, void, Context>>
	class IProperty;

	template<notvoids T, typename Context, typename Traits>
	class IProperty
	{
	public:
		using value_type = Traits::value_type;
		using reference_type = Traits::reference_type;
		using const_reference_type = Traits::const_reference_type;
		using context_type = Traits::context_type;
		using context_pointer = Traits::context_pointer;
		using const_context_pointer = Traits::const_context_pointer;

		friend Context;

	protected:
		using invoke_met_t = Traits::invoke_met_t;
		using invoke_ref_met_t = Traits::invoke_ref_met_t;
		using invoke_cref_met_t = Traits::invoke_cref_met_t;
		using invoke_const_met_t = Traits::invoke_const_met_t;
		using invoke_const_cref_met_t = Traits::invoke_const_cref_met_t;

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
			: myContext(owner)
			, myDelegates(method)
			, myValue(std::forward<U>(init))
		{}

		template<invocable_immutable_methods<context_type, void, value_type> Method, typename U>
			requires (constructible_from<value_type, U>)
		constexpr IProperty(const_context_pointer owner, const Method& method, U&& init)
			: myValue(std::forward<U>(init))
			, myContext(owner)
			, myDelegates(method)
		{}

	protected:
		context_pointer myContext;
		storage_t myDelegates;
		value_type myValue;

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

	template<notvoids T, typename Traits>
	class IProperty<T, void, Traits>
	{
	public:
		using value_type = Traits::value_type;
		using reference_type = Traits::reference_type;
		using const_reference_type = Traits::const_reference_type;

	protected:
		using invoke_fun_t = Traits::invoke_fun_t;
		using invoke_ref_fun_t = Traits::invoke_ref_fun_t;
		using invoke_cref_fun_t = Traits::invoke_cref_fun_t;
		//using invoker_t = iconer::util::IInvoker<value_type>;

		using storage_t = std::variant<std::monostate,
			invoke_fun_t,
			invoke_ref_fun_t, invoke_cref_fun_t>;

		struct Visitor
		{
			constexpr void operator()(std::monostate) const noexcept {}

			constexpr void operator()(const auto& fun) const
			{
				fun(value);
			}

			//constexpr void operator()(const std::unique_ptr<invoker_t>& fun) const
			//{
			//	(*fun.get())(value);
			//}

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
