module;

export module Iconer.Framework;
import Iconer.Net.IFramework;
import Iconer.Utility.TypeTraits;
import Iconer.Utility.Container.AtomicQueue;
import Iconer.Net.IoResult;
import Iconer.App.UserManager;
import Iconer.App.RoomManager;
import Iconer.App.User;
import Iconer.App.Room;
import Iconer.App.PacketProtocol;
import Iconer.App.Settings;
import Iconer.ThreadPool;
import <cstdint>;
import <cstdio>;
import <array>;
import <string_view>;
import <string>;
import <format>;
import <unordered_map>;

export namespace iconer::app
{
	class [[nodiscard]] PacketContext;
	class [[nodiscard]] SendContext;
}

export using EventDelegate = iconer::method_t<class ServerFramework, void, iconer::app::User&, const std::byte*>;

export class ServerFramework final : public iconer::net::IFramework
{
public:
	using super = iconer::net::IFramework;

	static inline constexpr std::uint16_t serverPort = iconer::app::Settings::serverPort;
	static inline constexpr std::uint16_t reservedSendContextCount = 10000;
	static inline constexpr std::uint16_t reservedPacketContextCount = 5000;

	ServerFramework() = default;
	~ServerFramework() = default;

	iconer::net::IoResult Initialize() override;
	void Startup() override;
	void Cleanup() override;

private:
	alignas(std::hardware_constructive_interference_size) ServerThreadPool myTaskPool{};
	alignas(std::hardware_constructive_interference_size) iconer::app::UserManager userManager{};
	alignas(std::hardware_constructive_interference_size) iconer::app::RoomManager roomManager{};


	alignas(std::hardware_constructive_interference_size) std::unordered_map<iconer::app::PacketProtocol, EventDelegate> packetProcessors{};

	void OnTaskSucceed(iconer::net::IoContext* context, std::uint64_t id, std::uint32_t bytes);
	void OnTaskFailure(iconer::net::IoContext* context, std::uint64_t id, std::uint32_t bytes);

	void OnUserDisconnected(iconer::app::User* user);
	void OnUserJoined(iconer::app::Room* room, iconer::app::User* user, std::int32_t member_cnt);
	void OnUserLeft(iconer::app::Room* room, iconer::app::User* user, std::int32_t member_cnt);
	void OnRoomClosed(iconer::app::Room* room);

	/* Set nickname and assign user's id */
	void EventOnSignIn(iconer::app::User& user, const std::byte* data);
	/* Make a room */
	void EventOnMakeRoom(iconer::app::User& user, const std::byte* data);
	void EventOnFailedToReserveRoom(iconer::app::User& user);
	void EventOnFailedToMakeRoom(iconer::app::User& user);
	/* Join to the room */
	void EventOnJoinRoom(iconer::app::User& user, const std::byte* data);
	/* Notify to self as joined the room */
	void EventOnNotifyRoomJoin(iconer::app::User& user);
	void EventOnFailedToNotifyRoomJoin(iconer::app::User& user);
	/* Exit from the room */
	void EventOnExitRoom(iconer::app::User& user, const std::byte* data);
	/* Match a random room */
	void EventOnSeekRoom(iconer::app::User& user, const std::byte* data);
	/* Send a list of room */
	void EventOnRoomList(iconer::app::User& user, const std::byte* data);
	/* Send a list of member in the room */
	void EventOnUserList(iconer::app::User& user, const std::byte* data);
	/* Broadcast the changed team state to members */
	void EventOnChangeTeam(iconer::app::User& user, const std::byte* data);
	void EventOnNotifyTeamChanged(iconer::app::User& user, std::uint32_t team_id);
	/* Schdule a room for game */
	void EventOnGameStartSignal(iconer::app::User& user, const std::byte* data);
	/* Broadcast game ticket to members */
	void EventOnMakeGame(iconer::app::User& user);
	void EventOnSpreadGameTickets(iconer::app::User& user);
	void EventOnFailedToSpreadGameTickets(iconer::app::User& user);
	/* Mark the player ready and starts game if they all are ready */
	void EventOnGameReadySignal(iconer::app::User& user, const std::byte* data);
	void EventOnGameStarted(iconer::app::User& user);
	void EventOnRpc(iconer::app::User& user, const std::byte* data);

	iconer::net::IoResult AcceptUser(iconer::app::User& user);
	void ReserveUser(iconer::app::User& user) const noexcept;
	iconer::net::IoResult TriggerUser(iconer::app::User& user) const noexcept;
	iconer::net::IoResult StartUser(iconer::app::User& user) const;
	void ProcessPacketsInline(iconer::app::User& user, const size_t& buf_size);
	void RoutePackets(iconer::app::User& user);
	void ProcessPackets(iconer::app::User& user, iconer::app::PacketContext* context, std::uint32_t recv_bytes);
	void AddPacketProcessor(iconer::app::PacketProtocol protocol, const EventDelegate& processor);

	void AddPacketContext(iconer::app::PacketContext* context) noexcept;
	void AddSendContext(iconer::app::SendContext* context) noexcept;
	[[nodiscard]] iconer::app::SendContext* AcquireSendContext() noexcept;

	ServerFramework(const ServerFramework&) = delete;
	ServerFramework& operator=(const ServerFramework&) = delete;
	ServerFramework(ServerFramework&&) = delete;
	ServerFramework& operator=(ServerFramework&&) = delete;
};

export void PrintLn()
{
	std::putchar('\n');
}

export void PrintLn(std::string_view msg)
{
	std::printf("%s\n", msg.data());
}

export void PrintLn(std::wstring_view msg)
{
	std::wprintf(L"%s\n", msg.data());
}

export template<std::formattable<char>... Args>
void PrintLn(std::format_string<std::decay_t<Args>...> msg, Args&&... args)
{
	const std::string content = std::vformat(msg.get(), std::make_format_args(args...));

	std::printf("%s\n", content.data());
}

export template<std::formattable<wchar_t>... Args>
void PrintLn(std::wformat_string<std::decay_t<Args>...> msg, Args&&... args)
{
	const std::wstring content = std::vformat(msg.get(), std::make_format_args<std::wformat_context>(args...));

	std::wprintf(L"%s\n", content.data());
}

