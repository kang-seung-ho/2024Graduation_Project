module;
#include <typeinfo>
#include <utility>
#include <string>
#include <format>

export module Iconer.Utility.Handler;
import Iconer.Utility.Constraints;

export namespace iconer::util
{
	template<typename H>
	class [[nodiscard]] Handler
	{
	public:
		using HandleType = H;

		constexpr Handler()
			noexcept(nothrow_default_constructibles<H>) requires default_initializables<H> = default;
		constexpr ~Handler()
			noexcept(nothrow_destructibles<H>) = default;

		explicit(not actual_integral<H>)
			constexpr Handler(const H& handle)
			noexcept(nothrow_copy_constructibles<H>) requires copy_constructible<H>
			: myHandle(handle)
		{
		}

		explicit(not actual_integral<H>)
			constexpr Handler(H&& handle)
			noexcept(nothrow_move_constructibles<H>) requires move_constructible<H>
			: myHandle(std::move(handle))
		{
		}

		constexpr Handler& operator=(const H& handle)
			noexcept(nothrow_copy_assignables<H>)
		{
			myHandle = handle;
			return *this;
		}

		constexpr Handler& operator=(H&& handle)
			noexcept(nothrow_move_assignables<H>)
		{
			myHandle = std::move(handle);
			return *this;
		}
		
		constexpr volatile Handler& operator=(const H& handle)
			volatile noexcept(nothrow_copy_assignables<H>)
		{
			myHandle = handle;
			return *this;
		}

		constexpr volatile Handler& operator=(H&& handle)
			volatile noexcept(nothrow_move_assignables<H>)
		{
			myHandle = std::move(handle);
			return *this;
		}

		constexpr Handler& operator=(const volatile H& handle)
			noexcept(nothrow_copy_assignables<H>)
		{
			myHandle = handle;
			return *this;
		}

		constexpr Handler& operator=(volatile H&& handle)
			noexcept(nothrow_move_assignables<H>)
		{
			myHandle = std::move(handle);
			return *this;
		}

		constexpr volatile Handler& operator=(const volatile H& handle)
			volatile noexcept(nothrow_copy_assignables<H>)
		{
			myHandle = handle;
			return *this;
		}

		constexpr volatile Handler& operator=(volatile H&& handle)
			volatile noexcept(nothrow_move_assignables<H>)
		{
			myHandle = std::move(handle);
			return *this;
		}

		[[nodiscard]]
		constexpr H& GetHandle() & noexcept
		{
			return myHandle;
		}

		[[nodiscard]]
		constexpr volatile H& GetHandle() volatile& noexcept
		{
			return myHandle;
		}

		[[nodiscard]]
		constexpr const H& GetHandle() const& noexcept
		{
			return myHandle;
		}

		[[nodiscard]]
		constexpr const volatile H& GetHandle() const volatile& noexcept
		{
			return myHandle;
		}

		[[nodiscard]]
		constexpr H&& GetHandle() && noexcept
		{
			return static_cast<H&&>(myHandle);
		}

		[[nodiscard]]
		constexpr volatile H&& GetHandle() volatile&& noexcept
		{
			return static_cast<volatile H&&>(myHandle);
		}

		[[nodiscard]]
		constexpr bool operator==(const Handler&) const noexcept = default;

		constexpr Handler(const Handler&)
			noexcept(nothrow_copy_constructibles<H>) requires copy_constructible<H> = default;
		constexpr Handler(Handler&&)
			noexcept(nothrow_move_constructibles<H>) requires move_constructible<H> = default;
		constexpr Handler& operator=(const Handler&)
			noexcept(nothrow_copy_assignables<H>) requires copy_assignables<H> = default;
		constexpr Handler& operator=(Handler&&)
			noexcept(nothrow_move_assignables<H>) requires move_assignables<H> = default;

	protected:
		H myHandle;
	};
}

export namespace std
{
	template<typename H>
	[[nodiscard]]
	string to_string(const iconer::util::Handler<H>& handler)
	{
		if constexpr (std::convertible_to<H, std::string>)
		{
			return static_cast<std::string>(handler.GetHandle());
		}
		else
		{
			return std::to_string(handler.GetHandle());
		}
	}

	template<typename H>
	[[nodiscard]]
	string to_string(iconer::util::Handler<H>&& handler)
	{
		if constexpr (std::convertible_to<H, std::string>)
		{
			return static_cast<std::string>(std::move(handler).GetHandle());
		}
		else
		{
			return std::to_string(std::move(handler).GetHandle());
		}
	}

	template<typename H>
	[[nodiscard]]
	wstring to_wstring(const iconer::util::Handler<H>& handler)
	{
		if constexpr (std::convertible_to<H, std::wstring>)
		{
			return static_cast<std::wstring>(handler.GetHandle());
		}
		else
		{
			return std::to_wstring(handler.GetHandle());
		}
	}

	template<typename H>
	[[nodiscard]]
	wstring to_wstring(iconer::util::Handler<H>&& handler)
	{
		if constexpr (std::convertible_to<H, std::wstring>)
		{
			return static_cast<std::wstring>(std::move(handler).GetHandle());
		}
		else
		{
			return std::to_wstring(std::move(handler).GetHandle());
		}
	}
}

export template<typename H>
struct std::formatter<iconer::util::Handler<H>, char>
{
	static constexpr format_parse_context::iterator
		parse(format_parse_context& context)
	{
		auto it = context.begin();
		const auto end = context.end();
		if (it == end or *it != '{')
		{
			throw std::format_error{ "Invalid format string." };
		}

		++it;
		if (it != end and *it != '}')
		{
			throw std::format_error{ "Missing '}' in format string." };
		}

		return it;
	}

	static format_context::iterator
		format(const iconer::util::Handler<H>& handler, format_context& context)
		noexcept
	{
		static auto&& meta = typeid(H);

		return format_to(context.out(), "Handler<{}>({})", meta.name(), to_string(handler.GetHandle()));
	}

	static format_context::iterator
		format(iconer::util::Handler<H>&& handler, format_context& context)
		noexcept
	{
		static auto&& meta = typeid(H);

		return format_to(context.out(), "Handler<{}>({})", meta.name(), to_string(std::move(handler).GetHandle()));
	}
};

export template<typename H>
struct std::formatter<iconer::util::Handler<H>, wchar_t>
{
	static constexpr wformat_parse_context::iterator
		parse(wformat_parse_context& context)
	{

		auto it = context.begin();
		const auto end = context.end();
		if (it == end or *it != L'{')
		{
			throw std::format_error{ "Invalid format string." };
		}

		++it;
		if (it == end or *it != L'}')
		{
			throw std::format_error{ "Missing '}' in format string." };
		}

		return it;
	}

	static wformat_context::iterator
		format(const iconer::util::Handler<H>& handler, wformat_context& context)
		noexcept
	{
		static auto&& meta = typeid(H);

		return format_to(context.out(), L"Handler<{}>({})", meta.name(), to_wstring(handler.GetHandle()));
	}

	static wformat_context::iterator
		format(iconer::util::Handler<H>&& handler, wformat_context& context)
		noexcept
	{
		static auto&& meta = typeid(H);

		return format_to(context.out(), L"Handler<{}>({})", meta.name(), to_wstring(std::move(handler).GetHandle()));
	}
};
