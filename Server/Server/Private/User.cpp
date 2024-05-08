module Iconer.Application.User;
import Iconer.Application.BasicPacket;
import Iconer.Application.Packet;

iconer::app::User::~User()
noexcept(iconer::app::User::nothrowDtor)
{
	if (mySocket.IsAvailable())
	{
		std::exchange(mySocket, iconer::net::Socket{}).Close();
	}
}

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

void
iconer::app::User::Cleanup()
noexcept
{
	SetOperation(AsyncOperations::None);
	SetState(UserStates::None);
	ContextType::Clear();
	myName.clear();
	recvOffset = 0;
	myRoomId = -1;
	myHealth = 100.0f;
	myTeamId = GetID() % 2 == 1 ? Team::Red : Team::Blue;
	myWeaponId = 0;
	isRidingGuardian = false;
}

bool
iconer::app::User::BeginClose()
noexcept
{
	ContextType::Clear();

	SetState(UserStates::Dead);
	SetOperation(AsyncOperations::OpDisconnect);
	return mySocket.CloseAsync(this);
}

bool
iconer::app::User::BeginClose(UserStates prev_state)
noexcept
{
	ContextType::Clear();

	if (TryChangeState(prev_state, UserStates::Dead, std::memory_order_acq_rel))
	{
		SetOperation(AsyncOperations::OpDisconnect);
		return mySocket.CloseAsync(this);
	}
	else
	{
		return false;
	}
}

bool
iconer::app::User::EndClose(UserStates prev_state)
noexcept
{
	return TryChangeState(prev_state, UserStates::None, std::memory_order_acq_rel);
}
