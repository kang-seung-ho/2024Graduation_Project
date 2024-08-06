#pragma once
#include <limits>;
#include <optional>;
#include <chrono>;

constexpr std::optional<float> Power(const float value, int times) noexcept
{
	constexpr auto inf = std::numeric_limits<float>::infinity();
	constexpr auto low = -inf;

	if (inf <= value or value <= low)
	{
		return std::nullopt;
	}

	float result = value;

	if (0 < times)
	{
		for (int i = 1; i < times; ++i)
		{
			result *= value;
		}
	}
	else
	{
		do
		{
			result /= value;
		}
		while (0 < (--times));
	}

	return result;
}

constexpr float fun_0(int exp)
{
	return Power(20.4f, exp).value_or(FP_NAN);
}

constexpr float fun_1(float init, int count)
{
	for (int i = 0; i < count; ++i)
	{
		init = init + (init + 1);
	}

	return init;
}

// C++23
constexpr auto fun_2() noexcept
{
	try
	{
		throw "Ok?";
	}
	catch (...)
	{
		return 0;
	}

	return 1;
}

int aaa()
{
	const std::chrono::system_clock::duration dur{};
	dur.count();

	constexpr auto value_0 = Power(3, 5);

	constexpr auto value_1 = Power(value_0.value(), 5);

	constexpr auto value_2 = Power(value_1.value(), -3);

	const auto value_3 = Power(value_2.value(), 2);

	const auto value_4 = Power(10.0f, 4);

	constexpr auto value_5 = fun_2();

	const auto value_6 = fun_2();

	const auto value_7 = fun_0(6);

	const auto value_8 = fun_1(1e2, 5);

	const auto value_9 = fun_1(3e-6, -10);
}
