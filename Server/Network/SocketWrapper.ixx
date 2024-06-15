export module Iconer.Net.SocketWrapper;
export import Iconer.Net.Socket;

export namespace iconer::net
{
	class [[nodiscard]] SocketWrapper
	{
	public:
		iconer::net::Socket mySocket;

		constexpr SocketWrapper() noexcept = default;
		constexpr ~SocketWrapper() noexcept = default;

		constexpr SocketWrapper(iconer::net::Socket&& socket) noexcept
			: mySocket(static_cast<iconer::net::Socket&&>(socket))
		{}

		SocketWrapper(SocketWrapper&&) = default;
		SocketWrapper& operator=(SocketWrapper&&) = default;

	private:
		SocketWrapper(const SocketWrapper&) = delete;
		SocketWrapper& operator=(const SocketWrapper&) = delete;
	};
}
