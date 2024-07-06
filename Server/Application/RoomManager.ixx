export module Iconer.App.RoomManager;
import Iconer.Utility.TypeTraits;
import Iconer.Net.ErrorCode;
import Iconer.Net.IoResult;
import Iconer.App.ISession;
import Iconer.App.Settings;
import <memory>;
import <optional>;
import <span>;
import <array>;
import <vector>;
import <atomic>;

export namespace iconer::app
{
	class [[nodiscard]] User;
	class [[nodiscard]] Room;

	struct SerializedRoom
	{
		static inline constexpr size_t nameLength = 16;

		[[nodiscard]]
		static consteval size_t GetSerializedSize() noexcept
		{
			return sizeof(std::int32_t) + sizeof(wchar_t) * nameLength + sizeof(size_t);
		}

		std::int32_t id{ -1 };
		std::array<wchar_t, nameLength + 1> title{};
		size_t member_cnt{};
	};

	struct [[nodiscard]] SerializedUser
	{
		static inline constexpr size_t nameLength = 16;

		std::int32_t id{ -1 };
		char team_id{ 0 }; // 1: red, 2: blue
		std::array<wchar_t, nameLength + 1> nickname{};
	};

	class [[nodiscard]] RoomManager
	{
	public:
		using id_type = iconer::app::ISession::id_type;
		using session_type = iconer::app::Room;
		using member_type = iconer::app::User;
		using pointer_type = session_type*;
		using reference = session_type&;
		using const_reference = const session_type&;

		static inline constexpr size_t maxRoomCount = iconer::app::Settings::roomsLimit;
		static inline constexpr id_type minRoomUid = 2000;
		static inline constexpr id_type maxRoomUid = minRoomUid + maxRoomCount;
		static inline constexpr std::int16_t maxSerializeRoomDataSize = 3 + SerializedRoom::GetSerializedSize() * maxRoomCount + sizeof(size_t);

		explicit constexpr RoomManager() noexcept = default;
		constexpr ~RoomManager() noexcept = default;

		void Initialize();
		void Startup();
		void Cleanup();

		template<invocable<reference> Callable>
		constexpr void Foreach(Callable&& fun) const noexcept(nothrow_invocable<Callable, reference>)
		{
			for (const pointer_type& ptr : everyRooms)
			{
				std::forward<Callable>(fun)(*ptr);
			}
		}

		template<invocable<const_reference> Callable>
		constexpr void Foreach(Callable&& fun) const noexcept(nothrow_invocable<Callable, const_reference>)
		{
			for (const pointer_type& ptr : everyRooms)
			{
				std::forward<Callable>(fun)(*ptr);
			}
		}

		template<invocable<pointer_type> Callable>
		constexpr void Foreach(Callable&& fun) const noexcept(nothrow_invocable<Callable, pointer_type>)
		{
			for (const pointer_type& ptr : everyRooms)
			{
				std::forward<Callable>(fun)(ptr);
			}
		}

		[[nodiscard]]
		std::optional<pointer_type> AcquireEmptyRoom(member_type& user) const;

		[[nodiscard]]
		std::span<const std::byte> AcquireCachedRoomData();

		[[nodiscard]]
		std::optional<pointer_type> FindRoom(id_type id) const noexcept;

		[[nodiscard]]
		bool HasRoom(id_type id) const noexcept;

	private:
		std::vector<pointer_type> everyRooms{};
		std::array<std::byte, maxSerializeRoomDataSize> precachedRoomListData{};
		size_t precachedRoomListDataSize{};
		std::atomic_bool isRoomDirty{};

		void AddRoom(pointer_type session);

		void OnRoomMade(pointer_type room, member_type* user);
		void OnUserJoined(pointer_type room, member_type* user, std::int32_t member_cnt);
		void OnUserLeft(pointer_type room, member_type* user, std::int32_t member_cnt);
		void OnRoomClosed(pointer_type room);
	};
}
