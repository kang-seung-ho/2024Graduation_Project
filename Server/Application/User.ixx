export module Iconer.App.User;
import Iconer.Net.Socket;
import Iconer.Net.TcpReceiver;
import Iconer.App.ISession;
import Iconer.App.TaskContext;
import <atomic>;

export namespace iconer::app
{
	class [[nodiscard]] User : public ISession
	{
	public:
		using super = ISession;
		using id_type = super::id_type;

		static inline constexpr size_t recvBufferSize = 512;

		iconer::net::TcpReceiver myReceiver;
		TaskContext mainContext;
		TaskContext recvContext;

		explicit constexpr User(id_type id, iconer::net::Socket&& socket) noexcept
			: super(id)
			, myReceiver(std::move(socket), recvBufferSize)
			, mainContext(), recvContext(TaskCategory::OpRecv)
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

		[[nodiscard]]
		const iconer::net::Socket& GetSocket() const noexcept
		{
			return myReceiver.mySocket;
		}

		void SetConnected(bool flag) volatile noexcept
		{
			isConnected.store(flag, std::memory_order_release);
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
	};
}
