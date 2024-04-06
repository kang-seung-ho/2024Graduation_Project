export module Iconer.Utility.TypeList;
import Iconer.Utility.TypeTraits;
import <typeinfo>;
import <tuple>;
import <string>;
import <string_view>;
import <format>;
import <ranges>;

export namespace iconer::util
{
	template<typename... Ts>
	struct [[nodiscard]] TypeList
	{
		explicit constexpr TypeList() noexcept = default;
		constexpr ~TypeList() noexcept = default;

		[[nodiscard]] constexpr bool operator==(const TypeList&) const noexcept = default;
	};
}

export template<size_t I, typename... Ts>
struct std::tuple_element<I, iconer::util::TypeList<Ts...>>
	: public std::tuple_element<I, std::tuple<Ts...>>
{};

export template<size_t I, typename... Ts>
struct std::tuple_element<I, const iconer::util::TypeList<Ts...>>
	: public std::tuple_element<I, std::tuple<Ts...>>
{};

export template<size_t I, typename... Ts>
struct std::tuple_element<I, volatile iconer::util::TypeList<Ts...>>
	: public std::tuple_element<I, std::tuple<Ts...>>
{};

export template<size_t I, typename... Ts>
struct std::tuple_element<I, const volatile iconer::util::TypeList<Ts...>>
	: public std::tuple_element<I, std::tuple<Ts...>>
{};

export template<typename... Ts>
struct std::tuple_size<iconer::util::TypeList<Ts...>> : public std::tuple_size<std::tuple<Ts...>>
{};

namespace
{
	template<typename T>
	[[nodiscard]]
	std::string_view GetName() noexcept
	{
		static auto&& meta = typeid(T);

		return meta.name();
	}

	template<typename T>
	[[nodiscard]]
	std::wstring_view GetWideName() noexcept
	{
		static auto&& meta = typeid(T);

		std::string_view narrow = GetName<T>();

		return std::wstring_view{ reinterpret_cast<const wchar_t*>(narrow.data()) };
	}
}

export namespace std
{
	template<typename... Ts>
	[[nodiscard]]
	constexpr string to_string(iconer::util::TypeList<Ts...>)
	{
		std::string result{};
		result.reserve(20 + iconer::byte_size_v<Ts...>);
		result.append("TypeList<");
		(result.append(::GetName<Ts>()), ...);
		result.push_back('>');

		return result;
	}

	template<typename... Ts>
	[[nodiscard]]
	constexpr wstring to_wstring(iconer::util::TypeList<Ts...>)
	{
		std::wstring result{};
		result.reserve(20 + iconer::byte_size_v<Ts...>);
		result.append(L"TypeList<");
		(result.append(::GetWideName<Ts>()), ...);
		result.push_back(L'>');

		return result;
	}
}

export template<typename... Ts>
struct std::formatter<iconer::util::TypeList<Ts...>, char>
{
	static format_parse_context::iterator
		parse(format_parse_context& context)
		noexcept
	{
		return context.begin();
	}

	static format_context::iterator
		format(iconer::util::TypeList<Ts...>, format_context& context)
		noexcept
	{
		return format_to(context.out(), "{}", to_string(iconer::util::TypeList<Ts...>{}));
	}
};

export template<typename... Ts>
struct std::formatter<iconer::util::TypeList<Ts...>, wchar_t>
{
	static wformat_parse_context::iterator
		parse(wformat_parse_context& context)
		noexcept
	{
		return context.begin();
	}

	static wformat_context::iterator
		format(const iconer::util::TypeList<Ts...>& list, wformat_context& context)
		noexcept
	{
		return format_to(context.out(), "{}", to_wstring(iconer::util::TypeList<Ts...>{}));
	}
};
