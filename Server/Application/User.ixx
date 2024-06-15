export module Iconer.App.User;
export import Iconer.App.ISession;
import Iconer.Net.Socket;
import Iconer.Net.TcpReceiver;

export namespace iconer::app
{
	class [[nodiscard]] User : public ISession, public iconer::net::TcpReceiver
	{
	public:
		using super = ISession;
		using super_receiver = iconer::net::TcpReceiver;
		using id_type = super::id_type;

		explicit constexpr User(id_type id, iconer::net::Socket&& socket) noexcept
			: super(id)
			, super_receiver(std::move(socket), 512)
		{}
	};
}
