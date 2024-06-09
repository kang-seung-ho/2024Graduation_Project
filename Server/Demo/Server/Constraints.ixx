export module Iconer.Utility.Constraints;
export import Iconer.Utility.TypeTraits;
export import <concepts>;
import <utility>;

export namespace iconer
{
	// functions
	using std::as_const;

	// placeholders
	using std::in_place_t;
	using std::in_place;
	using std::in_place_type_t;
	using std::in_place_type;
	using std::in_place_index_t;
	using std::in_place_index;

	// models
	using ::std::same_as;
	using ::std::copyable;
	using ::std::movable;

	using ::std::default_initializable;
	using ::std::copy_constructible;
	using ::std::move_constructible;
	using ::std::constructible_from;
	using ::std::convertible_to;
	using ::std::assignable_from;

	using ::std::equality_comparable;
	using ::std::equality_comparable_with;

	template<typename... Ts>
	concept classes = make_conjunction<std::is_class, clean_t<Ts>...>;

	template<typename T>
	concept finalized = classes<T> && std::is_final_v<clean_t<T>>;

	template<typename... Ts>
	concept functions = make_conjunction<is_function, clean_t<Ts>...>;

	template<typename... Ts>
	concept member_function_ptrs = make_conjunction<is_member_function_pointer, decay_t<Ts>...>;

	template<typename Method>
	concept methods = member_function_ptrs<Method>;

	template<typename Method, typename Class, typename... Args>
	concept method_invocable = classes<clean_t<Class>> and methods<decay_t<Method>> and requires
	{
		std::invoke(std::declval<Method>(), std::declval<Class>(), std::declval<Args>()...);
	};

	template<typename Derived, typename Parent>
	concept hierachy = classes<Derived, Parent>&& std::derived_from<Derived, remove_cv_t<Parent>>;

	template<typename D>
	concept crtp = classes<D> && std::same_as<D, remove_cv_t<D>>;

	template<typename T>
	using is_void = std::is_same<clean_t<T>, void>;

	template<typename T>
	using is_not_void = std::negation<std::is_same<clean_t<T>, void>>;

	template<typename... Ts>
	concept voids = make_conjunction<is_void, clean_t<Ts>...>;

	template<typename... Ts>
	concept notvoids = make_conjunction<is_not_void, clean_t<Ts>...>;

	template<typename... Ts>
	concept integrals = make_conjunction<std::is_integral, clean_t<Ts>...>;

	template<typename... Ts>
	concept floats = make_conjunction<std::is_floating_point, clean_t<Ts>...>;

	template<typename... Ts>
	concept enumerations = make_conjunction<std::is_enum, clean_t<Ts>...>;

	template<typename... Ts>
	concept soft_enumerations = enumerations<Ts...> && !make_disjunction<std::is_scoped_enum, clean_t<Ts>...>;

	template<typename... Ts>
	concept hard_enumerations = enumerations<Ts...> && make_conjunction<std::is_scoped_enum, clean_t<Ts>...>;

	template<typename T, typename... Args>
	concept invocables = !std::is_abstract_v<clean_t<T>> && std::invocable<T, Args...>;

	template<typename T, typename Result, typename... Args>
	concept r_invocables = !std::is_abstract_v<clean_t<T>> && std::invocable<T, Args...>&& std::is_invocable_r_v<Result, T, Args...>;

	template<typename Fn, typename T>
	concept lv_invocable = invocables<Fn, make_lvalue_t<T>>;

	template<typename Fn, typename T>
	concept rv_invocable = invocables<Fn, make_rvalue_t<T>>;

	template<typename Fn, typename T>
	concept cl_invocable = invocables<Fn, make_clvalue_t<T>>;

	template<typename Fn, typename T>
	concept cr_invocable = invocables<Fn, make_crvalue_t<T>>;

	template<typename T>
	concept aggregate_classes = classes<T> && std::is_aggregate_v<clean_t<T>>;

	template<typename T>
	concept nonaggregate_classes = classes<T> && std::is_aggregate_v<clean_t<T>>;

	template<typename... Ts>
	concept default_initializables = make_conjunction<std::is_default_constructible, Ts...>;

	template<typename... Ts>
	concept copy_constructibles = make_conjunction<std::is_copy_constructible, Ts...>;

	template<typename... Ts>
	concept move_constructibles = make_conjunction<std::is_move_constructible, Ts...>;

	template<typename... Ts>
	concept copy_assignables = make_conjunction<std::is_copy_assignable, Ts...>;

	template<typename... Ts>
	concept move_assignables = make_conjunction<std::is_move_assignable, Ts...>;

	template<typename... Ts>
	concept trivials = make_conjunction<std::is_trivial, Ts...>;

	template<typename... Ts>
	concept trivially_destructibles = make_conjunction<std::is_trivially_destructible, Ts...>;

	template<typename... Ts>
	concept trivially_initializables = make_conjunction<std::is_trivially_default_constructible, Ts...>;

	template<typename... Ts>
	concept trivially_copy_constructibles = make_conjunction<std::is_trivially_copy_constructible, Ts...>;

	template<typename... Ts>
	concept trivially_move_constructibles = make_conjunction<std::is_trivially_move_constructible, Ts...>;

	template<typename... Ts>
	concept trivially_copy_assignables = make_conjunction<std::is_trivially_copy_assignable, Ts...>;

	template<typename... Ts>
	concept trivially_move_assignables = make_conjunction<std::is_trivially_move_assignable, Ts...>;

	template<typename... Ts>
	concept nothrow_default_constructibles = make_conjunction<std::is_nothrow_default_constructible, Ts...>;

	template<typename T, typename... Args>
	concept nothrow_constructible = std::is_nothrow_constructible_v<T, Args...>;

	template<typename From, typename To>
	concept nothrow_convertibles = std::is_nothrow_convertible_v<From, To>;

	template<typename From, typename To>
	concept nothrow_assignable = std::is_nothrow_assignable_v<To, From>;

	template<typename... Ts>
	concept nothrow_copy_constructibles = make_conjunction<std::is_nothrow_copy_constructible, Ts...>;

	template<typename... Ts>
	concept nothrow_move_constructibles = make_conjunction<std::is_nothrow_move_constructible, Ts...>;

	template<typename... Ts>
	concept nothrow_copy_assignables = make_conjunction<std::is_nothrow_copy_assignable, Ts...>;

	template<typename... Ts>
	concept nothrow_move_assignables = make_conjunction<std::is_nothrow_move_assignable, Ts...>;

	template<typename... Ts>
	concept nothrow_swappables = make_conjunction<std::is_nothrow_swappable, Ts...>;

	template<typename... Ts>
	concept nothrow_destructibles = make_conjunction<std::is_nothrow_destructible, Ts...>;

	template<typename T, typename... Args>
	concept nothrow_invocables = std::is_nothrow_invocable_v<T, Args...>;

	template<typename T, typename Result, typename... Args>
	concept nothrow_r_invocables = std::is_nothrow_invocable_r_v<Result, T, Args...>;

	template<typename T>
	concept incrementable = requires(T t)
	{
		{ ++t } -> std::same_as<T>;
		{ t++ } -> std::same_as<T>;
	};

	template<typename T>
	concept nothrow_incrementable = requires(T t)
	{
		{ ++t } noexcept;
		{ t++ } noexcept;
	};

	template<typename T>
	concept decrementable = requires(T t)
	{
		{ --t } -> std::same_as<T>;
		{ t-- } -> std::same_as<T>;
	};

	template<typename T>
	concept nothrow_decrementable = requires(T t)
	{
		{ --t } noexcept;
		{ t-- } noexcept;
	};

	template<typename T, typename S>
	concept addable_with = requires(const T t, const S s)
	{
		{ t + s } -> std::same_as<std::common_type_t<T, S>>;
		{ s + t } -> std::same_as<std::common_type_t<T, S>>;
	};

	template<typename T, typename S>
	concept nothrow_addable_with = requires(const T t, const S s)
	{
		{ t + s } noexcept;
		{ s + t } noexcept;
	};

	template<typename T, typename S>
	concept subtractable_with = requires(const T t, const S s)
	{
		{ t - s } -> std::same_as<std::common_type_t<T, S>>;
		{ s - t } -> std::same_as<std::common_type_t<T, S>>;
	};

	template<typename T, typename S>
	concept nothrow_subtractable_with = requires(const T t, const S s)
	{
		{ t - s } noexcept;
		{ s - t } noexcept;
	};

	template<typename T, typename S>
	concept multipliable_with = requires(const T t, const S s)
	{
		{ t* s } -> std::same_as<std::common_type_t<T, S>>;
		{ s* t } -> std::same_as<std::common_type_t<T, S>>;
	};

	template<typename T, typename S>
	concept nothrow_multipliable_with = requires(const T t, const S s)
	{
		{ t* s } noexcept;
		{ s* t } noexcept;
	};

	template<typename T, typename S>
	concept dividable_with = requires(const T t, const S s)
	{
		{ t / s } -> std::same_as<std::common_type_t<T, S>>;
		{ s / t } -> std::same_as<std::common_type_t<T, S>>;
	};

	template<typename T, typename S>
	concept nothrow_dividable_with = requires(const T t, const S s)
	{
		{ t / s } noexcept;
		{ s / t } noexcept;
	};

	template<typename T, typename S>
	concept arithmetical_with = addable_with<T, S>
		&& subtractable_with<T, S>
		&& multipliable_with<T, S>
		&& dividable_with<T, S>;

	template<typename T, typename S>
	concept nothrow_arithmetical_with = arithmetical_with<T, S>
		&& nothrow_addable_with<T, S>
		&& nothrow_subtractable_with<T, S>
		&& nothrow_multipliable_with<T, S>
		&& nothrow_dividable_with<T, S>;

	template<typename T>
	concept addable = addable_with<T, T>;

	template<typename T>
	concept nothrow_addable = nothrow_addable_with<T, T>;

	template<typename T>
	concept subtractable = subtractable_with<T, T>;

	template<typename T>
	concept nothrow_subtractable = nothrow_subtractable_with<T, T>;

	template<typename T>
	concept multipliable = multipliable_with<T, T>;

	template<typename T>
	concept nothrow_multipliable = nothrow_multipliable_with<T, T>;

	template<typename T>
	concept dividable = dividable_with<T, T>;

	template<typename T>
	concept nothrow_dividable = nothrow_dividable_with<T, T>;

	template<typename T>
	concept arithmetical = arithmetical_with<T, T>;

	template<typename T>
	concept nothrow_arithmetical = nothrow_arithmetical_with<T, T>;

	template<typename T>
	concept basic_arithmeticals = std::is_arithmetic_v<clean_t<T>>;

	template<typename... Args>
	concept argument_available = 0 < sizeof...(Args);

	template<size_t... Indices>
	concept index_available = 0 < sizeof...(Indices);

	template<typename Ty1, typename Ty2>
	concept similar = !std::same_as<clean_t<Ty1>, clean_t<Ty2>>&& std::common_with<clean_t<Ty1>, clean_t<Ty2>>;

	template <typename T>
	concept actual_integral = is_actual_integral_v<T>;

	namespace detail
	{
		template<typename T, typename Oty>
		struct same_as_t
			: public std::bool_constant<std::same_as<T, Oty>>
		{};

		template<typename Oty>
		struct same_as_t<void, Oty>
			: public is_same<void, Oty>
		{};

		template<typename T>
		struct same_as_t<T, void>
			: public is_same<T, void>
		{};

		template<typename T>
		struct same_as_bind
		{
			template<typename Oty>
			using result = same_as_t<T, Oty>;
		};
	}

	template<typename T, typename... Ots>
	concept abs_same = make_conjunction<typename detail::same_as_bind<T>::template result, Ots...>;

	template<typename Special, template<typename...> typename Template>
	concept specializations = is_specialization_v<clean_t<Special>, Template>;

	template<typename Fn, typename Rx, typename... Args>
	concept invocable_results = invocables<Fn, Args...>&& same_as<std::invoke_result_t<Fn, Args...>, Rx>;

	template<classes Host, typename Method>
	[[nodiscard]]
	consteval bool CheckMethodException() noexcept
	{
		if constexpr (method_invocable<Method, Host&&>)
		{
			return noexcept((std::declval<Host&&>().*std::declval<Method>())());
		}
		else
		{
			return true;
		}
	}
}
