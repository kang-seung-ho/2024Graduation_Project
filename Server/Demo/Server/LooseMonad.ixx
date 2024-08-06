export module Iconer.Utility.LooseMonad;
import <string>;
import <format>;

export namespace iconer::util
{
	class LooseMonad
	{
	public:
		constexpr LooseMonad() noexcept = default;
		constexpr ~LooseMonad() noexcept = default;

		constexpr bool operator==(const LooseMonad&) const noexcept = default;

	private:
	};
}

export namespace std
{
	[[nodiscard]] string to_string(const iconer::util::LooseMonad& monad);
}

export template<>
struct std::formatter<iconer::util::LooseMonad, char>
{
	static format_parse_context::iterator
		parse(format_parse_context& context)
		noexcept;

	static format_context::iterator
		format(const iconer::util::LooseMonad& monad, format_context& context)
		noexcept;
};

export template<>
struct std::formatter<iconer::util::LooseMonad, wchar_t>
{
	static wformat_parse_context::iterator
		parse(wformat_parse_context& context)
		noexcept;

	static wformat_context::iterator
		format(const iconer::util::LooseMonad& monad, wformat_context& context)
		noexcept;
};
