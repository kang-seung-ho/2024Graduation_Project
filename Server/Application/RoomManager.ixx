export module Iconer.App.RoomManager;
import Iconer.Utility.TypeTraits;
import Iconer.Net.ErrorCode;
import Iconer.Net.IoResult;
import Iconer.App.ISession;
import Iconer.App.Settings;
import <optional>;
import <vector>;

export namespace iconer::app
{
	class [[nodiscard]] User;
	class [[nodiscard]] Room;

	class [[nodiscard]] RoomManager
	{
	public:
		using id_type = iconer::app::ISession::id_type;
		using session_type = iconer::app::Room;
		using pointer_type = iconer::app::Room*;

		static inline constexpr size_t maxRoomCount = iconer::app::Settings::roomsLimit;
		static inline constexpr id_type minRoomUid = 2000;
		static inline constexpr id_type maxRoomUid = minRoomUid + maxRoomCount;

		explicit constexpr RoomManager() noexcept = default;
		constexpr ~RoomManager() noexcept = default;

		void Initialize();
		void Startup();
		void Cleanup();

		[[nodiscard]]
		std::optional<pointer_type> AcquireEmptyRoom(iconer::app::User& user) const;

		[[nodiscard]]
		std::optional<pointer_type> FindRoom(id_type id) const noexcept;

		[[nodiscard]]
		bool HasRoom(id_type id) const noexcept;

	private:
		alignas(std::hardware_constructive_interference_size) std::vector<pointer_type> everyRooms{};

		void AddRoom(pointer_type session);
	};
}
