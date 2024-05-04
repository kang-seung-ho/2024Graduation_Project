module;
module Iconer.Application.User;
import Iconer.Application.BasicPacket;
import Iconer.Application.Packet;

void
iconer::app::User::Awake()
{
	mySocket.IsAddressReusable = true;

	ContextType::Clear();
	recvOffset = 0;
	myName.reserve(nicknameLength);

	preSignInPacket = std::make_unique_for_overwrite<std::byte[]>(packets::SC_SucceedSignInPacket::WannabeSize());
	preRoomCreationPacket = std::make_unique_for_overwrite<std::byte[]>(packets::SC_RoomCreatedPacket::WannabeSize());

	const packets::SC_SucceedSignInPacket signin_pk{ myHandle };
	signin_pk.Write(preSignInPacket.get());
}
