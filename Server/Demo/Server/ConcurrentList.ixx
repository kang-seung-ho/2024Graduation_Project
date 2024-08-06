export module Iconer.Collection.Concurrent.List;
import <string>;
import <format>;

export namespace iconer::collection
{
	class ConcurrentList
	{
	public:
		explicit constexpr ConcurrentList() noexcept = default;
		~ConcurrentList();

		[[nodiscard]] constexpr bool operator==(const ConcurrentList&) const noexcept = default;

	private:
	};
}

export namespace std
{
	[[nodiscard]] string to_string(const iconer::collection::ConcurrentList& list);
	[[nodiscard]] string to_string(const volatile iconer::collection::ConcurrentList& list);
}

export template<>
struct std::formatter<iconer::collection::ConcurrentList, char>
{
	static format_parse_context::iterator
		parse(format_parse_context& context)
		noexcept;

	static format_context::iterator
		format(const iconer::collection::ConcurrentList& list, format_context& context)
		noexcept;
};

export template<>
struct std::formatter<iconer::collection::ConcurrentList, wchar_t>
{
	static wformat_parse_context::iterator
		parse(wformat_parse_context& context)
		noexcept;

	static wformat_context::iterator
		format(const iconer::collection::ConcurrentList& list, wformat_context& context)
		noexcept;
};
