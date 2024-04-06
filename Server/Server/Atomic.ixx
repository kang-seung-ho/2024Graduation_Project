export module Iconer.Utility.Atomic;
import Iconer.Utility.Constraints;
export import <atomic>;
import <format>;

export namespace iconer::util
{
	template <typename T>
	struct is_nothrow_atomic_readable : false_type
	{};

	template<typename T>
	struct is_nothrow_atomic_readable<std::atomic<T>>
		: std::bool_constant<noexcept(std::declval<std::atomic<T>>().load(std::declval<std::memory_order>()))>
	{};

	template<typename T>
	inline constexpr bool is_nothrow_atomic_readable_v = is_nothrow_atomic_readable<T>::template value;

	template<typename T>
	concept nothrow_atomic_readable = is_nothrow_atomic_readable_v<T>;

	template <typename T>
	struct is_nothrow_atomic_writable : false_type
	{};

	template<typename T>
	struct is_nothrow_atomic_writable<std::atomic<T>>
		: std::bool_constant<noexcept(std::declval<std::atomic<T>>().store(std::declval<T>(), std::declval<std::memory_order>()))>
	{};

	template <typename T, typename U>
	struct is_nothrow_atomic_writable_with : false_type
	{};

	template<typename T, typename U>
	struct is_nothrow_atomic_writable_with<std::atomic<T>, U>
		: std::bool_constant<noexcept(std::declval<std::atomic<T>>().store(std::declval<U>(), std::declval<std::memory_order>()))>
	{};

	template<typename T>
	inline constexpr bool is_nothrow_atomic_writable_v = is_nothrow_atomic_writable<T>::template value;
	
	template<typename T, typename U>
	inline constexpr bool is_nothrow_atomic_writable_with_v = is_nothrow_atomic_writable_with<T, U>::template value;

	template<typename T, typename U>
	concept nothrow_atomic_writable_with = specializations<T, std::atomic> and is_nothrow_atomic_writable_with_v<T, U>;

	template<typename T>
	concept nothrow_atomic_writable = specializations<T, std::atomic> and is_nothrow_atomic_writable_v<T>;
}

namespace
{
	/// <summary>
	/// {a} : memory order acquire
	/// {r} : memory order release
	/// {o} : memory order acq_rel
	/// {x} : memory order relaxed
	/// {_} : memory order consume
	/// {c} : memory order seq_cst
	/// {}  : default
	/// </summary>
	[[nodiscard]]
	constexpr std::memory_order ParseOrderSpec(const char ch)
	{
		switch (ch)
		{
			case 'a': return std::memory_order_acquire;
			case 'r': return std::memory_order_release;
			case 'o': return std::memory_order_acq_rel;
			case 'x': return std::memory_order_relaxed;
			case '_': return std::memory_order_consume;
			case 'c': return std::memory_order_seq_cst;
			default: std::format_error{ "Invalid order specifier." };
		}
	}
	[[nodiscard]]
	constexpr std::memory_order ParseOrderSpec(const wchar_t ch)
	{
		switch (ch)
		{
			case L'a': return std::memory_order_acquire;
			case L'r': return std::memory_order_release;
			case L'o': return std::memory_order_acq_rel;
			case L'x': return std::memory_order_relaxed;
			case L'_': return std::memory_order_consume;
			case L'c': return std::memory_order_seq_cst;
			default: std::format_error{ "Invalid order specifier." };
		}
	}
}

export template<typename T>
struct std::formatter<std::atomic<T>, char>
{
	constexpr format_parse_context::iterator
		parse(format_parse_context& context)
	{
		auto it = context.begin();
		auto end = context.end();
		if (it == end or *it != '{')
		{
			throw std::format_error{ "Invalid format string." };
		}

		if (*it != '{')
		{
			throw std::format_error{ "Missing '{' in format string." };
		}

		if (++it == end)
		{
			throw std::format_error{ "Missing '}' in format string." };
		}

		if (it != end and *it == '}')
		{
			readOrder = std::memory_order_seq_cst;
			writeOrder = std::memory_order_seq_cst;
			return it;
		}

		readOrder = ParseOrderSpec(*it);

		if (++it == end)
		{
			throw std::format_error{ "Missing '}' in format string." };
		}

		if (*it == '}')
		{
			writeOrder = readOrder;
		}

		writeOrder = ParseOrderSpec(*(it++));

		if (it != end and *it != '}')
		{
			throw std::format_error{ "Missing '}' in format string." };
		}

		return it;
	}

	format_context::iterator
		format(const std::atomic<T>& atomic, format_context& context)
		const noexcept
	{
		auto value = atomic.load(readOrder);
		format_context::iterator out = format_to(context.out(), "Atomic({})", value);
		atomic.store(value, writeOrder);
		return out;
	}

	format_context::iterator
		format(const volatile std::atomic<T>& atomic, format_context& context)
		const noexcept
	{
		auto value = atomic.load(readOrder);
		format_context::iterator out = format_to(context.out(), "Atomic({})", value);
		atomic.store(value, writeOrder);
		return out;
	}

	std::memory_order readOrder;
	std::memory_order writeOrder;
};

export template<typename T>
struct std::formatter<std::atomic<T>, wchar_t>
{
	constexpr wformat_parse_context::iterator
		parse(wformat_parse_context& context)
	{
		auto it = context.begin();
		auto end = context.end();
		if (it == end or *it != L'{')
		{
			throw std::format_error{ "Invalid format string." };
		}

		if (*it != L'{')
		{
			throw std::format_error{ "Missing '{' in format string." };
		}

		if (++it == end)
		{
			throw std::format_error{ "Missing '}' in format string." };
		}

		if (it != end and *it == L'}')
		{
			readOrder = std::memory_order_seq_cst;
			writeOrder = std::memory_order_seq_cst;
			return it;
		}

		readOrder = ParseOrderSpec(*it);

		if (++it == end)
		{
			throw std::format_error{ "Missing '}' in format string." };
		}

		if (*it == L'}')
		{
			writeOrder = readOrder;
		}

		writeOrder = ParseOrderSpec(*(it++));

		if (it == end or *it != L'}')
		{
			throw std::format_error{ "Missing '}' in format string." };
		}

		return it;
	}

	wformat_context::iterator
		format(const std::atomic<T>& atomic, wformat_context& context)
		const noexcept
	{
		auto value = atomic.load(readOrder);
		wformat_context::iterator out = format_to(context.out(), L"Atomic({})", value);
		atomic.store(value, writeOrder);
		return out;
	}

	wformat_context::iterator
		format(const volatile std::atomic<T>& atomic, wformat_context& context)
		const noexcept
	{
		auto value = atomic.load(readOrder);
		format_context::iterator out = format_to(context.out(), L"Atomic({})", value);
		atomic.store(value, writeOrder);
		return out;
	}

	std::memory_order readOrder;
	std::memory_order writeOrder;
};
