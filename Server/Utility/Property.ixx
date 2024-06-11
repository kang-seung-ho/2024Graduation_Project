module;
#include "PropertyBoolean.hpp"
#include "PropertyArithmetics.hpp"

export module Iconer.Utility.Property;
import Iconer.Utility.TypeTraits;
import Iconer.Utility.IProperty;

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

	template<classes Context, typename T, method_invocable<Context, T> Method>
	Property(Context*, Method, T) -> Property<T, Context>;
}

module:private;

namespace iconer::test
{
	using namespace iconer::util;

	void Del1(int val) noexcept
	{}

	long Del2(bool val)
	{
		return 1;
	}

	void Del3(float val) {}

	struct Del4
	{
		void operator()(int val)
		{}

		void operator()(int val) const
		{}
	};

	struct Del5
	{
		float Implementation(int val) noexcept
		{
			return 0;
		}

		void AAA4()
		{
			//myValue.isDelegateNothrowInvocable;
		}

		void AAA45(int)
		{

		}

		Property<int, Del5> myProp{ this, &Del5::AAA45, -1 };
	};

	void AAA45(int) {}

	constexpr int AAA48()
	{
		Property<int> res{ 10 };

		return res;
	}

	void aAA()
	{
		Property<bool> pk;

		constexpr auto aaa48 = AAA48();

		constexpr bool cc = arithmetics<int>;

		const auto aai0 = Property<int>{ 0 };
		const auto aai1 = Property<int>{ 0, Del1 };
		const Property aai2 = Property{ 0, Del1 };
		const auto aai3 = Property<int>{};
		const auto aai4 = Property{ 0LL };
		const auto aai5 = Property{ 0, Del1 };
		const auto aai6 = Property{ Del1 };
		const auto aai7 = Property{ 0, Del3 };
		const auto aai8 = Property{ Del3 };

		constexpr Del4 aai10_del{};
		const auto aai10 = Property<int>{ 1, aai10_del };

		using test_del_t = Property<int, Del4>;

		Del5 aaa{};
		test_del_t myProp1{ &aaa, &Del5::AAA45 };

		const auto aab0 = Property{ true };
		auto aab1 = Property{ false };
		const auto aab2 = Property<bool>{};
		const auto aab3 = Property{ true, Del2 };
		aab1 |= true;
		aab1 |= aab2;

		//aab1 |= 1;

		const auto aaf0 = Property<float>{};
		const auto aaf1 = Property<float>{ 308931 };
		const auto aaf2 = Property<float>{ 243423 };

		using method_tt = decltype(Del5::Implementation);

		Del5 del5_instance{};
		const auto del5_myValue0 = Property<int, Del5>(&del5_instance, &Del5::Implementation, 0);
		const auto del5_myValue1 = Property(&del5_instance, &Del5::Implementation);
	}

	void BoolDel1(bool val) noexcept {}

	long BoolDel2(bool val) { return 1; }

	struct BoolDel3
	{
		long Method0(bool) {}
		void Method1(bool) {}

		Property<bool> myProp0{ this, &BoolDel3::Method0 };
		Property<bool> myProp1{ this, &BoolDel3::Method1 };
	};



	void test_bools()
	{
		const Property<bool> abool0{};

		const Property<bool> abool1{ BoolDel1 };

		const Property<bool> abool2{ BoolDel2 };
	}
}
