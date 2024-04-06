export module Iconer.Utility.Property;
import Iconer.Utility.Constraints;
import :IProperty;
import <typeinfo>;
import <concepts>;
import <string>;
import <format>;

export namespace iconer::util
{
	template<notvoids T>
	using Property = IProperty<T, void>;

	template<notvoids T, notvoids Predicate>
	using CustomProperty = IProperty<T, Predicate>;

	template<typename T, typename Predicate>
	IProperty(T, Predicate) -> IProperty<T, Predicate>;
	template<typename T>
	IProperty(T) -> IProperty<T, void>;
}

export namespace std
{
	template<typename T, typename Predicate>
	[[nodiscard]]
	constexpr
		string
		to_string(const iconer::util::IProperty<T, Predicate>& property)
	{
		if constexpr (std::convertible_to<T, std::string>)
		{
			return static_cast<std::string>(property);
		}
		else
		{
			return std::to_string(static_cast<const T&>(property));
		}
	}

	template<typename T, typename Predicate>
	[[nodiscard]]
	constexpr
		string
		to_string(iconer::util::IProperty<T, Predicate>& property)
	{
		if constexpr (std::convertible_to<T, std::string>)
		{
			return static_cast<std::string>(property);
		}
		else
		{
			return std::to_string(static_cast<T&>(property));
		}
	}

	template<typename T, typename Predicate>
	[[nodiscard]]
	constexpr
		string
		to_string(const iconer::util::IProperty<T, Predicate>&& property)
	{
		if constexpr (std::convertible_to<T, std::string>)
		{
			return static_cast<std::string>(property);
		}
		else
		{
			return std::to_string(static_cast<const T&&>(property));
		}
	}

	template<typename T, typename Predicate>
	[[nodiscard]]
	constexpr
		string
		to_string(iconer::util::IProperty<T, Predicate>&& property)
	{
		if constexpr (std::convertible_to<T, std::string>)
		{
			return static_cast<std::string>(property);
		}
		else
		{
			return std::to_string(static_cast<T&&>(property));
		}
	}

	template<typename T, typename Predicate>
	[[nodiscard]]
	constexpr
		wstring
		to_wstring(iconer::util::IProperty<T, Predicate>& property)
	{
		if constexpr (std::convertible_to<T, std::wstring>)
		{
			return static_cast<std::wstring>(property);
		}
		else
		{
			return std::to_wstring(static_cast<T&>(property));
		}
	}

	template<typename T, typename Predicate>
	[[nodiscard]]
	constexpr
		wstring
		to_wstring(const iconer::util::IProperty<T, Predicate>& property)
	{
		if constexpr (std::convertible_to<T, std::wstring>)
		{
			return static_cast<std::wstring>(property);
		}
		else
		{
			return std::to_wstring(static_cast<const T&>(property));
		}
	}

	template<typename T, typename Predicate>
	[[nodiscard]]
	constexpr
		wstring
		to_wstring(iconer::util::IProperty<T, Predicate>&& property)
	{
		if constexpr (std::convertible_to<T, std::wstring>)
		{
			return static_cast<std::wstring>(property);
		}
		else
		{
			return std::to_wstring(static_cast<T&&>(property));
		}
	}

	template<typename T, typename Predicate>
	[[nodiscard]]
	constexpr
		wstring
		to_wstring(const iconer::util::IProperty<T, Predicate>&& property)
	{
		if constexpr (std::convertible_to<T, std::wstring>)
		{
			return static_cast<std::wstring>(property);
		}
		else
		{
			return std::to_wstring(static_cast<const T&&>(property));
		}
	}
}

export template<typename T, typename Predicate>
struct std::formatter<iconer::util::IProperty<T, Predicate>, char>
	: protected std::formatter<T, char>
{
	using property_t = iconer::util::IProperty<T, Predicate>;

	using std::formatter<T, char>::parse;

	format_context::iterator
		format(const property_t& property, format_context& context) const
	{
		static auto&& meta = typeid(T);

		if constexpr (iconer::notvoids<Predicate>)
		{
			return format_to(context.out(), "CustomProperty<{}>({})", meta.name(), static_cast<const T&>(property));
		}
		else
		{
			return format_to(context.out(), "Property<{}>({})", meta.name(), static_cast<const T&>(property));
		}
	}

	format_context::iterator
		format(property_t&& property, format_context& context) const
	{
		static auto&& meta = typeid(T);

		if constexpr (iconer::notvoids<Predicate>)
		{
			return format_to(context.out(), "CustomProperty<{}>({})", meta.name(), static_cast<T&&>(std::move(property)));
		}
		else
		{
			return format_to(context.out(), "Property<{}>({})", meta.name(), static_cast<T&&>(std::move(property)));
		}
	}
};

export template<typename T, typename Predicate>
struct std::formatter<iconer::util::IProperty<T, Predicate>, wchar_t>
	: protected std::formatter<T, wchar_t>
{
	using property_t = iconer::util::IProperty<T, Predicate>;

	using std::formatter<T, wchar_t>::parse;

	wformat_context::iterator
		format(const property_t& property, wformat_context& context) const
	{
		static auto&& meta = typeid(T);
		auto name = reinterpret_cast<const wchar_t*>(meta.name());

		if constexpr (iconer::notvoids<Predicate>)
		{
			return format_to(context.out(), L"CustomProperty<{}>({})", name, static_cast<const T&>(property));
		}
		else
		{
			return format_to(context.out(), L"Property<{}>({})", name, static_cast<const T&>(property));
		}
	}

	wformat_context::iterator
		format(property_t&& property, wformat_context& context) const
	{
		static auto&& meta = typeid(T);
		auto name = reinterpret_cast<const wchar_t*>(meta.name());

		if constexpr (iconer::notvoids<Predicate>)
		{
			return format_to(context.out(), L"CustomProperty<{}>({})", name, static_cast<T&&>(std::move(property)));
		}
		else
		{
			return format_to(context.out(), L"Property<{}>({})", name, static_cast<T&&>(std::move(property)));
		}
	}
};
