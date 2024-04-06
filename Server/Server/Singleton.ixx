export module Iconer.Utility.Singleton;
import Iconer.Utility.Constraints;
import <typeinfo>;
import <exception>;
import <string>;
import <format>;

export namespace iconer::util
{
	class bad_singleton_creation : public std::exception
	{
	public:
		bad_singleton_creation() noexcept = default;

		[[nodiscard]]
		const char* __CLR_OR_THIS_CALL what() const noexcept override
		{
			return "bad singleton initialization";
		}
	};

	class bad_singleton_access : public std::exception
	{
	public:
		bad_singleton_access() noexcept = default;

		[[nodiscard]]
		const char* __CLR_OR_THIS_CALL what() const noexcept override
		{
			return "bad singleton access";
		}
	};

	template<crtp T>
	class Singleton
	{
	public:
		constexpr ~Singleton() noexcept = default;

		constexpr Singleton(T* instance)
		{
			if (myInstance == nullptr)
			{
				myInstance = instance;
			}
			else
			{
				throw bad_singleton_creation{};
			}
		}

		[[nodiscard]]
		constexpr static T* Handle() noexcept
		{
			return myInstance;
		}

		[[nodiscard]]
		constexpr static T& Instance() noexcept
		{
			if (myInstance == nullptr)
			{
				throw bad_singleton_access{};
			}

			return myInstance;
		}

		constexpr bool operator==(const Singleton&) const noexcept = default;

	protected:
		constexpr Singleton(const Singleton&) noexcept = default;
		constexpr Singleton& operator=(const Singleton&) noexcept = default;
		constexpr Singleton(Singleton&&) noexcept = default;
		constexpr Singleton& operator=(Singleton&&) noexcept = default;

		static inline constinit T* myInstance = nullptr;
	};
}

export namespace std
{
	template<typename T>
	[[nodiscard]]
	string to_string(const iconer::util::Singleton<T>& single)
	{
		return to_string(iconer::util::Singleton<T>::Instance());
	}

	template<typename T>
	[[nodiscard]]
	wstring to_wstring(const iconer::util::Singleton<T>& single)
	{
		return to_wstring(iconer::util::Singleton<T>::Instance());
	}
}

export template<typename T>
struct std::formatter<iconer::util::Singleton<T>, char>
{
	static format_parse_context::iterator
		parse(format_parse_context& context)
		noexcept
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
		format(const iconer::util::Singleton<T>& single, format_context& context)
		noexcept
	{
		static auto&& meta = typeid(T);

		if (nullptr == iconer::util::Singleton<T>::Handle())
		{
			return format_to(context.out(), "Singleton<{}>(Empty)", meta.name());
		}
		else
		{
			return format_to(context.out(), "Singleton<{}>({})", meta.name(), iconer::util::Singleton<T>::Instance());
		}
	}
};

export template<typename T>
struct std::formatter<iconer::util::Singleton<T>, wchar_t>
{
	static wformat_parse_context::iterator
		parse(wformat_parse_context& context)
		noexcept
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
		format(const iconer::util::Singleton<T>& single, wformat_context& context)
		noexcept
	{
		static auto&& meta = typeid(T);

		if (nullptr == iconer::util::Singleton<T>::Handle())
		{
			return format_to(context.out(), L"Singleton<{}>(Empty)", meta.name());
		}
		else
		{
			return format_to(context.out(), L"Singleton<{}>({})", meta.name(), iconer::util::Singleton<T>::Instance());
		}
	}
};
