export module Iconer.Utility.Property;
export import :Definitions;
export import :Arithmetics;
export import :Boolean;

export namespace iconer::util
{
	template<typename T>
	Property(T) -> Property<T, void>;

	template<typename T>
	Property(function_t<void, T>) -> Property<clean_t<T>, void>;

	template<typename T>
	Property(nothrow_function_t<void, T>) -> Property<clean_t<T>, void>;

	template<typename T>
	Property(T, function_t<void, T>) -> Property<T, void>;

	template<typename T>
	Property(T, nothrow_function_t<void, T>) -> Property<T, void>;

	template<typename T, invocable<T> Delegate>
	Property(T, Delegate) -> Property<T, void>;


	template<typename Context, typename T>
	Property(Context*, method_t<Context, void, T>) -> Property<T, Context>;

	template<typename Context, typename T>
	Property(Context*, const_method_t<Context, void, T>) -> Property<T, Context>;

	template<typename Context, typename T>
	Property(const Context*, const_method_t<Context, void, T>) -> Property<T, Context>;

	template<typename Context, typename T>
	Property(Context*, method_t<Context, void, T>, T) -> Property<T, Context>;

	template<typename Context, typename T>
	Property(Context*, const_method_t<Context, void, T>, T) -> Property<T, Context>;

	template<typename Context, typename T>
	Property(const Context*, const_method_t<Context, void, T>, T) -> Property<T, Context>;


	template<typename Context, typename T>
	Property(Context*, method_t<Context, void, std::add_lvalue_reference_t<T>>) -> Property<T, Context>;

	template<typename Context, typename T>
	Property(Context*, const_method_t<Context, void, std::add_lvalue_reference_t<T>>) -> Property<T, Context>;

	template<typename Context, typename T>
	Property(const Context*, const_method_t<Context, void, std::add_lvalue_reference_t<T>>) -> Property<T, Context>;

	template<typename Context, typename T>
	Property(Context*, method_t<Context, void, std::add_lvalue_reference_t<T>>, T) -> Property<T, Context>;

	template<typename Context, typename T>
	Property(Context*, const_method_t<Context, void, std::add_lvalue_reference_t<T>>, T) -> Property<T, Context>;

	template<typename Context, typename T>
	Property(const Context*, const_method_t<Context, void, std::add_lvalue_reference_t<T>>, T) -> Property<T, Context>;


	template<typename Context, typename T>
	Property(Context*, method_t<Context, void, std::add_lvalue_reference_t<std::add_const_t<T>>>) -> Property<T, Context>;

	template<typename Context, typename T>
	Property(Context*, const_method_t<Context, void, std::add_lvalue_reference_t<std::add_const_t<T>>>) -> Property<T, Context>;

	template<typename Context, typename T>
	Property(const Context*, const_method_t<Context, void, std::add_lvalue_reference_t<std::add_const_t<T>>>) -> Property<T, Context>;

	template<typename Context, typename T>
	Property(Context*, method_t<Context, void, std::add_lvalue_reference_t<std::add_const_t<T>>>, T) -> Property<T, Context>;

	template<typename Context, typename T>
	Property(Context*, const_method_t<Context, void, std::add_lvalue_reference_t<std::add_const_t<T>>>, T) -> Property<T, Context>;

	template<typename Context, typename T>
	Property(const Context*, const_method_t<Context, void, std::add_lvalue_reference_t<std::add_const_t<T>>>, T) -> Property<T, Context>;
}
