export module Iconer.Utility.Annihilator;
import <typeinfo>;
import <cstring>;
import <string>;
import <format>;
import <memory>;

export namespace iconer::util
{
	template<typename Deallocator>
	struct Annihilator
	{
		constexpr ~Annihilator()
			noexcept(noexcept(std::declval<Deallocator>().deallocate(std::declval<std::allocator_traits<Deallocator>::pointer>(), size_t{})))
		{
			if (ptrHandle != nullptr)
			{
				std::destroy_n(ptrHandle, ptrSize);

				Deallocator allocator{};
				allocator.deallocate(ptrHandle, ptrSize);
			}
		}

		std::allocator_traits<Deallocator>::pointer ptrHandle;
		size_t ptrSize;
	};

	template<typename Deallocator>
	struct FailsafeAnnihilator
	{
		constexpr ~FailsafeAnnihilator()
			noexcept(noexcept(std::declval<Deallocator>().deallocate(std::declval<std::allocator_traits<Deallocator>::pointer>(), size_t{})))
		{
			if (not isSafe and ptrHandle != nullptr)
			{
				std::destroy_n(ptrHandle, ptrSize);

				Deallocator allocator{};
				allocator.deallocate(ptrHandle, ptrSize);
			}
		}

		constexpr void MakeSafe() noexcept
		{
			isSafe = true;
		}

		constexpr void MakeSafe() volatile noexcept
		{
			isSafe = true;
		}

		std::allocator_traits<Deallocator>::pointer ptrHandle;
		size_t ptrSize;
		volatile bool isSafe;
	};
}

export namespace std
{
	template<typename Deallocator>
	[[nodiscard]]
	string to_string(const iconer::util::Annihilator<Deallocator>& annihilator)
	{
		constexpr auto&& description = "Annihilator<>[]";
		static auto&& meta = typeid(Deallocator);
		static auto&& type_meta = typeid(typename allocator_traits<Deallocator>::value_type);

		string result{};
		result.reserve(sizeof(description) + strlen(meta.name()) + strlen(type_meta.name()));
		result.append("Annihilator<");
		result.append(meta.name());
		result.append(">[");
		result.append(type_meta.name());
		result.push_back(']');

		return result;
	}

	template<typename Deallocator>
	[[nodiscard]]
	string to_string(const iconer::util::FailsafeAnnihilator<Deallocator>& annihilator)
	{
		constexpr auto&& description = "FailsafeAnnihilator<>[]";
		static auto&& meta = typeid(Deallocator);
		static auto&& type_meta = typeid(typename allocator_traits<Deallocator>::value_type);

		string result{};
		result.reserve(sizeof(description) + strlen(meta.name()) + strlen(type_meta.name()));
		result.append("FailsafeAnnihilator<");
		result.append(meta.name());
		result.append(">[");
		result.append(type_meta.name());
		result.push_back(']');

		return result;
	}
}

export template<typename Deallocator>
struct std::formatter<iconer::util::Annihilator<Deallocator>, char>
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

	format_context::iterator
		format(const iconer::util::Annihilator<Deallocator>& annihilator, format_context& context)
	{
		static auto&& meta = typeid(Deallocator);

		if (nullptr == annihilator.ptrHandle)
		{
			return format_to(context.out(), "Annihilator<{}>({})[{}]", meta.name(), nullptr, annihilator.ptrSize);
		}
		else
		{
			return format_to(context.out(), "Annihilator<{}>({})[{}]", meta.name(), annihilator.ptrHandle, annihilator.ptrSize);
		}
	}
};

export template<typename Deallocator>
struct std::formatter<iconer::util::Annihilator<Deallocator>, wchar_t>
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

	wformat_context::iterator
		format(const iconer::util::Annihilator<Deallocator>& annihilator, wformat_context& context)
	{
		static auto&& meta = typeid(Deallocator);

		if (nullptr == annihilator.ptrHandle)
		{
			return format_to(context.out(), L"Annihilator<{}>({})[{}]", meta.name(), nullptr, annihilator.ptrSize);
		}
		else
		{
			return format_to(context.out(), L"Annihilator<{}>({})[{}]", meta.name(), annihilator.ptrHandle, annihilator.ptrSize);
		}
	}
};

export template<typename Deallocator>
struct std::formatter<iconer::util::FailsafeAnnihilator<Deallocator>, char>
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

	format_context::iterator
		format(const iconer::util::FailsafeAnnihilator<Deallocator>& annihilator, format_context& context)
	{
		static auto&& meta = typeid(Deallocator);

		if (nullptr == annihilator.ptrHandle)
		{
			return format_to(context.out(), "FailsafeAnnihilator<{}>({})[{}]", meta.name(), nullptr, annihilator.ptrSize);
		}
		else
		{
			return format_to(context.out(), "FailsafeAnnihilator<{}>({})[{}]", meta.name(), annihilator.ptrHandle, annihilator.ptrSize);
		}
	}
};

export template<typename Deallocator>
struct std::formatter<iconer::util::FailsafeAnnihilator<Deallocator>, wchar_t>
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

	wformat_context::iterator
		format(const iconer::util::FailsafeAnnihilator<Deallocator>& annihilator, wformat_context& context)
	{
		static auto&& meta = typeid(Deallocator);

		if (nullptr == annihilator.ptrHandle)
		{
			return format_to(context.out(), L"FailsafeAnnihilator<{}>({})[{}]", meta.name(), nullptr, annihilator.ptrSize);
		}
		else
		{
			return format_to(context.out(), L"FailsafeAnnihilator<{}>({})[{}]", meta.name(), annihilator.ptrHandle, annihilator.ptrSize);
		}
	}
};
