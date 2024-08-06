export module Iconer.Utility.Monad;
import <string>;
import <format>;

export namespace iconer::util
{
	class Monad
	{
	public:
		constexpr Monad() noexcept = default;
		constexpr ~Monad() noexcept = default;

		constexpr bool operator==(const Monad&) const noexcept = default;

	private:
	};
}

export namespace std
{
	[[nodiscard]] string to_string(const iconer::util::Monad& monad);
}

export template<>
struct std::formatter<iconer::util::Monad, char>
{
	static format_parse_context::iterator
		parse(format_parse_context& context)
		noexcept;

	static format_context::iterator
		format(const iconer::util::Monad& monad, format_context& context)
		noexcept;
};

export template<>
struct std::formatter<iconer::util::Monad, wchar_t>
{
	static wformat_parse_context::iterator
		parse(wformat_parse_context& context)
		noexcept;

	static wformat_context::iterator
		format(const iconer::util::Monad& monad, wformat_context& context)
		noexcept;
};
