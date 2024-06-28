export module Iconer.Net.SocketWrapper;
export import Iconer.Net.Socket;
import <new>;

export namespace iconer::net
{
	class alignas(std::hardware_constructive_interference_size) [[nodiscard]] SocketWrapper
	{
	public:
		iconer::net::Socket mySocket{};

		constexpr SocketWrapper() noexcept = default;

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
