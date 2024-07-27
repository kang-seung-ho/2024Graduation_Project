module;
#include <exception>
#include <atomic>
#include <utility>
#include <vector>
#include <algorithm>
#include <shared_mutex>

export module Iconer.Application.ISessionManager;
import Iconer.Utility.Constraints;
import Iconer.Application.IObject;

export namespace iconer::app
{
	class SessionLimitExceedError : public std::exception
	{
	public:
		explicit SessionLimitExceedError() noexcept
			: exception("Number of session is on limited.")
		{
		}
	};

	template<typename T>
	class ISessionManager
	{
	public:
		using lock_t = std::shared_mutex;
		using mapped_type = T;
		using key_type = typename mapped_type::IdType;
		using data_t = std::vector<std::pair<key_type, mapped_type>>;
		using value_type = typename data_t::value_type;

	private:
		struct KeyComparator
		{
			[[nodiscard]]
			constexpr bool operator()(const value_type& lhs, const value_type& rhs) const noexcept
			{
				return std::get<0>(lhs) < std::get<0>(rhs);
			}
			[[nodiscard]]
			constexpr bool operator()(const key_type& id, const value_type& rhs) const noexcept
			{
				return id < std::get<0>(rhs);
			}
			[[nodiscard]]
			constexpr bool operator()(const value_type& lhs, const key_type& id) const noexcept
			{
				return std::get<0>(lhs) < id;
			}
		};
		struct KeyExtractor
		{
			[[nodiscard]]
			constexpr const key_type& operator()(const value_type& pair) const noexcept
			{
				return std::get<0>(pair);
			}

			[[nodiscard]]
			constexpr const key_type&& operator()(value_type&& pair) const noexcept
			{
				return std::get<0>(pair);
			}
		};
		struct ValueExtractor
		{
			[[nodiscard]]
			constexpr mapped_type& operator()(value_type& pair) const noexcept
			{
				return std::get<1>(pair);
			}

			[[nodiscard]]
			constexpr const mapped_type& operator()(const value_type& pair) const noexcept
			{
				return std::get<1>(pair);
			}

			[[nodiscard]]
			constexpr mapped_type&& operator()(value_type&& pair) const noexcept
			{
				return std::get<1>(pair);
			}
		};

	public:
		using allocator_type = typename data_t::allocator_type;
		using pointer = typename data_t::pointer;
		using const_pointer = typename data_t::const_pointer;
		using reference = typename data_t::reference;
		using const_reference = typename data_t::const_reference;
		using size_type = typename data_t::size_type;
		using difference_type = typename data_t::difference_type;
		using iterator = typename data_t::iterator;
		using const_iterator = typename data_t::const_iterator;

		static inline constexpr bool NothrowInitializable = nothrow_default_constructibles<data_t, lock_t>;
		static inline constexpr bool NothrowLockInitializable = nothrow_default_constructibles<lock_t>;
		static inline constexpr bool NothrowDestuctible = nothrow_destructibles<data_t, lock_t>;

		constexpr ISessionManager() noexcept(NothrowInitializable) = default;
		constexpr ~ISessionManager() noexcept(NothrowDestuctible) = default;

		constexpr ISessionManager(size_type size)
			noexcept(NothrowInitializable and NothrowLockInitializable and noexcept(Reserve(size_type{})))
			: myData(), myLock()
			, sessionLimit(size), isLimited(false)
		{
			Reserve(size);
		}

		// Unsafe
		constexpr void Reserve(size_type size)
			noexcept(noexcept(std::declval<data_t>().reserve(size_type{})))
		{
			myData.reserve(size);
		}

		constexpr void SetLimit(bool flag) noexcept
		{
			isLimited = flag;
		}

		void SetLimitation(size_type size, bool flag) noexcept
		{
			sessionLimit = size;
			isLimited = flag;
		}

		void SetLimitation(size_type size) noexcept
		{
			sessionLimit = size;
		}

		constexpr void Add(const mapped_type& object) requires copyable<data_t>
		{
			std::unique_lock lock{ myLock };
			if (isLimited and sessionLimit <= myData.size())
			{
				throw SessionLimitExceedError{};
			}

			if (not UncheckedContains(object.GetID()))
			{
				myData.push_back(std::make_pair(object.GetID(), object));
				MakeHoldUniques();
			}
		}

		constexpr void Add(mapped_type&& object) requires movable<data_t>
		{
			std::unique_lock lock{ myLock };
			if (isLimited and sessionLimit <= myData.size())
			{
				throw SessionLimitExceedError{};
			}

			if (not UncheckedContains(object.GetID()))
			{
				myData.push_back(std::make_pair(std::move(object).GetID(), std::move(object)));
			}
		}

		template<typename... Args>
		constexpr iterator Emplace(const key_type& id, Args&&... args)
		{
			std::unique_lock lock{ myLock };
			if (isLimited and sessionLimit <= myData.size())
			{
				throw SessionLimitExceedError{};
			}

			static SessionFactory<mapped_type> factory{};

			iterator bound = std::lower_bound(myData.begin(), myData.end(), id, KeyComparator{});
			auto result = myData.insert(bound, factory.Create(std::forward<Args>(args)...));

			return result;

		}

		constexpr void MakeHoldUniques() noexcept
		{
			std::sort(myData.begin(), myData.end(), KeyComparator{});
		}

		template<invocable_results<bool, const mapped_type&> Fn>
		[[nodiscard]]
		iterator Search(Fn&& fn) noexcept
		{
			std::unique_lock lock{ myLock, std::try_to_lock_t{} };
			return UncheckedSearch(std::forward<Fn>(fn));
		}

		template<invocable_results<bool, const mapped_type&> Fn>
		[[nodiscard]]
		const_iterator Search(Fn&& fn) const noexcept
		{
			std::unique_lock lock{ myLock, std::try_to_lock_t{} };
			return UncheckedSearch(std::forward<Fn>(fn));
		}

		[[nodiscard]]
		iterator Find(const key_type& id) noexcept
		{
			std::unique_lock lock{ myLock, std::try_to_lock_t{} };
			return UncheckedFind(id);
		}

		[[nodiscard]]
		iterator Find(key_type&& id) noexcept
		{
			std::unique_lock lock{ myLock, std::try_to_lock_t{} };
			return UncheckedFind(std::move(id));
		}

		[[nodiscard]]
		const_iterator Find(const key_type& id) const noexcept
		{
			std::unique_lock lock{ myLock, std::try_to_lock_t{} };
			return UncheckedFind(id);
		}

		[[nodiscard]]
		const_iterator Find(key_type&& id) const noexcept
		{
			std::unique_lock lock{ myLock, std::try_to_lock_t{} };
			return UncheckedFind(std::move(id));
		}

		template<typename Uid>
		[[nodiscard]]
		iterator Find(Uid&& id) noexcept
		{
			std::unique_lock lock{ myLock, std::try_to_lock_t{} };
			return UncheckedFind(std::forward<Uid>(id));
		}

		template<typename Uid>
		[[nodiscard]]
		const_iterator Find(Uid&& id) const noexcept
		{
			std::unique_lock lock{ myLock, std::try_to_lock_t{} };
			return UncheckedFind(std::forward<Uid>(id));
		}

		[[nodiscard]]
		iterator begin() noexcept
		{
			std::shared_lock lock{ myLock };
			return myData.begin();
		}

		[[nodiscard]]
		iterator end() noexcept
		{
			std::shared_lock lock{ myLock };
			return myData.end();
		}

		[[nodiscard]]
		const_iterator begin() const noexcept
		{
			std::shared_lock lock{ myLock };
			return myData.begin();
		}

		[[nodiscard]]
		const_iterator end() const noexcept
		{
			std::shared_lock lock{ myLock };
			return myData.end();
		}

		[[nodiscard]]
		const_iterator cbegin() const noexcept
		{
			std::shared_lock lock{ myLock };
			return myData.begin();
		}

		[[nodiscard]]
		const_iterator cend() const noexcept
		{
			std::shared_lock lock{ myLock };
			return myData.end();
		}

		[[nodiscard]]
		mapped_type* operator[](const key_type& id) noexcept
		{
			std::shared_lock lock{ myLock };
			if (iterator it = Find(id); it != myData.end())
			{
				return std::addressof(it->second);
			}
			else
			{
				return nullptr;
			}
		}

		[[nodiscard]]
		mapped_type* operator[](const key_type& id) const noexcept
		{
			std::shared_lock lock{ myLock };
			if (const_iterator it = Find(id); it != myData.end())
			{
				return std::addressof(it->second);
			}
			else
			{
				return nullptr;
			}
		}

		[[nodiscard]]
		size_type GetSize() const noexcept
		{
			return myData.size();
		}

		[[nodiscard]]
		size_t GetCapacity() const noexcept
		{
			return myData.capacity();
		}

		[[nodiscard]]
		size_type GetLimit() const noexcept
		{
			return sessionLimit.load(std::memory_order_relaxed);
		}

		template<typename Uid>
		[[nodiscard]]
		bool Contains(Uid id) const noexcept
		{
			std::unique_lock lock{ myLock };
			return UncheckedContains(std::forward<Uid>(id));
		}

		[[nodiscard]]
		bool ReachedLimit() const noexcept
		{
			std::unique_lock lock{ myLock, std::try_to_lock_t{} };
			return isLimited and sessionLimit == myData.size();
		}

		[[nodiscard]]
		bool IsEmpty() const noexcept
		{
			std::shared_lock lock{ myLock };
			return myData.empty();
		}

		[[nodiscard]]
		constexpr bool IsLimited() const noexcept
		{
			return isLimited;
		}

	protected:
		template<typename Fn>
		[[nodiscard]]
		constexpr iterator UncheckedSearch(Fn&& fn) noexcept
		{
			static_assert(invocable_results<Fn, bool, const mapped_type&>);

			return std::ranges::find_if(myData.begin(), myData.end(), std::forward<Fn>(fn), ValueExtractor{});
		}

		template<typename Fn>
		[[nodiscard]]
		constexpr const_iterator UncheckedSearch(Fn&& fn) const noexcept
		{
			static_assert(invocable_results<Fn, bool, const mapped_type&>);

			return std::ranges::find_if(myData.cbegin(), myData.cend(), std::forward<Fn>(fn), ValueExtractor{});
		}

		template<typename Uid>
		[[nodiscard]]
		constexpr iterator UncheckedFind(Uid&& id) noexcept
		{
			return std::ranges::find(myData.begin(), myData.end(), std::forward<Uid>(id), KeyExtractor{});
		}

		template<typename Uid>
		[[nodiscard]]
		constexpr const_iterator UncheckedFind(Uid&& id) const noexcept
		{
			return std::ranges::find(myData.cbegin(), myData.cend(), std::forward<Uid>(id), KeyExtractor{});
		}

		template<typename Uid>
		[[nodiscard]]
		constexpr bool UncheckedContains(Uid&& id) const noexcept
		{
			return std::binary_search(myData.cbegin(), myData.cend(), std::forward<Uid>(id), KeyComparator{});
		}

		data_t myData;
		mutable lock_t myLock;
		std::atomic<size_type> sessionLimit;
		bool isLimited;
	};
}

export namespace std
{
	template<typename S>
	[[nodiscard]]
	auto size(const iconer::app::ISessionManager<S>& mgr) noexcept
	{
		return mgr.GetSize();
	}

	template<typename S>
	[[nodiscard]]
	bool empty(const iconer::app::ISessionManager<S>& mgr) noexcept
	{
		return mgr.IsEmpty();
	}
}
