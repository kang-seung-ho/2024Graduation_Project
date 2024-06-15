export module Iconer.Utility.InvokeTraits;
import Iconer.Utility.TypeTraits;

export namespace iconer::util
{
	template<typename T, typename R, typename Context>
	struct InvokeTraits
	{
		using value_type = T;
		using reference_type = std::add_lvalue_reference_t<value_type>;
		using const_reference_type = std::add_lvalue_reference_t<std::add_const_t<value_type>>;
		using rvalue_type = std::add_rvalue_reference_t<value_type>;
		using const_rvalue_type = std::add_rvalue_reference_t<std::add_const_t<value_type>>;

		using context_type = Context;
		using context_pointer = std::add_pointer_t<context_type>;
		using const_context_pointer = std::add_pointer_t<std::add_const_t<context_type>>;

		using invoke_fun_t = function_t<R, value_type>;
		using invoke_ref_fun_t = function_t<R, reference_type>;
		using invoke_cref_fun_t = function_t<R, const_reference_type>;
		using invoke_rval_fun_t = function_t<R, rvalue_type>;
		using invoke_crvl_fun_t = function_t<R, const_rvalue_type>;

		using invoke_met_t = method_t<context_type, R, value_type>;
		using invoke_ref_met_t = method_t<context_type, R, reference_type>;
		using invoke_cref_met_t = method_t<context_type, R, const_reference_type>;
		using invoke_rval_met_t = method_t<context_type, R, rvalue_type>;
		using invoke_crvl_met_t = method_t<context_type, R, const_rvalue_type>;

		using invoke_const_met_t = const_method_t<context_type, R, value_type>;
		using invoke_const_ref_met_t = const_method_t<context_type, R, reference_type>;
		using invoke_const_cref_met_t = const_method_t<context_type, R, const_reference_type>;
		using invoke_const_rval_met_t = const_method_t<context_type, R, rvalue_type>;
		using invoke_const_crvl_met_t = const_method_t<context_type, R, const_rvalue_type>;
	};

	template<typename T, typename R>
	struct InvokeTraits<T, R, void>
	{
		using value_type = T;
		using reference_type = std::add_lvalue_reference_t<value_type>;
		using const_reference_type = std::add_lvalue_reference_t<std::add_const_t<value_type>>;
		using rvalue_type = std::add_rvalue_reference_t<value_type>;
		using const_rvalue_type = std::add_rvalue_reference_t<std::add_const_t<value_type>>;

		using context_type = void;

		using invoke_fun_t = function_t<R, value_type>;
		using invoke_ref_fun_t = function_t<R, reference_type>;
		using invoke_cref_fun_t = function_t<R, const_reference_type>;
		using invoke_rval_fun_t = function_t<R, rvalue_type>;
		using invoke_crvl_fun_t = function_t<R, const_rvalue_type>;
	};
}
