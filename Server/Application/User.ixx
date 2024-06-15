export module Iconer.App.User;
import Iconer.Net.Socket;
import Iconer.Net.TcpReceiver;
import Iconer.App.ISession;
import Iconer.App.TaskContext;

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

		[[nodiscard]]
		friend constexpr bool operator==(const User& lhs, const User& rhs) noexcept
		{
			return lhs.GetID() == rhs.GetID();
		}
	};
}
