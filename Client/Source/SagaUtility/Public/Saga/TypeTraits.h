#pragma once
#ifndef SAGAFRAMEWORK_UTIL_TYPETRAITS_H
#define SAGAFRAMEWORK_UTIL_TYPETRAITS_H

#include <type_traits>
#include <concepts>
#include <utility>

namespace saga
{
	// type properties
	using ::std::add_const;
	using ::std::add_const_t;
	using ::std::add_volatile;
	using ::std::add_volatile_t;
	using ::std::add_cv;
	using ::std::add_cv_t;
	using ::std::add_pointer;
	using ::std::add_pointer_t;
	using ::std::add_lvalue_reference;
	using ::std::add_lvalue_reference_t;
	using ::std::add_rvalue_reference;
	using ::std::add_rvalue_reference_t;

	using ::std::remove_const;
	using ::std::remove_const_t;
	using ::std::remove_volatile;
	using ::std::remove_volatile_t;
	using ::std::remove_cv;
	using ::std::remove_cv_t;
	using ::std::remove_pointer;
	using ::std::remove_pointer_t;
	using ::std::remove_extent;
	using ::std::remove_extent_t;
	using ::std::remove_all_extents;
	using ::std::remove_all_extents_t;
	using ::std::remove_reference;
	using ::std::remove_reference_t;
	using ::std::remove_cvref;
	using ::std::remove_cvref_t;

	using ::std::ref;
	using ::std::cref;
	using ::std::unwrap_reference;
	using ::std::unwrap_reference_t;
	using ::std::unwrap_reference;
	using ::std::unwrap_reference_t;
	using ::std::decay;
	using ::std::decay_t;
	using ::std::unwrap_ref_decay;
	using ::std::unwrap_ref_decay_t;

	// logical meta functors
	using ::std::negation;
	using ::std::negation_v;
	using ::std::conditional;
	using ::std::conditional_t;
	using ::std::enable_if;
	using ::std::enable_if_t;
	using ::std::void_t;

	using ::std::integral_constant;
	using ::std::true_type;
	using ::std::false_type;

	using ::std::is_same;
	using ::std::is_same_v;

	// ctor flags
	using ::std::is_copy_constructible;
	using ::std::is_move_constructible;
	using ::std::is_copy_constructible_v;
	using ::std::is_move_constructible_v;
	using ::std::is_copy_assignable;
	using ::std::is_move_assignable;
	using ::std::is_copy_assignable_v;
	using ::std::is_move_assignable_v;

	// trivials
	using ::std::is_trivial;
	using ::std::is_trivial_v;
	using ::std::is_trivially_constructible;
	using ::std::is_trivially_constructible_v;
	using ::std::is_trivially_default_constructible;
	using ::std::is_trivially_default_constructible_v;
	using ::std::is_trivially_copy_constructible;
	using ::std::is_trivially_copy_constructible_v;
	using ::std::is_trivially_move_constructible;
	using ::std::is_trivially_move_constructible_v;
	using ::std::is_trivially_copy_assignable;
	using ::std::is_trivially_copy_assignable_v;
	using ::std::is_trivially_move_assignable;
	using ::std::is_trivially_move_assignable_v;
	using ::std::is_trivially_destructible;
	using ::std::is_trivially_destructible_v;

	// functions
	using ::std::is_function;
	using ::std::is_function_v;
	using ::std::is_member_function_pointer;
	using ::std::is_member_function_pointer_v;
	using ::std::invoke_result;
	using ::std::invoke_result_t;

	template <size_t... Indices>
	struct in_place_indices_t
	{
		explicit constexpr in_place_indices_t() noexcept = default;
		constexpr ~in_place_indices_t() noexcept = default;

		explicit constexpr in_place_indices_t(std::index_sequence<Indices...>) noexcept
		{
		}
	};

	template <size_t... Indices>
	inline constexpr in_place_indices_t<Indices...> in_place_indices{};

	// cleaner
	template<typename T>
	using clean_t = std::remove_cvref_t<std::remove_all_extents_t<T>>;

	template <typename T>
	inline constexpr bool is_actual_integral_v = std::_Is_any_of_v<std::remove_cvref_t<T>, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long>;

	template <typename Special, template <typename...> typename Template>
	struct is_specialization : std::false_type
	{};

	template<template<typename...> typename Template, typename... Args>
	struct is_specialization<Template<Args...>, Template> : std::true_type
	{};

	template<typename Special, template<typename...> typename Template>
	inline constexpr bool is_specialization_v = is_specialization<Special, Template>::value;

	template <typename Special, template <size_t...> typename Template>
	struct is_indexed : std::false_type
	{};

	template<template<size_t...> typename Template, size_t... Indices>
	struct is_indexed<Template<Indices...>, Template> : std::true_type
	{};

	template<typename Special, template<size_t...> typename Template>
	inline constexpr bool is_indexed_v = is_indexed<Special, Template>::value;

	template<typename T>
	struct avoid_void
	{
		template<typename Fn, typename... Args>
		static consteval auto Eval() noexcept(std::is_nothrow_invocables<Fn, Args...>)->invoke_result_t<Fn, Args...>;
	};

	template<typename Void>
		requires (is_same_v<Void, void>)
	struct avoid_void<Void>
	{
		template<typename Fn, typename... Args>
		static consteval auto Eval() noexcept(std::is_nothrow_invocable_v<Fn, Args...>)->invoke_result_t<Fn, Args...>;

		template<typename Fn>
		static consteval auto Eval() noexcept(std::is_nothrow_invocable_v<Fn>)->invoke_result_t<Fn>;
	};

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
	struct logical_product<MetaFn, Wrapper> : false_type
	{};

	template<template<typename> typename MetaFn, template<typename> typename Wrapper, typename... Ts>
	inline constexpr bool logical_product_v = logical_product<MetaFn, Wrapper, Ts...>::value;

	template<template<typename> typename MetaFn, typename... Ts>
	inline constexpr bool make_conjunction = logical_product_v<MetaFn, std::conjunction, Ts...>;

	template<template<typename> typename MetaFn, typename... Ts>
	inline constexpr bool make_disjunction = logical_product_v<MetaFn, std::disjunction, Ts...>;

	template<typename T>
	struct is_explicit_constructible : conditional_t<is_trivial_v<T>, true_type, false_type> {};

	template<typename T>
	inline constexpr bool is_explicit_constructible_v = is_explicit_constructible<T>::value;

	template<typename T>
	using make_lvalue_t = conditional_t<std::same_as<clean_t<T>, void>, void, add_lvalue_reference_t<remove_const_t<T>>>;

	template<typename T>
	using make_clvalue_t = conditional_t<std::same_as<clean_t<T>, void>, void, add_lvalue_reference_t<add_const_t<T>>>;

	template<typename T>
	using make_rvalue_t = conditional_t<std::same_as<clean_t<T>, void>, void, add_rvalue_reference_t<remove_const_t<T>>>;

	template<typename T>
	using make_crvalue_t = conditional_t<std::same_as<clean_t<T>, void>, void, add_rvalue_reference_t<add_const_t<T>>>;

	template<typename Fn, typename... Params>
	using function_t = Fn(*)(Params...);

	template<typename Fn, typename... Params>
	using nothrow_function_t = Fn(*)(Params...) noexcept;

	template<typename Class, typename Fn, typename... Params>
	using method_t = Fn(clean_t<Class>::*)(Params...);

	template<typename Class, typename Fn, typename... Params>
	using const_method_t = Fn(clean_t<Class>::*)(Params...) const;

	template<typename Class, typename Fn, typename... Params>
	using nothrow_method_t = Fn(clean_t<Class>::*)(Params...) noexcept;

	template<typename Class, typename Fn, typename... Params>
	using const_nothrow_method_t = Fn(clean_t<Class>::*)(Params...) const noexcept;

	template<typename Class, typename Fn, typename... Params>
	using method_lv_t = Fn(clean_t<Class>::*)(Params...)&;

	template<typename Class, typename Fn, typename... Params>
	using method_cl_t = Fn(clean_t<Class>::*)(Params...) const&;

	template<typename Class, typename Fn, typename... Params>
	using method_rv_t = Fn(clean_t<Class>::*)(Params...)&&;

	template<typename Class, typename Fn, typename... Params>
	using method_cr_t = Fn(clean_t<Class>::*)(Params...) const&&;

	template<typename Class, typename Fn, typename... Params>
	using nothrow_method_lv_t = Fn(clean_t<Class>::*)(Params...) & noexcept;

	template<typename Class, typename Fn, typename... Params>
	using nothrow_method_cl_t = Fn(clean_t<Class>::*)(Params...) const& noexcept;

	template<typename Class, typename Fn, typename... Params>
	using nothrow_method_rv_t = Fn(clean_t<Class>::*)(Params...) && noexcept;

	template<typename Class, typename Fn, typename... Params>
	using nothrow_method_cr_t = Fn(clean_t<Class>::*)(Params...) const&& noexcept;

	template<typename Ref, typename M>
	struct is_method_invocable
		: public false_type
	{};

	template<typename Ref, typename R, typename... Params>
	struct is_method_invocable<Ref, method_t<Ref, R, Params...>>
		: public std::bool_constant<!std::is_const_v<remove_reference_t<Ref>>>
	{};

	template<typename Ref, typename R, typename... Params>
	struct is_method_invocable<Ref, const_method_t<Ref, R, Params...>>
		: public true_type
	{};

	template<typename Ref, typename R, typename... Params>
	struct is_method_invocable<Ref, method_lv_t<Ref, R, Params...>>
		: public std::bool_constant<!std::is_const_v<remove_reference_t<Ref>>>
	{};

	template<typename Ref, typename R, typename... Params>
	struct is_method_invocable<Ref, method_cl_t<Ref, R, Params...>>
		: public true_type
	{};

	template<typename Ref, typename R, typename... Params>
	struct is_method_invocable<Ref, method_rv_t<Ref, R, Params...>>
		: public std::bool_constant<std::is_rvalue_reference_v<remove_const_t<Ref>> && !std::is_const_v<remove_reference_t<Ref>>>
	{};

	template<typename Ref, typename R, typename... Params>
	struct is_method_invocable<Ref, method_cr_t<Ref, R, Params...>>
		: public std::bool_constant<std::is_rvalue_reference_v<remove_const_t<Ref>>>
	{};

	template<typename Ref, typename R, typename... Params>
	struct is_method_invocable<Ref, nothrow_method_t<Ref, R, Params...>>
		: public is_method_invocable<Ref, method_t<Ref, R, Params...>>
	{};

	template<typename Ref, typename R, typename... Params>
	struct is_method_invocable<Ref, const_nothrow_method_t<Ref, R, Params...>>
		: public is_method_invocable<Ref, const_method_t<Ref, R, Params...>>
	{};

	template<typename Ref, typename R, typename... Params>
	struct is_method_invocable<Ref, nothrow_method_lv_t<Ref, R, Params...>>
		: public is_method_invocable<Ref, method_lv_t<Ref, R, Params...>>
	{};

	template<typename Ref, typename R, typename... Params>
	struct is_method_invocable<Ref, nothrow_method_cl_t<Ref, R, Params...>>
		: public is_method_invocable<Ref, method_cl_t<Ref, R, Params...>>
	{};

	template<typename Ref, typename R, typename... Params>
	struct is_method_invocable<Ref, nothrow_method_rv_t<Ref, R, Params...>>
		: public is_method_invocable<Ref, method_rv_t<Ref, R, Params...>>
	{};

	template<typename Ref, typename R, typename... Params>
	struct is_method_invocable<Ref, nothrow_method_cr_t<Ref, R, Params...>>
		: public is_method_invocable<Ref, method_cr_t<Ref, R, Params...>>
	{};

	template<typename Ref, typename M>
	inline constexpr bool is_method_invocable_v = is_method_invocable<Ref, M>::value;

	template<typename M, typename C>
	struct [[nodiscard]] method_trait;

	template<typename C, typename R, typename... Params>
	struct [[nodiscard]] method_trait<method_t<C, R, Params...>, C>
	{
		using type = method_t<decay_t<C>, R, Params...>;
		using result = R;
		using owner = C;

		template<typename... Args>
		using apply = decltype((std::declval<C>().*std::declval<type>())(std::declval<Args>()...));

		static inline constexpr bool is_nothrow = false;
	};

	template<typename C, typename R, typename... Params>
	struct [[nodiscard]] method_trait<const_method_t<C, R, Params...>, C>
	{
		using type = const_method_t<decay_t<C>, R, Params...>;
		using result = R;
		using owner = C;

		template<typename... Args>
		using apply = decltype((std::declval<C>().*std::declval<type>())(std::declval<Args>()...));

		static inline constexpr bool is_nothrow = false;
	};

	template<typename C, typename R, typename... Params>
	struct [[nodiscard]] method_trait<nothrow_method_t<C, R, Params...>, C>
	{
		using type = nothrow_method_t<decay_t<C>, R, Params...>;
		using result = R;
		using owner = C;

		template<typename... Args>
		using apply = decltype((std::declval<C>().*std::declval<type>())(std::declval<Args>()...));

		static inline constexpr bool is_nothrow = true;
	};

	template<typename C, typename R, typename... Params>
	struct [[nodiscard]] method_trait<const_nothrow_method_t<C, R, Params...>, C>
	{
		using type = const_nothrow_method_t<decay_t<C>, R, Params...>;
		using result = R;
		using owner = C;

		template<typename... Args>
		using apply = decltype((std::declval<C>().*std::declval<type>())(std::declval<Args>()...));

		static inline constexpr bool is_nothrow = true;
	};

	/// <summary>
	/// A decayed method type trait
	/// </summary>
	/// <typeparam name="M"></typeparam>
	/// <typeparam name="C"></typeparam>
	template<typename M, typename C>
	using method_trait_t = typename method_trait<clean_t<M>, C>::type;

	// summarize byte size of each type in a sequence
	template <typename...>
	struct byte_size;

	template <typename... Ts>
	inline constexpr size_t byte_size_v = byte_size<Ts...>::value;

	template <typename T>
	struct byte_size<T> : public std::integral_constant<size_t, sizeof(T)>
	{};

	template <typename T, typename... Rests>
	struct byte_size<T, Rests...> : public std::integral_constant<size_t, sizeof(T) + byte_size_v<Rests...>>
	{};
}

#endif // !SAGAFRAMEWORK_UTIL_TYPETRAITS_H
