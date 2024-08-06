export module Iconer.Application.BorrowedSendContext;
import Iconer.Application.IContext;
import <cstddef>;
import <utility>;
import <memory>;

export namespace iconer::app
{
	class [[nodiscard]] BorrowedSendContext : public IContext
	{
	public:
		using Super = IContext;

		explicit constexpr BorrowedSendContext() noexcept
			: Super(AsyncOperations::OpSendBorrowed)
			, myBlob(), mySize(0)
		{}

		explicit constexpr BorrowedSendContext(std::unique_ptr<std::byte[]>&& ptr, const size_t& size) noexcept
			: Super(AsyncOperations::OpSendBorrowed)
			, myBlob(std::move(ptr)), mySize(size)
		{}

		constexpr BorrowedSendContext(BorrowedSendContext&& other) noexcept
			: Super(std::move(other.GetOperation()))
			, myBlob(std::move(other.myBlob)), mySize(std::move(other.mySize))
		{}

		constexpr BorrowedSendContext& operator=(BorrowedSendContext&& other) noexcept
		{
			lastOperation = std::move(other).GetOperation();
			myBlob = std::move(other.myBlob);
			mySize = std::move(other.mySize);
			return *this;
		}

		[[nodiscard]]
		constexpr void SetBlob(std::unique_ptr<std::byte[]>&& buffer) noexcept
		{
			myBlob = std::move(buffer);
		}

		[[nodiscard]]
		constexpr void SetSize(size_t size) noexcept
		{
			mySize = size;
		}

		void ReturnToBase();

		[[nodiscard]]
		constexpr std::byte* Detach() noexcept
		{
			mySize = 0;
			return myBlob.release();
		}

		constexpr void Destroy() noexcept
		{
			mySize = 0;
		}

		[[nodiscard]]
		constexpr const std::unique_ptr<std::byte[]>& GetBlob() const& noexcept
		{
			return myBlob;
		}

		[[nodiscard]]
		constexpr std::unique_ptr<std::byte[]>&& GetBlob() && noexcept
		{
			return std::move(myBlob);
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

		[[nodiscard]]
		constexpr ptrdiff_t GetSignedSize() const noexcept
		{
			return static_cast<ptrdiff_t>(mySize);
		}

	protected:
		std::unique_ptr<std::byte[]> myBlob;
		size_t mySize;

	private:
		BorrowedSendContext(const BorrowedSendContext&) = delete;
		BorrowedSendContext& operator=(const BorrowedSendContext&) = delete;
	};

	class [[nodiscard]] Borrower final
	{
	public:
		constexpr Borrower() noexcept = default;

		constexpr Borrower(Borrower&& other) noexcept
			: borrowedContext(std::exchange(other.borrowedContext, nullptr))
		{}

		explicit constexpr Borrower(BorrowedSendContext& ctx) noexcept
			: borrowedContext(std::addressof(ctx))
		{
		}

		constexpr Borrower(BorrowedSendContext* const& ctx) noexcept
			: borrowedContext(ctx)
		{
		}

		~Borrower()
		{
			//Complete();
		}

		[[nodiscard]]
		BorrowedSendContext* Release() noexcept
		{
			return std::exchange(borrowedContext, nullptr);
		}

		[[nodiscard]]
		BorrowedSendContext* Release() volatile noexcept
		{
			return std::exchange(borrowedContext, nullptr);
		}

		void Complete()
		{
			if (nullptr != borrowedContext)
			{
				Release()->ReturnToBase();
			}
		}
		
		void Complete() volatile
		{
			if (nullptr != borrowedContext)
			{
				Release()->ReturnToBase();
			}
		}

		constexpr Borrower& operator=(Borrower&& other) noexcept
		{
			borrowedContext = std::exchange(other.borrowedContext, nullptr);
			return *this;
		}

		constexpr Borrower& operator=(nullptr_t) noexcept
		{
			borrowedContext = nullptr;
			return *this;
		}
		
		constexpr Borrower& operator=(BorrowedSendContext* const& ptr) noexcept
		{
			borrowedContext = ptr;
			return *this;
		}

		constexpr BorrowedSendContext* operator->() const noexcept
		{
			return borrowedContext;
		}

		constexpr BorrowedSendContext* operator->() const volatile noexcept
		{
			return borrowedContext;
		}

		constexpr BorrowedSendContext& operator*() noexcept
		{
			return *borrowedContext;
		}

		constexpr const BorrowedSendContext& operator*() const noexcept
		{
			return *borrowedContext;
		}

		constexpr volatile BorrowedSendContext& operator*() volatile noexcept
		{
			return *borrowedContext;
		}

		constexpr const volatile BorrowedSendContext& operator*() const volatile noexcept
		{
			return *borrowedContext;
		}

		constexpr operator BorrowedSendContext* () const noexcept
		{
			return borrowedContext;
		}

		constexpr operator BorrowedSendContext* () const volatile noexcept
		{
			return borrowedContext;
		}

	private:
		Borrower(const Borrower&) = delete;
		Borrower& operator=(const Borrower&) = delete;

		BorrowedSendContext* borrowedContext;
	};
}
