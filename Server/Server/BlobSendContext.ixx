export module Iconer.Application.BlobSendContext;
import Iconer.Application.IContext;
import <cstddef>;
import <utility>;
import <memory>;

export namespace iconer::app
{
	class [[nodiscard]] BlobSendContext : public IContext
	{
	public:
		using Super = IContext;

		explicit constexpr BlobSendContext() noexcept
			: Super(AsyncOperations::OpSend)
			, myBlob()
		{}

		constexpr ~BlobSendContext() noexcept
		{
			Destroy();
		}

		explicit constexpr BlobSendContext(std::byte* ptr) noexcept
			: Super(AsyncOperations::OpSend)
			, myBlob(ptr)
		{}

		constexpr BlobSendContext(BlobSendContext&& other) noexcept
			: Super(std::move(other.GetOperation()))
			, myBlob(std::exchange(other.myBlob, nullptr))
		{}

		constexpr BlobSendContext& operator=(BlobSendContext&& other) noexcept
		{
			lastOperation = std::move(other).GetOperation();
			myBlob = std::exchange(other.myBlob, nullptr);
			return *this;
		}

		[[nodiscard]]
		constexpr std::byte* Detach() noexcept
		{
			return std::exchange(myBlob, nullptr);
		}

		constexpr void Destroy() noexcept
		{
			if (myBlob != nullptr)
			{
				delete[] std::exchange(myBlob, nullptr);
			}
		}

		[[nodiscard]]
		constexpr std::byte* GetBlob() const noexcept
		{
			return myBlob;
		}

	protected:
		std::byte* myBlob;

	private:
		BlobSendContext(const BlobSendContext&) = delete;
		BlobSendContext& operator=(const BlobSendContext&) = delete;
	};
}
