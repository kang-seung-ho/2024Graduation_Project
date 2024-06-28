export module Iconer.Utility.Expected;
export import <expected>;

export namespace iconer::util
{
	template<typename T, typename Err>
	using Expected = std::expected<T, Err>;
	template<typename Err>
	using Unexpected = std::unexpected<Err>;

	using std::unexpect_t;
	using std::unexpect;

	template<typename Err>
	using BadExpectedAccessError = std::bad_expected_access<Err>;
}
