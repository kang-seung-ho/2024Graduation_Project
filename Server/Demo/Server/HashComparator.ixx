export module Iconer.Collection.Concurrent.HashComparator;

export namespace iconer::collection
{
	// Template class for hash compare
	template<typename KeyType, typename _Hasher, typename _Key_equality>
	class HashComparator
	{
	public:
		using key_type = KeyType;
		using hasher = _Hasher;
		using comparator = _Key_equality;

		[[nodiscard]]
		constexpr size_t operator()(const key_type& _Keyval) const
		{
			return static_cast<size_t>(_M_hash_object(_Keyval));
		}

		[[nodiscard]]
		constexpr bool operator()(const key_type& lhs, const key_type& rhs) const
		{
			return (!_M_key_compare_object(lhs, rhs));
		}

		hasher     _M_hash_object;        // The hash object
		comparator _M_key_compare_object; // The equality comparator object
	};
}
