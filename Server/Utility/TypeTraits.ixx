export module Iconer.Utility.TypeTraits;
export import <type_traits>;
export import <concepts>;

export namespace iconer
{
	// cleaner
	template<typename T>
	using clean_t = std::remove_cvref_t<std::remove_all_extents_t<T>>;

	template<typename Fn, typename... Params>
	using function_t = Fn(*)(Params...);

	template<typename Fn, typename... Params>
	using nothrow_function_t = Fn(*)(Params...) noexcept;
	
	template<typename Fn, typename... Params>
	using std_function_t = Fn(__stdcall *)(Params...);

	template<typename Fn, typename... Params>
	using nothrow_std_function_t = Fn(__stdcall *)(Params...) noexcept;

	template<typename Class, typename Fn, typename... Params>
	using method_t = Fn(clean_t<Class>::*)(Params...);

	template<typename Class, typename Fn, typename... Params>
	using const_method_t = Fn(clean_t<Class>::*)(Params...) const;

	template<typename Class, typename Fn, typename... Params>
	using nothrow_method_t = Fn(clean_t<Class>::*)(Params...) noexcept;

	template<typename Class, typename Fn, typename... Params>
	using const_nothrow_method_t = Fn(clean_t<Class>::*)(Params...) const noexcept;

	template<typename T>
	using is_void = std::is_same<clean_t<T>, void>;

	template<typename T>
	using is_not_void = std::negation<std::is_same<clean_t<T>, void>>;

	template <typename Special, template <typename...> typename Template>
	struct is_specialization : std::false_type
	{};

	template<template<typename...> typename Template, typename... Args>
	struct is_specialization<Template<Args...>, Template> : std::true_type
	{};

	template<typename Special, template<typename...> typename Template>
	inline constexpr bool is_specialization_v = is_specialization<Special, Template>::value;

	template<template<typename> typename MetaFn, template<typename> typename Wrapper, typename... Ts>
	struct logical_product
	{
		using result = Wrapper<MetaFn<Ts>...>;
		static inline constexpr bool value = result::value;
	};

	template<template<typename> typename MetaFn, template<typename> typename Wrapper, typename... Rests>
	struct logical_product<MetaFn, Wrapper, void, Rests...> : logical_product<MetaFn, Wrapper, Rests...>
	{};

	template<template<typename> typename MetaFn, template<typename> typename Wrapper>
	struct logical_product<MetaFn, Wrapper> : std::false_type
	{};

	template<template<typename> typename MetaFn, template<typename> typename Wrapper, typename... Ts>
	inline constexpr bool logical_product_v = logical_product<MetaFn, Wrapper, Ts...>::value;

	template<template<typename> typename MetaFn, typename... Ts>
	inline constexpr bool make_conjunction = logical_product_v<MetaFn, std::conjunction, Ts...>;

	template<template<typename> typename MetaFn, typename... Ts>
	inline constexpr bool make_disjunction = logical_product_v<MetaFn, std::disjunction, Ts...>;

	template<typename... Ts>
	concept classes = make_conjunction<std::is_class, clean_t<Ts>...>;

	template<typename T>
	concept finalized = classes<T> && std::is_final_v<clean_t<T>>;

	template<typename... Ts>
	concept functions = make_conjunction<std::is_function, clean_t<Ts>...>;

	template<typename... Ts>
	concept member_function_ptrs = make_conjunction<std::is_member_function_pointer, std::decay_t<Ts>...>;

	template<typename Method>
	concept methods = member_function_ptrs<Method>;

	template<typename Method, typename Class, typename... Args>
	concept method_invocable = classes<clean_t<Class>> and methods<std::decay_t<Method>>
		and requires(Class& instance)
	{
		std::invoke(std::declval<Method>(), instance, std::declval<Args>()...);
	};

	template<typename Derived, typename Parent>
	concept hierachy = classes<Derived, Parent>&& std::derived_from<Derived, std::remove_cv_t<Parent>>;

	template<typename D>
	concept crtp = classes<D> && std::same_as<D, std::remove_cv_t<D>>;

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
	concept nothrow_constructible = std::is_nothrow_constructible_v<T, Args...>;

	template<typename From, typename To>
	concept nothrow_convertible = std::is_nothrow_convertible_v<From, To>;

	template<typename From, typename To>
	concept nothrow_assignable = std::is_nothrow_assignable_v<To, From>;

	using std::invocable;

	template<typename T, typename... Args>
	concept nothrow_invocable = std::is_nothrow_invocable_v<T, Args...>;

	template<typename T, typename Result, typename... Args>
	concept nothrow_r_invocable = std::is_nothrow_invocable_r_v<Result, T, Args...>;

	template <typename T>
	inline constexpr bool is_actual_integral_v = std::_Is_any_of_v<std::remove_cvref_t<T>, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long>;

	template <typename T>
	concept actual_integral = is_actual_integral_v<T>;

	template<typename Special, template<typename...> typename Template>
	concept specializations = is_specialization_v<clean_t<Special>, Template>;
}