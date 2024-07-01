#include "Framework.hpp"

import Iconer.App.User;
import Iconer.App.TaskContext;
import Iconer.App.PacketSerializer;

void
ServerFramework::EventOnMakeRoom(iconer::app::User& user, std::byte* data)
{}

void
ServerFramework::EventOnJoinRoom(iconer::app::User& user, std::byte* data)
{}

void
ServerFramework::EventOnExitRoom(iconer::app::User& user, std::byte* data)
{}

void
ServerFramework::EventOnSeekRoom(iconer::app::User& user, std::byte* data)
{}

void
ServerFramework::EventOnRoomList(iconer::app::User& user, std::byte* data)
{}
