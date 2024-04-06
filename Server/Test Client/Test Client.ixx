export module TestClient;
import Iconer.Utility.Serializer;
import Iconer.Net;
import Iconer.Net.IpAddress;
import Iconer.Net.EndPoint;
import Iconer.Net.Socket;
import Iconer.Application.RoomContract;
import <cstddef>;
import <type_traits>;
import <expected>;
import <utility>;
import <memory>;
import <string_view>;
import <unordered_map>;
import <algorithm>;

export namespace test
{
	int Awake();
	int Start();
	int Update();
	void Cleanup();
}

namespace test
{
	using IdType = int;

	enum class Levels
	{
		None, Init, Logo, Title, MainMenu, Lobby, Room, Game, GameFinished,
	};

	iconer::net::Socket::IoResult SendSignInPacket();
	iconer::net::Socket::IoResult SendCreateRoomPacket(std::wstring_view title);
	iconer::net::Socket::IoResult SendJoinRoomPacket(IdType room_id);
	iconer::net::Socket::IoResult SendLeaveRoomPacket();
	iconer::net::Socket::IoResult SendRequestVersionPacket();
	iconer::net::Socket::IoResult SendRequestRoomsPacket();
	iconer::net::Socket::IoResult SendRequestMembersPacket();
	iconer::net::Socket::IoResult SendGameStartPacket();
	iconer::net::Socket::IoResult SendGameIsLoadedPacket();
	bool SendPositionPacket();

	const std::byte* ReceiveSignInSucceedPacket(const std::byte* buffer);
	const std::byte* ReceiveSignInFailurePacket(const std::byte* buffer, int& error);
	const std::byte* ReceiveRoomCreatedPacket(const std::byte* buffer, IdType& room_id);
	const std::byte* ReceiveRoomCreationFailedPacket(const std::byte* buffer, iconer::app::RoomContract& error);
	const std::byte* ReceiveRoomJoinedPacket(const std::byte* buffer, IdType& client_id, IdType& room_id);
	const std::byte* ReceiveRoomJoinFailedPacket(const std::byte* buffer, iconer::app::RoomContract& error);
	const std::byte* ReceiveRoomLeftPacket(const std::byte* buffer, IdType& client_id);

	void Receiver();

	Levels currentLevel = Levels::Init;
	inline constexpr std::wstring_view my_name = L"iconer";
	IdType my_id = -1;
	IdType roomId = -1;

	struct FSagaPlayer
	{
		IdType id;
		std::wstring nickname;

		// position
		float x, y, z;
		// orientation
		float look, up, right;
	};

	constexpr std::uint16_t server_port = 40000;
	iconer::net::Socket app_socket{};
	iconer::net::IpAddress server_address{};
	iconer::net::EndPoint server_ep{};

	inline constexpr size_t recvMaxSize = 512;
	iconer::net::IoContext recv_ctx{};
	std::byte* recv_space = new std::byte[recvMaxSize * 2];
	std::span<std::byte, recvMaxSize> recv_buffer{ recv_space, recvMaxSize };
	constinit unsigned long received_bytes = 0;

	std::unordered_map<IdType, FSagaPlayer> everyClients{};
	constinit FSagaPlayer* localPlayer = nullptr;
}
