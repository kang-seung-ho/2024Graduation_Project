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

	template<typename T, typename Delegate>
	struct ILambdaInvokerSample : public Delegate, public IInvoker<T>
	{
		using value_type = IInvoker<T>::value_type;

		constexpr virtual void operator()(const T& value) override
		{
			Delegate::operator()(value);
		}
	};

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

	template<typename T, typename Delegate>
	struct ILambdaInvoker : public Delegate, public iconer::util::detail::IInvoker<T>
	{
		constexpr virtual void operator()(const T& value) override
		{
			Delegate::operator()(value);
		}
	};
}

export namespace iconer::util
{
	template<typename T, typename Context = void> class Property;

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

		template<typename Delegate>
		struct LambdaInvoker : public Delegate, public iconer::util::detail::IInvoker<value_type>
		{
			constexpr virtual void operator()(const value_type& value) override
			{
				Delegate::operator()(value);
			}
		};

		static inline constexpr bool defaultInitializable = default_initializable<value_type>;

	public:
		value_type myValue;
		std::variant<std::monostate,
			invoke_fun_t, invoke_nt_fun_t,
			invoke_cref_fun_t, invoke_nt_cref_fun_t,
			iconer::util::detail::IInvoker<value_type>> myDelegates;

		constexpr IProperty() noexcept requires (defaultInitializable) = default;
		constexpr ~IProperty() noexcept = default;

		constexpr IProperty(const invoke_fun_t& native_fn) requires (defaultInitializable)
			: IProperty(value_type{}, std::as_const(native_fn))
		{}

		constexpr IProperty(const invoke_nt_fun_t& native_fn) requires (defaultInitializable)
			: IProperty(value_type{}, std::as_const(native_fn))
		{}

		constexpr IProperty(const invoke_cref_fun_t& native_fn) requires (defaultInitializable)
			: IProperty(value_type{}, std::as_const(native_fn))
		{}

		constexpr IProperty(const invoke_nt_cref_fun_t& native_fn) requires (defaultInitializable)
			: IProperty(value_type{}, std::as_const(native_fn))
		{}

		template<invocable<value_type> Delegate>
		constexpr IProperty(Delegate&& delegate) requires (defaultInitializable)
			: IProperty(value_type{}, std::forward<Delegate>(delegate))
		{}

		template<typename U>
			requires (constructible_from<value_type, U>)
		constexpr IProperty(U&& init)
			: myValue(std::forward<U>(init))
			, myStrategie(iconer::util::detail::InvokeStrategie::basic)
		{}

		template<typename U>
			requires (constructible_from<value_type, U>)
		constexpr IProperty(U&& init, const invoke_fun_t& native_fn)
			: myValue(std::forward<U>(init))
			, myStrategie(iconer::util::detail::InvokeStrategie::fun)
			, myFun(native_fn)
		{}

		template<typename U>
			requires (constructible_from<value_type, U>)
		constexpr IProperty(U&& init, const invoke_nt_fun_t& native_fn)
			: myValue(std::forward<U>(init))
			, myStrategie(iconer::util::detail::InvokeStrategie::fun)
			, myFun(static_cast<invoke_fun_t>(native_fn))
		{}

		template<typename U>
			requires (constructible_from<value_type, U>)
		constexpr IProperty(U&& init, const invoke_cref_fun_t& native_fn)
			: myValue(std::forward<U>(init))
			, myStrategie(iconer::util::detail::InvokeStrategie::ref_fun)
			, myRefFun(native_fn)
		{}

		template<typename U>
			requires (constructible_from<value_type, U>)
		constexpr IProperty(U&& init, const invoke_nt_cref_fun_t& native_fn)
			: myValue(std::forward<U>(init))
			, myStrategie(iconer::util::detail::InvokeStrategie::ref_fun)
			, myRefFun(static_cast<invoke_cref_fun_t>(native_fn))
		{}

		template<typename U, invocable<value_type> Delegate>
			requires (constructible_from<value_type, U>)
		constexpr IProperty(U&& init, Delegate&& delegate)
			: myValue(std::forward<U>(init))
			, myStrategie(iconer::util::detail::InvokeStrategie::custom)
			, myDelegate(MakeInvoker(std::forward<Delegate>(delegate)))
		{}

	protected:
		/// <exception cref="std::bad_variant_access"/>
		constexpr void SetValue(const value_type& value) requires (copy_assignables<value_type>)
		{
			myValue = value;

			using enum iconer::util::detail::InvokeStrategie;
			switch (myStrategie)
			{
			case basic:
			{}
			break;

			// function
			case fun:
			{
				myFun(myValue);
			}
			break;

			// cref function
			case ref_fun:
			{
				myRefFun(myValue);
			}
			break;

			// customization point object
			case custom:
			{
				(*myDelegate)(myValue);
			}
			break;
			}
		}

		/// <exception cref="std::bad_variant_access"/>
		constexpr void SetValue(value_type&& value) requires (move_assignables<value_type>)
		{
			myValue = std::move(value);

			using enum iconer::util::detail::InvokeStrategie;
			switch (myStrategie)
			{
			case basic:
			{}
			break;

			// function
			case fun:
			{
				myFun(myValue);
			}
			break;

			// cref function
			case ref_fun:
			{
				myRefFun(myValue);
			}
			break;

			// customization point object
			case custom:
			{
				(*myDelegate)(myValue);
			}
			break;
			}
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

		invoke_fun_t myFun;
		invoke_cref_fun_t myRefFun;
		std::unique_ptr<iconer::util::detail::IInvoker<value_type>> myDelegate;
	};

	template<notvoids T, classes Context>
	class IProperty<T, Context>
	{
	public:
		using value_type = T;
		using reference_type = std::add_lvalue_reference_t<value_type>;
		using const_reference_type = std::add_lvalue_reference_t<std::add_const_t<value_type>>;
		using context_type = Context;
		using context_ptr_t = std::add_pointer_t<context_type>;
		using const_context_ptr_t = std::add_pointer_t<std::add_const_t<context_type>>;

	protected:
		using invoke_met_t = method_t<context_type, void, value_type>;
		using invoke_const_met_t = const_method_t<context_type, void, value_type>;

		using invoke_nt_met_t = nothrow_method_t<context_type, void, value_type>;
		using invoke_nt_const_met_t = nothrow_const_method_t<context_type, void, value_type>;

		static inline constexpr bool defaultInitializable = default_initializable<value_type>;

	public:
		value_type myValue;
		iconer::util::detail::InvokeStrategie myStrategie;

		constexpr IProperty() noexcept requires (defaultInitializable) = default;
		constexpr ~IProperty() noexcept = default;

		constexpr IProperty(context_ptr_t ctx, const invoke_met_t& method) requires (defaultInitializable)
			: IProperty(ctx, method, value_type{})
		{}

		constexpr IProperty(context_ptr_t ctx, const invoke_const_met_t& method) requires (defaultInitializable)
			: IProperty(ctx, method, value_type{})
		{}

		template<typename U>
			requires (constructible_from<value_type, U> and classes<Context>)
		constexpr IProperty(context_ptr_t owner, const invoke_met_t& method, U&& init) requires (classes<Context>)
			: myValue(std::forward<U>(init))
			, myStrategie(iconer::util::detail::InvokeStrategie::method)
			, myContext(owner)
			, myMethod(method)
		{}

		template<typename U>
		IProperty(const_context_ptr_t, const invoke_met_t&, U&&) requires (classes<Context>) = delete;

		template<typename U>
			requires (constructible_from<value_type, U> and classes<Context>)
		constexpr IProperty(context_ptr_t owner, const invoke_const_met_t& method, U&& init)
			: myValue(std::forward<U>(init))
			, myStrategie(iconer::util::detail::InvokeStrategie::const_method)
			, myContext(owner)
			, myConstMethod(method)
		{}

		template<typename U>
			requires (constructible_from<value_type, U> and classes<Context>)
		constexpr IProperty(const_context_ptr_t owner, const invoke_const_met_t& method, U&& init)
			: myValue(std::forward<U>(init))
			, myStrategie(iconer::util::detail::InvokeStrategie::const_method)
			, myContext(const_cast<context_ptr_t>(owner))
			, myConstMethod(method)
		{}

	protected:
		constexpr void SetValue(const value_type& value) requires (copy_assignables<value_type>)
		{
			myValue = value;

			using enum iconer::util::detail::InvokeStrategie;
			switch (myStrategie)
			{
			case basic:
			{}
			break;

			// function
			case fun:
			{
				myFun(myValue);
			}
			break;

			// cref function
			case ref_fun:
			{
				myRefFun(myValue);
			}
			break;

			// method
			case method:
			{
				((*myContext).*myMethod)(myValue);
			}
			break;

			// immutable method
			case const_method:
			{
				((*myContext).*myConstMethod)(myValue);
			}
			break;

			// customization point object
			case custom:
			{
				(*myDelegate)(myValue);
			}
			break;
			}
		}

		constexpr void SetValue(value_type&& value) requires (move_assignables<value_type>)
		{
			myValue = std::move(value);

			using enum iconer::util::detail::InvokeStrategie;
			switch (myStrategie)
			{
			case basic:
			{}
			break;

			// function
			case fun:
			{
				myFun(myValue);
			}
			break;

			// cref function
			case ref_fun:
			{
				myRefFun(myValue);
			}
			break;

			// method
			case method:
			{
				((*myContext).*myMethod)(myValue);
			}
			break;

			// immutable method
			case const_method:
			{
				((*myContext).*myConstMethod)(myValue);
			}
			break;

			// customization point object
			case custom:
			{
				(*myDelegate)(myValue);
			}
			break;
			}
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

		invoke_met_t myMethod;
		invoke_const_met_t myConstMethod;
		context_ptr_t myContext;
	};

	template<typename L, typename R, typename CL, typename CR>
	[[nodiscard]]
	constexpr bool operator==(const IProperty<L, CL>& lhs, const IProperty<R, CR>& rhs) noexcept
	{
		return lhs.myValue == rhs.myValue;
	}

	template<typename L, typename CL, typename U>
	[[nodiscard]]
	constexpr bool operator==(const IProperty<L, CL>& lhs, const U& rhs) noexcept
	{
		return lhs.myValue == rhs;
	}
}
