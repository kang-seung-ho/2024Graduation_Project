export module Iconer.Utility.BorrowedPointer;
import Iconer.Utility.Constraints;
import <memory>;
import <string>;
import <format>;

export namespace iconer::util
{
	template<typename T>
	class [[nodiscard]] BorrowedPointer;

	template<notvoids T>
		requires (not std::is_array_v<T>)
	class [[nodiscard]] BorrowedPointer<T>
	{
	public:
		constexpr BorrowedPointer() noexcept = default;

		constexpr BorrowedPointer(nullptr_t) noexcept
			: myData()
		{
		}

		explicit constexpr BorrowedPointer(T* ptr) noexcept
			: myData(ptr)
		{
		}

		template<convertible_to<T> U>
			requires (not std::is_array_v<U>)
		explicit constexpr BorrowedPointer(U* ptr) noexcept
			: myData(ptr)
		{
		}

		[[nodiscard]]
		constexpr T& operator*() const& noexcept
		{
			return *myData;
		}

		[[nodiscard]]
		constexpr T& operator*() && noexcept
		{
			return *std::exchange(myData, nullptr);
		}

		[[nodiscard]]
		constexpr T* operator->() const noexcept
		{
			return myData;
		}

		constexpr operator T* () const noexcept
		{
			return myData;
		}

		BorrowedPointer(const BorrowedPointer&) noexcept = default;
		BorrowedPointer& operator=(const BorrowedPointer&) noexcept = default;
		BorrowedPointer(BorrowedPointer&&) noexcept = default;
		BorrowedPointer& operator=(BorrowedPointer&&) noexcept = default;

	private:
		T* myData;
	};

	template<notvoids T>
	class [[nodiscard]] BorrowedPointer<T[]>
	{
	public:
		constexpr BorrowedPointer() noexcept = default;

		constexpr BorrowedPointer(nullptr_t) noexcept
			: myData(), mySize(0)
		{
		}

		constexpr BorrowedPointer(nullptr_t, const size_t&) noexcept
			: myData(), mySize(0)
		{
		}

		explicit constexpr BorrowedPointer(const T* ptr, const size_t size) noexcept
			: myData(ptr), mySize(size)
		{
		}

		template<size_t Length>
		explicit constexpr BorrowedPointer(const T(&ptr)[Length]) noexcept
			: myData(ptr), mySize(Length)
		{
		}

		template<size_t Length>
		explicit constexpr BorrowedPointer(T(&& ptr)[Length]) noexcept
			: myData(std::move(ptr)), mySize(Length)
		{
		}

		template<typename U>
		explicit constexpr BorrowedPointer(const U* ptr, const size_t size) noexcept
			: myData(ptr), mySize(size)
		{
		}

		[[nodiscard]]
		constexpr const T*& Get() & noexcept
		{
			return myData;
		}

		[[nodiscard]]
		constexpr const T* const& Get() const& noexcept
		{
			return myData;
		}

		[[nodiscard]]
		constexpr const T*&& Get() && noexcept
		{
			return std::move(myData);
		}

		[[nodiscard]]
		constexpr const T* const&& Get() const&& noexcept
		{
			return std::move(myData);
		}

		[[nodiscard]]
		constexpr const size_t& GetSize() const& noexcept
		{
			return mySize;
		}

		[[nodiscard]]
		constexpr size_t&& GetSize() && noexcept
		{
			return std::move(mySize);
		}

		constexpr operator const T* () const noexcept
		{
			return myData;
		}

		BorrowedPointer(const BorrowedPointer&) noexcept = default;
		BorrowedPointer& operator=(const BorrowedPointer&) noexcept = default;
		BorrowedPointer(BorrowedPointer&&) noexcept = default;
		BorrowedPointer& operator=(BorrowedPointer&&) noexcept = default;

	private:
		const T* myData;
		size_t mySize;
	};

	template<notvoids T>
	class [[nodiscard]] BorrowedPointer<std::shared_ptr<T>>
	{
	public:
		constexpr BorrowedPointer() noexcept = default;

		constexpr BorrowedPointer(nullptr_t) noexcept
			: myData()
		{
		}

		BorrowedPointer(const std::shared_ptr<T>& ptr)
			: myData(ptr)
		{
		}

		BorrowedPointer(std::shared_ptr<T>&& ptr) noexcept
			: myData(std::move(ptr))
		{
		}

		[[nodiscard]]
		T& operator*() const& noexcept
		{
			std::shared_ptr<T> ptr = myData.lock();
			return *ptr;
		}

		[[nodiscard]]
		T& operator*() && noexcept
		{
			std::shared_ptr<T> ptr = std::exchange(myData, nullptr).lock();
			return *ptr;
		}

		[[nodiscard]]
		T* operator->() const noexcept
		{
			std::shared_ptr<T> ptr = myData.lock();
			return ptr.get();
		}

		operator T* () const noexcept
		{
			return myData.get();
		}

		BorrowedPointer(const BorrowedPointer&) noexcept = default;
		BorrowedPointer& operator=(const BorrowedPointer&) noexcept = default;
		BorrowedPointer(BorrowedPointer&&) noexcept = default;
		BorrowedPointer& operator=(BorrowedPointer&&) noexcept = default;

	private:
		std::weak_ptr<T> myData;
	};

	template<>
	class [[nodiscard]] BorrowedPointer<void>
	{
	public:
		constexpr BorrowedPointer() noexcept = default;
		constexpr BorrowedPointer(nullptr_t) noexcept
			: myData()
		{
		}

		constexpr BorrowedPointer(void* ptr) noexcept
			: myData(ptr)
		{
		}

		constexpr operator void* () const noexcept
		{
			return myData;
		}

		BorrowedPointer(const BorrowedPointer&) noexcept = default;
		BorrowedPointer& operator=(const BorrowedPointer&) noexcept = default;
		BorrowedPointer(BorrowedPointer&&) noexcept = default;
		BorrowedPointer& operator=(BorrowedPointer&&) noexcept = default;

	private:
		void* myData;
	};
}

export namespace std
{
	template<typename T>
	[[nodiscard]] string to_string(const iconer::util::BorrowedPointer<T>& ptr)
	{
		return std::format("BorrowedPointer[{}]", static_cast<const T*>(ptr));
	}

	template<typename T>
	[[nodiscard]] string to_string(iconer::util::BorrowedPointer<T>&& ptr)
	{
		return std::format("BorrowedPointer[{}]", static_cast<const T*>(ptr));
	}
}

export template<typename T>
struct std::formatter<iconer::util::BorrowedPointer<T>, char>
{
	static constexpr
		format_parse_context::iterator
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

	static format_context::iterator
		format(const iconer::util::BorrowedPointer<T>& ptr, format_context& context)
	{
		return format_to("BorrowedPointer[{}]", static_cast<const T*>(ptr));
	}

	static format_context::iterator
		format(iconer::util::BorrowedPointer<T>&& ptr, format_context& context)
	{
		return format_to("BorrowedPointer[{}]", static_cast<const T*>(ptr));
	}
};

export template<typename T>
struct std::formatter<iconer::util::BorrowedPointer<T>, wchar_t>
{
	static constexpr
		wformat_parse_context::iterator
		parse(wformat_parse_context& context)
		noexcept
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

	static wformat_context::iterator
		format(const iconer::util::BorrowedPointer<T>& ptr, wformat_context& context)
		noexcept
	{
		return format_to(L"BorrowedPointer[{}]", static_cast<const T*>(ptr));
	}

	static wformat_context::iterator
		format(iconer::util::BorrowedPointer<T>&& ptr, wformat_context& context)
		noexcept
	{
		return format_to(L"BorrowedPointer[{}]", static_cast<const T*>(ptr));
	}
};
