export module Iconer.Collection.FlatMap;
import <string>;
import <format>;

export namespace iconer::collection
{
	class FlatMap
	{
	public:
		explicit constexpr FlatMap() noexcept = default;
		~FlatMap();

		[[nodiscard]] constexpr bool operator==(const FlatMap&) const noexcept = default;

	private:
	};
}

export namespace std
{
	[[nodiscard]] string to_string(const iconer::collection::FlatMap& map);
}

export template<>
struct std::formatter<iconer::collection::FlatMap, char>
{
	static format_parse_context::iterator
		parse(format_parse_context& context)
		noexcept;

	static format_context::iterator
		format(const iconer::collection::FlatMap& map, format_context& context)
		noexcept;
};

export template<>
struct std::formatter<iconer::collection::FlatMap, wchar_t>
{
	static wformat_parse_context::iterator
		parse(wformat_parse_context& context)
		noexcept;

	static wformat_context::iterator
		format(const iconer::collection::FlatMap& map, wformat_context& context)
		noexcept;
};
