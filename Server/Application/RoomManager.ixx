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

		std::int32_t id{ -1 };
		std::array<wchar_t, nameLength + 1> title{};
		size_t member_cnt{};

		[[nodiscard]]
		static consteval size_t GetSerializedSize() noexcept
		{
			return sizeof(std::int32_t) + sizeof(wchar_t) * nameLength + sizeof(size_t);
		}
	};

	class [[nodiscard]] RoomManager
	{
	public:
		using id_type = iconer::app::ISession::id_type;
		using key_type = iconer::app::Room;
		using value_type = iconer::app::User;
		using pointer_type = key_type*;
		using reference = key_type&;
		using const_reference = const key_type&;

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
		std::optional<pointer_type> AcquireEmptyRoom(value_type& user) const;

		[[nodiscard]]
		std::span<const std::byte> AcquireCachedRoomData();

		[[nodiscard]]
		std::optional<pointer_type> FindRoom(id_type id) const noexcept;

		[[nodiscard]]
		bool HasRoom(id_type id) const noexcept;

	private:
		alignas(std::hardware_constructive_interference_size) std::vector<pointer_type> everyRooms{};
		std::atomic_bool isRoomDirty{};
		alignas(std::hardware_constructive_interference_size) std::array<std::byte, maxSerializeRoomDataSize> precachedRoomListData{};
		size_t precachedRoomListDataSize{};

		void AddRoom(pointer_type session);

		void OnRoomMade(pointer_type room, value_type* user);
		void OnUserJoined(pointer_type room, value_type* user, std::int32_t member_cnt);
		void OnUserLeft(pointer_type room, value_type* user, std::int32_t member_cnt);
		void OnRoomClosed(pointer_type room);
	};
}
