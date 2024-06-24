export module Iconer.App.User;
import Iconer.Utility.ReadOnly;
import Iconer.Utility.Delegate;
import Iconer.Net.Socket;
import Iconer.Net.TcpReceiver;
import Iconer.App.ISession;
import Iconer.App.TaskContext;
import <memory>;
import <span>;
import <atomic>;

export namespace iconer::app
{
	class [[nodiscard]] Room;

	class [[nodiscard]] User : public ISession
	{
	public:
		using super = ISession;
		using id_type = super::id_type;

		static inline constexpr size_t recvBufferSize = 512;

		iconer::net::TcpReceiver myReceiver;
		iconer::util::ReadOnly<TaskContext> mainContext;
		iconer::util::ReadOnly<TaskContext> recvContext;

		iconer::util::Delegate<void, User*> onDisconnected;

		explicit constexpr User(id_type id, iconer::net::Socket&& socket) noexcept
			: super(id)
			, myReceiver(std::move(socket), recvBufferSize)
			, mainContext(), recvContext(TaskCategory::OpRecv)
			, isConnected()
			, myRoom()
			, onDisconnected()
		{}

		void Cleanup();

		[[nodiscard]]
		std::expected<void, iconer::net::ErrorCode> BeginReceive()
		{
			return myReceiver.BeginReceive(recvContext);
		}

		bool EndReceive(const std::uint32_t bytes)
		{
			return myReceiver.EndReceive(recvContext, bytes);
		}

		void SetConnected(bool flag) volatile noexcept
		{
			isConnected.store(flag, std::memory_order_release);
		}

		[[nodiscard]]
		std::span<const std::byte> GetReceivedData() const noexcept
		{
			return myReceiver.GetReceiveBuffer();
		}

		[[nodiscard]]
		std::unique_ptr<std::byte[]> AcquireReceivedData(size_t size) const
		{
			auto span = myReceiver.GetReceiveBuffer();

			std::unique_ptr<std::byte[]> result = std::make_unique<std::byte[]>(size);

			std::memcpy(result.get(), span.data(), size);

			return std::move(result);
		}

		[[nodiscard]]
		const iconer::net::Socket& GetSocket() const noexcept
		{
			return myReceiver.mySocket;
		}

		[[nodiscard]]
		bool IsConnected() const volatile noexcept
		{
			return isConnected.load(std::memory_order_acquire);
		}

		[[nodiscard]]
		friend constexpr bool operator==(const User& lhs, const User& rhs) noexcept
		{
			return lhs.GetID() == rhs.GetID();
		}

	private:
		std::atomic_bool isConnected;
		std::atomic<Room*> myRoom;
	};
}
