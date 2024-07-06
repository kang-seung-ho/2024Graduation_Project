export module Iconer.Net.SocketPool;
import Iconer.Net.IoResult;
import <new>;
import <vector>;

export namespace iconer::net
{
	class [[nodiscard]] Socket;

	class [[nodiscard]] SocketPool
	{
	public:
		explicit constexpr SocketPool() = default;
		~SocketPool() noexcept;

		iconer::net::IoResult Initialize(size_t socket_cnt);
		void Cleanup() noexcept;

		[[nodiscard]]
		constexpr auto begin() noexcept
		{
			return mySockets.begin();
		}

		[[nodiscard]]
		constexpr auto end() noexcept
		{
			return mySockets.end();
		}

		[[nodiscard]]
		constexpr auto begin() const noexcept
		{
			return mySockets.begin();
		}

		[[nodiscard]]
		constexpr auto end() const noexcept
		{
			return mySockets.end();
		}

		[[nodiscard]]
		constexpr auto cbegin() const noexcept
		{
			return mySockets.cbegin();
		}

		[[nodiscard]]
		constexpr auto cend() const noexcept
		{
			return mySockets.cend();
		}

	private:
		alignas(std::hardware_constructive_interference_size) std::vector<iconer::net::Socket*> mySockets{};
	};
}
