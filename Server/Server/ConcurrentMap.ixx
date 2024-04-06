module;
#include "internal_concurrent_hash.h"

export module Iconer.Collection.Concurrent.Map;
import Iconer.Utility.Constraints;
import Iconer.Collection.Concurrent.HashComparator;
import <concepts>;
import <utility>;

export namespace iconer::collection
{
	// Template class for hash map traits
	template<std::movable KeyType, std::movable ValueType, std::movable Comparator, std::movable _Allocator_type, bool _Allow_multimapping>
	class ConcurrentMapTrait : public ::std::_Container_base
	{
	public:
		using key_type = KeyType;
		using value_type = std::pair<const KeyType, ValueType>;
		using allocator_type = typename ::std::allocator_traits<_Allocator_type>::template rebind_alloc<value_type>;
		using _Key_compare = Comparator;

		static inline constexpr bool _M_allow_multimapping = _Allow_multimapping;

		constexpr ConcurrentMapTrait()
			: _M_comparator()
		{
		}

		constexpr ConcurrentMapTrait(const _Key_compare& comp)
			noexcept(nothrow_copy_constructibles<_Key_compare>)
			: _M_comparator(comp)
		{
		}
		
		constexpr ConcurrentMapTrait(_Key_compare&& comp)
			noexcept(nothrow_move_constructibles<_Key_compare>)
			: _M_comparator(std::move(comp))
		{
		}

		class _Value_compare
		{
			friend class ConcurrentMapTrait<KeyType, ValueType, Comparator, _Allocator_type, _Allow_multimapping>;

		public:
			constexpr _Value_compare(const Comparator& comp)
				noexcept(nothrow_copy_constructibles<Comparator>)
				: _M_comparator(comp)
			{
			}

			constexpr _Value_compare(Comparator&& comp)
				noexcept(nothrow_move_constructibles<Comparator>)
				: _M_comparator(std::move(comp))
			{
			}

			[[nodiscard]]
			constexpr bool operator()(const value_type& lhs, const value_type& rhs) const
			{
				return (_M_comparator(lhs.first, rhs.first));
			}

		protected:
			_Key_compare _M_comparator;    // the comparator predicate for keys
		};

		template<class _Type1, class _Type2>
		[[nodiscard]]
		static constexpr const _Type1& _Key_function(const ::std::pair<_Type1, _Type2>& _Value)
		{
			return _Value.first;
		}

		_Key_compare _M_comparator; // the comparator predicate for keys
	};

	template <typename _Key_type, typename ValueType, typename _Hasher = ::std::hash<_Key_type>, typename _Key_equality = ::std::equal_to<_Key_type>, typename _Allocator_type = ::std::allocator<::std::pair<const _Key_type, ValueType>>>
	class ConcurrentMap
		: public concurrency::details::_Concurrent_hash<ConcurrentMapTrait<_Key_type, ValueType, HashComparator<_Key_type, _Hasher, _Key_equality>, _Allocator_type, false>>
	{
	public:
		// Base type definitions
		typedef ConcurrentMap<_Key_type, ValueType, _Hasher, _Key_equality, _Allocator_type> _Mytype;
		typedef HashComparator<_Key_type, _Hasher, _Key_equality> _Key_compare;
		typedef concurrency::details::_Concurrent_hash<ConcurrentMapTrait<_Key_type, ValueType, _Key_compare, _Allocator_type, false>> _Mybase;

		/// <summary>
		///     The type of an ordering key.
		/// </summary>
		/**/
		typedef _Key_type key_type;

		/// <summary>
		///     The type of an element.
		/// </summary>
		/**/
		typedef typename _Mybase::value_type value_type;

		/// <summary>
		///     The type of a mapped value associated with each key.
		/// </summary>
		/**/
		typedef ValueType mapped_type;

		/// <summary>
		///     The type of the hash function.
		/// </summary>
		/**/
		typedef _Hasher hasher;

		/// <summary>
		///     The type of the comparison function.
		/// </summary>
		/**/
		typedef _Key_equality key_equal;

		/// <summary>
		///     The type of an allocator for managing storage.
		/// </summary>
		/**/
		typedef typename _Mybase::allocator_type allocator_type;

		/// <summary>
		///     The type of a pointer to an element.
		/// </summary>
		/**/
		typedef typename _Mybase::pointer pointer;

		/// <summary>
		///     The type of a constant pointer to an element.
		/// </summary>
		/**/
		typedef typename _Mybase::const_pointer const_pointer;

		/// <summary>
		///     The type of a reference to an element.
		/// </summary>
		/**/
		typedef typename _Mybase::reference reference;

		/// <summary>
		///     The type of a constant reference to an element.
		/// </summary>
		/**/
		typedef typename _Mybase::const_reference const_reference;

		/// <summary>
		///     The type of an unsigned distance between two elements.
		/// </summary>
		/**/
		typedef typename _Mybase::size_type size_type;

		/// <summary>
		///     The type of a signed distance between two elements.
		/// </summary>
		/**/
		typedef typename _Mybase::difference_type difference_type;

		/// <summary>
		///     The type of an iterator for the controlled sequence.
		/// </summary>
		/**/
		typedef typename _Mybase::iterator iterator;

		/// <summary>
		///     The type of a constant iterator for the controlled sequence.
		/// </summary>
		/**/
		typedef typename _Mybase::const_iterator const_iterator;

		/// <summary>
		///     The type of a bucket iterator for the controlled sequence.
		/// </summary>
		/**/
		typedef typename _Mybase::iterator local_iterator;

		/// <summary>
		///     The type of a constant bucket iterator for the controlled sequence.
		/// </summary>
		/**/
		typedef typename _Mybase::const_iterator const_local_iterator;

		/// <summary>
		///     Constructs a concurrent unordered map.
		/// </summary>
		/// <param name="_Number_of_buckets">
		///     The initial number of buckets for this unordered map.
		/// </param>
		/// <param name="_Hasharg">
		///     The hash function for this unordered map.
		/// </param>
		/// <param name="_Keyeqarg">
		///     The equality comparison function for this unordered map.
		/// </param>
		/// <param name="_Allocator">
		///     The allocator for this unordered map.
		/// </param>
		/// <remarks>
		///     All constructors store an allocator object <paramref name="_Allocator"/> and initialize the unordered map.
		///     <para>The first constructor specifies an empty initial map and explicitly specifies the number of buckets,
		///     hash function, equality function and allocator type to be used.</para>
		///     <para>The second constructor specifies an allocator for the unordered map.</para>
		///     <para>The third constructor specifies values supplied by the iterator range [<paramref name="_Begin"/>, <paramref name="_End"/>).</para>
		///     <para>The fourth and fifth constructors specify a copy of the concurrent unordered map <paramref name="_Umap"/>.</para>
		///     <para>The last constructor specifies a move of the concurrent unordered map <paramref name="_Umap"/>.</para>
		/// </remarks>
		/**/
		explicit ConcurrentMap(size_type _Number_of_buckets = 8, const hasher& _Hasharg = hasher(), const key_equal& _Keyeqarg = key_equal(),
			const allocator_type& _Allocator = allocator_type())
			: _Mybase(_Number_of_buckets, _Key_compare(_Hasharg, _Keyeqarg), _Allocator)
		{
			this->rehash(_Number_of_buckets);
		}

		/// <summary>
		///     Constructs a concurrent unordered map.
		/// </summary>
		/// <param name="_Allocator">
		///     The allocator for this unordered map.
		/// </param>
		/// <remarks>
		///     All constructors store an allocator object <paramref name="_Allocator"/> and initialize the unordered map.
		///     <para>The first constructor specifies an empty initial map and explicitly specifies the number of buckets,
		///     hash function, equality function and allocator type to be used.</para>
		///     <para>The second constructor specifies an allocator for the unordered map.</para>
		///     <para>The third constructor specifies values supplied by the iterator range [<paramref name="_Begin"/>, <paramref name="_End"/>).</para>
		///     <para>The fourth and fifth constructors specify a copy of the concurrent unordered map <paramref name="_Umap"/>.</para>
		///     <para>The last constructor specifies a move of the concurrent unordered map <paramref name="_Umap"/>.</para>
		/// </remarks>
		/**/
		ConcurrentMap(const allocator_type& _Allocator) : _Mybase(8, _Key_compare(), _Allocator)
		{
		}

		/// <summary>
		///     Constructs a concurrent unordered map.
		/// </summary>
		/// <typeparam name="_Iterator">
		///     The type of the input iterator.
		/// </typeparam>
		/// <param name="_Begin">
		///     The position of the first element in the range of elements to be copied.
		/// </param>
		/// <param name="_End">
		///     The position of the first element beyond the range of elements to be copied.
		/// </param>
		/// <param name="_Number_of_buckets">
		///     The initial number of buckets for this unordered map.
		/// </param>
		/// <param name="_Hasharg">
		///     The hash function for this unordered map.
		/// </param>
		/// <param name="_Keyeqarg">
		///     The equality comparison function for this unordered map.
		/// </param>
		/// <param name="_Allocator">
		///     The allocator for this unordered map.
		/// </param>
		/// <remarks>
		///     All constructors store an allocator object <paramref name="_Allocator"/> and initialize the unordered map.
		///     <para>The first constructor specifies an empty initial map and explicitly specifies the number of buckets,
		///     hash function, equality function and allocator type to be used.</para>
		///     <para>The second constructor specifies an allocator for the unordered map.</para>
		///     <para>The third constructor specifies values supplied by the iterator range [<paramref name="_Begin"/>, <paramref name="_End"/>).</para>
		///     <para>The fourth and fifth constructors specify a copy of the concurrent unordered map <paramref name="_Umap"/>.</para>
		///     <para>The last constructor specifies a move of the concurrent unordered map <paramref name="_Umap"/>.</para>
		/// </remarks>
		/**/
		template <typename _Iterator>
		ConcurrentMap(_Iterator _Begin, _Iterator _End, size_type _Number_of_buckets = 8, const hasher& _Hasharg = hasher(),
			const key_equal& _Keyeqarg = key_equal(), const allocator_type& _Allocator = allocator_type())
			: _Mybase(_Number_of_buckets, _Key_compare(_Hasharg, _Keyeqarg), _Allocator)
		{
			this->rehash(_Number_of_buckets);
			for (; _Begin != _End; ++_Begin)
			{
				this->_Insert(*_Begin);
			}
		}

		/// <summary>
		///     Constructs a concurrent unordered map.
		/// </summary>
		/// <param name="_Umap">
		///     The source <c>ConcurrentMap</c> object to copy or move elements from.
		/// </param>
		/// <remarks>
		///     All constructors store an allocator object <paramref name="_Allocator"/> and initialize the unordered map.
		///     <para>The first constructor specifies an empty initial map and explicitly specifies the number of buckets,
		///     hash function, equality function and allocator type to be used.</para>
		///     <para>The second constructor specifies an allocator for the unordered map.</para>
		///     <para>The third constructor specifies values supplied by the iterator range [<paramref name="_Begin"/>, <paramref name="_End"/>).</para>
		///     <para>The fourth and fifth constructors specify a copy of the concurrent unordered map <paramref name="_Umap"/>.</para>
		///     <para>The last constructor specifies a move of the concurrent unordered map <paramref name="_Umap"/>.</para>
		/// </remarks>
		/**/
		ConcurrentMap(const ConcurrentMap& _Umap) : _Mybase(_Umap)
		{
		}

		/// <summary>
		///     Constructs a concurrent unordered map.
		/// </summary>
		/// <param name="_Umap">
		///     The source <c>ConcurrentMap</c> object to copy or move elements from.
		/// </param>
		/// <param name="_Allocator">
		///     The allocator for this unordered map.
		/// </param>
		/// <remarks>
		///     All constructors store an allocator object <paramref name="_Allocator"/> and initialize the unordered map.
		///     <para>The first constructor specifies an empty initial map and explicitly specifies the number of buckets,
		///     hash function, equality function and allocator type to be used.</para>
		///     <para>The second constructor specifies an allocator for the unordered map.</para>
		///     <para>The third constructor specifies values supplied by the iterator range [<paramref name="_Begin"/>, <paramref name="_End"/>).</para>
		///     <para>The fourth and fifth constructors specify a copy of the concurrent unordered map <paramref name="_Umap"/>.</para>
		///     <para>The last constructor specifies a move of the concurrent unordered map <paramref name="_Umap"/>.</para>
		/// </remarks>
		/**/
		ConcurrentMap(const ConcurrentMap& _Umap, const allocator_type& _Allocator) : _Mybase(_Umap, _Allocator)
		{
		}

		/// <summary>
		///     Constructs a concurrent unordered map.
		/// </summary>
		/// <param name="_Umap">
		///     The source <c>ConcurrentMap</c> object to copy or move elements from.
		/// </param>
		/// <remarks>
		///     All constructors store an allocator object <paramref name="_Allocator"/> and initialize the unordered map.
		///     <para>The first constructor specifies an empty initial map and explicitly specifies the number of buckets,
		///     hash function, equality function and allocator type to be used.</para>
		///     <para>The second constructor specifies an allocator for the unordered map.</para>
		///     <para>The third constructor specifies values supplied by the iterator range [<paramref name="_Begin"/>, <paramref name="_End"/>).</para>
		///     <para>The fourth and fifth constructors specify a copy of the concurrent unordered map <paramref name="_Umap"/>.</para>
		///     <para>The last constructor specifies a move of the concurrent unordered map <paramref name="_Umap"/>.</para>
		/// </remarks>
		/**/
		ConcurrentMap(ConcurrentMap&& _Umap) : _Mybase(::std::move(_Umap))
		{
		}

		/// <summary>
		///     Assigns the contents of another <c>ConcurrentMap</c> object to this one. This method is not concurrency-safe.
		/// </summary>
		/// <param name="_Umap">
		///     The source <c>ConcurrentMap</c> object.
		/// </param>
		/// <returns>
		///     A reference to this <c>ConcurrentMap</c> object.
		/// </returns>
		/// <remarks>
		///     After erasing any existing elements a concurrent vector, <c>operator=</c> either copies or moves the contents of <paramref name="_Umap"/> into
		///     the concurrent vector.
		/// </remarks>
		/**/
		ConcurrentMap& operator=(const ConcurrentMap& _Umap)
		{
			_Mybase::operator=(_Umap);
			return (*this);
		}

		/// <summary>
		///     Assigns the contents of another <c>ConcurrentMap</c> object to this one. This method is not concurrency-safe.
		/// </summary>
		/// <param name="_Umap">
		///     The source <c>ConcurrentMap</c> object.
		/// </param>
		/// <returns>
		///     A reference to this <c>ConcurrentMap</c> object.
		/// </returns>
		/// <remarks>
		///     After erasing any existing elements in a concurrent vector, <c>operator=</c> either copies or moves the contents of <paramref name="_Umap"/> into
		///     the concurrent vector.
		/// </remarks>
		/**/
		ConcurrentMap& operator=(ConcurrentMap&& _Umap)
		{
			_Mybase::operator=(::std::move(_Umap));
			return (*this);
		}

		/// <summary>
		///     Adds elements to the <c>ConcurrentMap</c> object.
		/// </summary>
		/// <param name="_Value">
		///     The value to be inserted.
		/// </param>
		/// <returns>
		///     A pair that contains an iterator and a boolean value. See the Remarks section for more details.
		/// </returns>
		/// <remarks>
		///     The first member function determines whether an element X exists in the sequence whose key has equivalent ordering to
		///     that of <paramref name="_Value"/>. If not, it creates such an element X and initializes it with <paramref name="_Value"/>.
		///     The function then determines the iterator <c>where</c> that designates X. If an insertion occurred, the function returns
		///     <c>std::pair(where, true)</c>. Otherwise, it returns <c>std::pair(where, false)</c>.
		///     <para>The second member function returns insert(<paramref name="_Value"/>), using <paramref name="_Where"/> as a starting
		///     place within the controlled sequence to search for the insertion point.</para>
		///     <para>The third member function inserts the sequence of element values from the range [<paramref name="_First"/>,
		///     <paramref name="_Last"/>).</para>
		///     <para>The last two member functions behave the same as the first two, except that <paramref name="_Value"/> is used to
		///     construct the inserted value.</para>
		/// </remarks>
		/**/
		::std::pair<iterator, bool> insert(const value_type& _Value)
		{
			return this->_Insert(_Value);
		}

		/// <summary>
		///     Adds elements to the <c>ConcurrentMap</c> object.
		/// </summary>
		/// <param name="_Where">
		///     The starting location to search for an insertion point.
		/// </param>
		/// <param name="_Value">
		///     The value to be inserted.
		/// </param>
		/// <returns>
		///     An iterator pointing to the insertion location of the object.  If the key already exists in the container
		///     an iterator pointing to the duplicate key location in the map is returned.
		/// </returns>
		/// <remarks>
		///     The first member function determines whether an element X exists in the sequence whose key has equivalent ordering to
		///     that of <paramref name="_Value"/>. If not, it creates such an element X and initializes it with <paramref name="_Value"/>.
		///     The function then determines the iterator <c>where</c> that designates X. If an insertion occurred, the function returns
		///     <c>std::pair(where, true)</c>. Otherwise, it returns <c>std::pair(where, false)</c>.
		///     <para>The second member function returns insert(<paramref name="_Value"/>), using <paramref name="_Where"/> as a starting
		///     place within the controlled sequence to search for the insertion point.</para>
		///     <para>The third member function inserts the sequence of element values from the range [<paramref name="_First"/>,
		///     <paramref name="_Last"/>).</para>
		///     <para>The last two member functions behave the same as the first two, except that <paramref name="_Value"/> is used to
		///     construct the inserted value.</para>
		/// </remarks>
		/**/
		iterator insert(const_iterator _Where, const value_type& _Value)
		{
			// Current implementation ignores the hint. The method is provided for compatibility with unordered_map.
			return this->_Insert(_Value).first;
		}

		/// <summary>
		///     Adds elements to the <c>ConcurrentMap</c> object.
		/// </summary>
		/// <typeparam name="_Iterator">
		///     The iterator type used for insertion.
		/// </typeparam>
		/// <param name="_First">
		///     The beginning of the range to insert.
		/// </param>
		/// <param name="_Last">
		///     The end of the range to insert.
		/// </param>
		/// <remarks>
		///     The first member function determines whether an element X exists in the sequence whose key has equivalent ordering to
		///     that of <paramref name="_Value"/>. If not, it creates such an element X and initializes it with <paramref name="_Value"/>.
		///     The function then determines the iterator <c>where</c> that designates X. If an insertion occurred, the function returns
		///     <c>std::pair(where, true)</c>. Otherwise, it returns <c>std::pair(where, false)</c>.
		///     <para>The second member function returns insert(<paramref name="_Value"/>), using <paramref name="_Where"/> as a starting
		///     place within the controlled sequence to search for the insertion point.</para>
		///     <para>The third member function inserts the sequence of element values from the range [<paramref name="_First"/>,
		///     <paramref name="_Last"/>).</para>
		///     <para>The last two member functions behave the same as the first two, except that <paramref name="_Value"/> is used to
		///     construct the inserted value.</para>
		/// </remarks>
		/**/
		template<class _Iterator>
		void insert(_Iterator _First, _Iterator _Last)
		{
			this->_Insert(_First, _Last);
		}

		/// <summary>
		///     Adds elements to the <c>ConcurrentMap</c> object.
		/// </summary>
		/// <typeparam name="_Valty">
		///     The type of the value inserted into the map.
		/// </typeparam>
		/// <param name="_Value">
		///     The value to be inserted.
		/// </param>
		/// <returns>
		///     A pair that contains an iterator and a boolean value. See the Remarks section for more details.
		/// </returns>
		/// <remarks>
		///     The first member function determines whether an element X exists in the sequence whose key has equivalent ordering to
		///     that of <paramref name="_Value"/>. If not, it creates such an element X and initializes it with <paramref name="_Value"/>.
		///     The function then determines the iterator <c>where</c> that designates X. If an insertion occurred, the function returns
		///     <c>std::pair(where, true)</c>. Otherwise, it returns <c>std::pair(where, false)</c>.
		///     <para>The second member function returns insert(<paramref name="_Value"/>), using <paramref name="_Where"/> as a starting
		///     place within the controlled sequence to search for the insertion point.</para>
		///     <para>The third member function inserts the sequence of element values from the range [<paramref name="_First"/>,
		///     <paramref name="_Last"/>).</para>
		///     <para>The last two member functions behave the same as the first two, except that <paramref name="_Value"/> is used to
		///     construct the inserted value.</para>
		/// </remarks>
		/**/
		template<class _Valty>
		::std::pair<iterator, bool> insert(_Valty&& _Value)
		{
			return this->_Insert(::std::forward<_Valty>(_Value));
		}

		/// <summary>
		///     Adds elements to the <c>ConcurrentMap</c> object.
		/// </summary>
		/// <typeparam name="_Valty">
		///     The type of the value inserted into the map.
		/// </typeparam>
		/// <param name="_Where">
		///     The starting location to search for an insertion point.
		/// </param>
		/// <param name="_Value">
		///     The value to be inserted.
		/// </param>
		/// <returns>
		///     An iterator pointing to the insertion location of the object.  If the key already exists in the container
		///     an iterator pointing to the duplicate key location in the map is returned.
		/// </returns>
		/// <remarks>
		///     The first member function determines whether an element X exists in the sequence whose key has equivalent ordering to
		///     that of <paramref name="_Value"/>. If not, it creates such an element X and initializes it with <paramref name="_Value"/>.
		///     The function then determines the iterator <c>where</c> that designates X. If an insertion occurred, the function returns
		///     <c>std::pair(where, true)</c>. Otherwise, it returns <c>std::pair(where, false)</c>.
		///     <para>The second member function returns insert(<paramref name="_Value"/>), using <paramref name="_Where"/> as a starting
		///     place within the controlled sequence to search for the insertion point.</para>
		///     <para>The third member function inserts the sequence of element values from the range [<paramref name="_First"/>,
		///     <paramref name="_Last"/>).</para>
		///     <para>The last two member functions behave the same as the first two, except that <paramref name="_Value"/> is used to
		///     construct the inserted value.</para>
		/// </remarks>
		/**/
		template<class _Valty>
		::std::enable_if_t<!::std::is_same_v<const_iterator,
			::std::remove_reference_t<_Valty>>, iterator>
			insert(const_iterator _Where, _Valty&& _Value)
		{
			// Current implementation ignores the hint. The method is provided for compatibility with unordered_map.
			return this->_Insert(::std::forward<_Valty>(_Value)).first;
		}

		/// <summary>
		///     Removes elements from the <c>ConcurrentMap</c> at specified positions. This method is not concurrency-safe.
		/// </summary>
		/// <param name="_Where">
		///     The iterator position to erase from.
		/// </param>
		/// <remarks>
		///     The first member function removes the element of the controlled sequence pointed to by <paramref name="_Where"/>. The second
		///     member function removes the elements in the range [<paramref name="_Begin"/>, <paramref name="_End"/>).
		///     <para>The third member function removes the elements in the range delimited by <see cref="ConcurrentMap::equal_range Method">
		///     ConcurrentMap::equal_range</see>(_Keyval). </para>
		/// </remarks>
		/// <returns>
		///     The first two member functions return an iterator that designates the first element remaining beyond any elements removed,
		///     or <see cref="ConcurrentMap::end Method"> ConcurrentMap::end</see>() if no such element exists. The third
		///     member function returns the number of elements it removes.
		/// </returns>
		/**/
		iterator unsafe_erase(const_iterator _Where)
		{
			return _Mybase::unsafe_erase(_Where);
		}

		/// <summary>
		///     Removes elements from the <c>ConcurrentMap</c> at specified positions. This method is not concurrency-safe.
		/// </summary>
		/// <param name="_Begin">
		///     The position of the first element in the range of elements to be erased.
		/// </param>
		/// <param name="_End">
		///     The position of the first element beyond the range of elements to be erased.
		/// </param>
		/// <remarks>
		///     The first member function removes the element of the controlled sequence pointed to by <paramref name="_Where"/>. The second
		///     member function removes the elements in the range [<paramref name="_Begin"/>, <paramref name="_End"/>).
		///     <para>The third member function removes the elements in the range delimited by <see cref="ConcurrentMap::equal_range Method">
		///     ConcurrentMap::equal_range</see>(_Keyval). </para>
		/// </remarks>
		/// <returns>
		///     The first two member functions return an iterator that designates the first element remaining beyond any elements removed,
		///     or <see cref="ConcurrentMap::end Method"> ConcurrentMap::end</see>() if no such element exists. The third
		///     member function returns the number of elements it removes.
		/// </returns>
		/**/
		iterator unsafe_erase(const_iterator _Begin, const_iterator _End)
		{
			return _Mybase::unsafe_erase(_Begin, _End);
		}

		/// <summary>
		///     Removes elements from the <c>ConcurrentMap</c> at specified positions. This method is not concurrency-safe.
		/// </summary>
		/// <param name="_Keyval">
		///     The key value to erase.
		/// </param>
		/// <remarks>
		///     The first member function removes the element of the controlled sequence pointed to by <paramref name="_Where"/>. The second
		///     member function removes the elements in the range [<paramref name="_Begin"/>, <paramref name="_End"/>).
		///     <para>The third member function removes the elements in the range delimited by <see cref="ConcurrentMap::equal_range Method">
		///     ConcurrentMap::equal_range</see>(_Keyval). </para>
		/// </remarks>
		/// <returns>
		///     The first two member functions return an iterator that designates the first element remaining beyond any elements removed,
		///     or <see cref="ConcurrentMap::end Method"> ConcurrentMap::end</see>() if no such element exists. The third
		///     member function returns the number of elements it removes.
		/// </returns>
		/**/
		size_type unsafe_erase(const key_type& _Keyval)
		{
			return _Mybase::unsafe_erase(_Keyval);
		}

		/// <summary>
		///     Swaps the contents of two <c>ConcurrentMap</c> objects. This method is not concurrency-safe.
		/// </summary>
		/// <param name="_Umap">
		///     The <c>ConcurrentMap</c> object to swap with.
		/// </param>
		/**/
		void swap(ConcurrentMap& _Umap)
		{
			_Mybase::swap(_Umap);
		}

		/// <summary>
		///     Gets the stored hash function object.
		/// </summary>
		/// <returns>
		///     The stored hash function object.
		/// </returns>
		/**/
		constexpr hasher hash_function() const noexcept(nothrow_copy_constructibles<hasher>)
		{
			return this->_M_comparator._M_hash_object;
		}

		/// <summary>
		///     Gets the stored equality comparison function object.
		/// </summary>
		/// <returns>
		///     The stored equality comparison function object.
		/// </returns>
		/**/
		constexpr key_equal key_eq() const noexcept(nothrow_copy_constructibles<key_equal>)
		{
			return this->_M_comparator._M_key_compare_object;
		}

		/// <summary>
		///     Finds or inserts an element with the specified key. This method is concurrency-safe.
		/// </summary>
		/// <param name="_Keyval">
		///     The key value to find or insert.
		/// </param>
		/// <returns>
		///     A reference to the data value of the found or inserted element.
		/// </returns>
		/// <remarks>
		///     If the argument key value is not found, then it is inserted along with the default value of the data type.
		///     <para> <c>operator[]</c> may be used to insert elements into a map <c>m</c> using <c>m[_Key] = DataValue;</c>, where
		///     <c>DataValue</c> is the value of the <c>mapped_type</c> of the element with a key value of <c>_Key</c>.</para>
		///     <para> When using <c>operator[]</c> to insert elements, the returned reference does not indicate whether an insertion
		///     is changing a pre-existing element or creating a new one. The member functions <see cref="ConcurrentMap::find Method">
		///     find</see> and <see cref="ConcurrentMap::insert Method">insert</see> can be used to determine whether an element
		///     with a specified key is already present before an insertion.</para>
		/// </remarks>
		/**/
		mapped_type& operator[](const key_type& _Keyval)
		{
			iterator _Where = this->find(_Keyval);

			if (_Where == this->end())
			{
				_Where = this->_Insert(::std::make_pair(_Keyval, mapped_type())).first;
			}

			return ((*_Where).second);
		}

		/// <summary>
		///     Finds or inserts an element with the specified key. This method is concurrency-safe.
		/// </summary>
		/// <param name="_Keyval">
		///     The key value to find or insert.
		/// </param>
		/// <returns>
		///     A reference to the data value of the found or inserted element.
		/// </returns>
		/// <remarks>
		///     If the argument key value is not found, then it is inserted along with the default value of the data type.
		///     <para> <c>operator[]</c> may be used to insert elements into a map <c>m</c> using <c>m[_Key] = DataValue;</c>, where
		///     <c>DataValue</c> is the value of the <c>mapped_type</c> of the element with a key value of <c>_Key</c>.</para>
		///     <para> When using <c>operator[]</c> to insert elements, the returned reference does not indicate whether an insertion
		///     is changing a pre-existing element or creating a new one. The member functions <see cref="ConcurrentMap::find Method">
		///     find</see> and <see cref="ConcurrentMap::insert Method">insert</see> can be used to determine whether an element
		///     with a specified key is already present before an insertion.</para>
		/// </remarks>
		/**/
		mapped_type& operator[](key_type&& _Keyval)
		{
			iterator _Where = this->find(_Keyval);

			if (_Where == this->end())
			{
				_Where = this->_Insert(::std::make_pair(::std::forward<key_type>(_Keyval), mapped_type())).first;
			}

			return ((*_Where).second);
		}

		/// <summary>
		///     Finds an element in a <c>ConcurrentMap</c> with a specified key value.. This method is concurrency-safe.
		/// </summary>
		/// <param name="_Keyval">
		///     The key value to find.
		/// </param>
		/// <returns>
		///     A reference to the data value of the element found.
		/// </returns>
		/// <remarks>
		///     If the argument key value is not found, the function throws an object of class <c>out_of_range</c>.
		/// </remarks>
		/**/
		mapped_type& at(const key_type& _Keyval)
		{
			iterator _Where = this->find(_Keyval);

			if (_Where == this->end())
			{
				_STD _Xout_of_range("invalid ConcurrentMap<K, T> key");
			}

			return ((*_Where).second);
		}

		/// <summary>
		///     Finds an element in a <c>ConcurrentMap</c> with a specified key value.. This method is concurrency-safe.
		/// </summary>
		/// <param name="_Keyval">
		///     The key value to find.
		/// </param>
		/// <returns>
		///     A reference to the data value of the element found.
		/// </returns>
		/// <remarks>
		///     If the argument key value is not found, the function throws an object of class <c>out_of_range</c>.
		/// </remarks>
		/**/
		const mapped_type& at(const key_type& _Keyval) const
		{
			const_iterator _Where = this->find(_Keyval);

			if (_Where == this->end())
			{
				_STD _Xout_of_range("invalid ConcurrentMap<K, T> key");
			}

			return ((*_Where).second);
		}
	};
}