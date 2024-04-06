module;
#include <cstdio>
#include <print>
#include <thread>
module TestClient;

int
test::Update()
{
	std::println("Starting receiving...");

	std::jthread th{ Receiver };
	th.detach();

	char commands[256]{};
	static constexpr unsigned cmd_size = sizeof(commands);

	static constexpr std::int32_t test_room_id = 22;

	while (true)
	{
		const auto inputs = ::scanf_s("%s", commands, cmd_size);
		if (EOF != inputs)
		{
			if (1 == inputs)
			{
				const auto& cmd = commands[0];
				if (cmd == 'q')
				{
					break;
				}
				else if (cmd == 'g') // send game start message (by room master)
				{
					// CS_GAME_START
					if (auto sr = SendGameStartPacket(); not sr.has_value())
					{
						std::println("`SendGameStartPacket` error: {}", sr.error());
						return 4;
					}
				}
				else if (cmd == 'r') // send game ready message (by everyone)
				{
					// CS_GAME_LOADED
					if (auto sr = SendGameIsLoadedPacket(); not sr.has_value())
					{
						std::println("`SendGameIsLoadedPacket` error: {}", sr.error());
						return 4;
					}
				}
				else if (cmd == 'v') // request game's version
				{
					if (auto sr = SendRequestVersionPacket(); not sr.has_value())
					{
						std::println("`SendRequestVersionPacket` error: {}", sr.error());
						return 5;
					}
				}
				else if (cmd == 'o') // request list of rooms
				{
					if (auto sr = SendRequestRoomsPacket(); not sr.has_value())
					{
						std::println("`SendRequestRoomsPacket` error: {}", sr.error());
						return 5;
					}
				}
				else if (cmd == 'm') // request room's list of members
				{
					if (auto sr = SendRequestMembersPacket(); not sr.has_value())
					{
						std::println("`SendRequestMembersPacket` error: {}", sr.error());
						return 5;
					}
				}
				else if (cmd == 'c') // create a room
				{
					if (auto sr = SendCreateRoomPacket(L"Test Room"); not sr.has_value())
					{
						std::println("`SendCreateRoomPacket` error: {}", sr.error());
						return 3;
					}
				}
				else if (cmd == 'j') // join a room
				{
					if (auto sr = SendJoinRoomPacket(test_room_id); not sr.has_value())
					{
						std::println("`SendJoinRoomPacket` error: {}", sr.error());
						return 3;
					}
				}
				else if (cmd == 'l') // leave room
				{
					if (auto sr = SendLeaveRoomPacket(); not sr.has_value())
					{
						std::println("`SendLeaveRoomPacket` error: {}", sr.error());
						return 3;
					}
				}
				else if (cmd == 'w')
				{
					localPlayer->y = ++localPlayer->y;
					SendPositionPacket();
				}
				else if (cmd == 'a')
				{
					localPlayer->x = --localPlayer->x;
					SendPositionPacket();
				}
				else if (cmd == 's')
				{
					localPlayer->y = --localPlayer->y;
					SendPositionPacket();
				}
				else if (cmd == 'd')
				{
					localPlayer->x = ++localPlayer->x;
					SendPositionPacket();
				}
				else if (cmd == 'z')
				{
					localPlayer->z = ++localPlayer->z;
					SendPositionPacket();
				}
				else if (cmd == 'x')
				{
					localPlayer->z = --localPlayer->z;
					SendPositionPacket();
				}
			}

			auto cmd = std::string_view{ commands, static_cast<size_t>(inputs) };
			if ("move up" == cmd)
			{
				localPlayer->z = ++localPlayer->z;
				SendPositionPacket();
			}
			else if ("move dw" == cmd)
			{
				localPlayer->z = --localPlayer->z;
				SendPositionPacket();
			}
			else if ("move fw" == cmd)
			{
				localPlayer->y = ++localPlayer->y;
				SendPositionPacket();
			}
			else if ("move bw" == cmd)
			{
				localPlayer->y = --localPlayer->y;
				SendPositionPacket();
			}
			else if ("move lt" == cmd)
			{
				localPlayer->x = --localPlayer->x;
				SendPositionPacket();
			}
			else if ("move rt" == cmd)
			{
				localPlayer->x = ++localPlayer->x;
				SendPositionPacket();
			}
			else if ("quit" == cmd)
			{
				break;
			}

			std::ranges::fill(commands, 0);
		}
	}

	app_socket.Close();
	th.request_stop();

	return 0;
}
