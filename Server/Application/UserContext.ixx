export module Iconer.App.UserContext;
import Iconer.Utility.ReadOnly;
import Iconer.App.TaskContext;
import Iconer.App.ISession;
import Iconer.App.PacketSerializer;
import <cstddef>;
import <span>;

export namespace iconer::app
{
	class [[nodiscard]] UserContext : public TaskContext
	{
	public:
		using super = TaskContext;
		using this_class = UserContext;
		using id_type = iconer::app::ISession::id_type;

		static inline constexpr size_t signInPacketSize = 3 + sizeof(std::int32_t);
		static inline constexpr size_t signInFailedPacketSize = 3 + sizeof(std::uint32_t);

		iconer::util::ReadOnly<id_type> ownerId;
		iconer::util::ReadOnly<class User*> ownerHandle;

		constexpr UserContext(id_type id, class User* ptr) noexcept
			: super()
			, ownerId(id), ownerHandle(ptr)
		{
			(void)SerializeAt(signInPacketData, PacketProtocol::SC_SIGNIN_SUCCESS, static_cast<std::int32_t>(id));

			(void)SerializeAt(signInFailedPacketData, PacketProtocol::SC_SIGNIN_FAILURE, std::uint32_t(0));
		}

		[[nodiscard]]
		constexpr std::span<const std::byte, signInPacketSize> GetSignInPacketData() const noexcept
		{
			return std::span{ signInPacketData };
		}
		
		[[nodiscard]]
		constexpr std::span<const std::byte, signInFailedPacketSize> GetSignInFailurePacketData() const noexcept
		{
			return std::span{ signInFailedPacketData };
		}

	private:
		std::byte signInPacketData[signInPacketSize]{};
		std::byte signInFailedPacketData[signInFailedPacketSize]{};
	};
}
