export module Iconer.Collection.Concurrent.Stack;
import <string>;
import <format>;

export namespace iconer::collection
{
	class ConcurrentStack
	{
	public:
		explicit constexpr ConcurrentStack() noexcept = default;
		~ConcurrentStack();

		[[nodiscard]] constexpr bool operator==(const ConcurrentStack&) const noexcept = default;

	private:
	};
}

export namespace std
{
	[[nodiscard]] string to_string(const iconer::collection::ConcurrentStack& stack);
	[[nodiscard]] string to_string(const volatile iconer::collection::ConcurrentStack& stack);
}

export template<>
struct std::formatter<iconer::collection::ConcurrentStack, char>
{
	static format_parse_context::iterator
		parse(format_parse_context& context)
		noexcept;

	static format_context::iterator
		format(const iconer::collection::ConcurrentStack& stack, format_context& context)
		noexcept;
};

export template<>
struct std::formatter<iconer::collection::ConcurrentStack, wchar_t>
{
	static wformat_parse_context::iterator
		parse(wformat_parse_context& context)
		noexcept;

	static wformat_context::iterator
		format(const iconer::collection::ConcurrentStack& stack, wformat_context& context)
		noexcept;
};
