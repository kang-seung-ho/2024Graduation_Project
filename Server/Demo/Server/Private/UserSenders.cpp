module;
#include <utility>
#include <unordered_map>

module Iconer.Application.User;
import Iconer.Application.BasicPacket;
import Iconer.Application.Packet;
import Iconer.Application.SendContextPool;

iconer::app::User::IoResult
iconer::app::User::SendGeneralData(IContext* ctx, const std::byte* static_buffer, size_t size)
noexcept
{
	return mySocket.Send(*ctx, static_buffer, size);
}

iconer::app::User::IoResult
iconer::app::User::SendGeneralData(iconer::app::IContext* ctx, const std::byte* static_buffer, size_t size)
const noexcept
{
	return mySocket.Send(*ctx, static_buffer, size);
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendGeneralData(const std::byte* static_buffer, size_t size) const
{
	auto ctx = SendContextPool::Pop();

	auto io = SendGeneralData(ctx, static_buffer, size);
	return BorrowedIoResult{ std::exchange(io, {}), std::exchange(ctx, {}) };
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendGeneralData(std::unique_ptr<std::byte[]>&& buffer, size_t size)
const
{
	auto ctx = SendContextPool::Pop();
	auto buf = buffer.get();

	ctx->SetBlob(std::move(buffer));
	ctx->SetSize(size);

	return BorrowedIoResult{ SendGeneralData(ctx, buf, size), std::move(ctx) };
}

iconer::app::User::IoResult
iconer::app::User::SendSignInPacket()
noexcept
{
	return mySocket.Send(*this, preSignInPacket.get(), packets::SC_SucceedSignInPacket::WannabeSize());
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendRespondVersionPacket() const
{
	static constinit packets::SC_RespondVersionPacket pk{};
	static const auto buffer = pk.Serialize();
	static constexpr auto size = packets::SC_ReadyForGamePacket::WannabeSize();

	// Preserve the serialized packet
	return SendGeneralData(buffer.get(), size);
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendRespondRoomsPacket(std::span<const std::byte> buffer) const
{
	// Preserve the serialized packet
	return SendGeneralData(buffer.data(), buffer.size_bytes());
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendRespondMembersPacket(std::span<const std::byte> buffer) const
{
	// Preserve the serialized packet
	return SendGeneralData(buffer.data(), buffer.size_bytes());
}

iconer::app::User::IoResult
iconer::app::User::SendRoomCreatedPacket(iconer::app::IContext* room, iconer::app::User::IdType room_id)
const
{
	const packets::SC_RoomCreatedPacket pk{ room_id };
	pk.Write(preRoomCreationPacket.get());

	return mySocket.Send(*room, preRoomCreationPacket.get(), pk.WannabeSize());
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendRoomCreationFailedPacket(const iconer::app::RoomContract& reason)
const
{
	const iconer::app::packets::SC_RoomCreationFailedPacket pk{ reason };

	return SendGeneralData(pk.Serialize(), pk.WannabeSize());
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendRoomJoinedPacket(iconer::app::User::IdType room_id, const User& newbie)
const
{
	iconer::app::packets::SC_RoomOtherJoinedPacket pk{};

	pk.memberInfo = { newbie.GetID(), static_cast<char>(newbie.myTeamId.Load()) };
	std::copy(newbie.myName.cbegin(), newbie.myName.cend(), std::begin(pk.memberInfo.nickname));
	pk.roomId = room_id;

	return SendGeneralData(pk.Serialize(), pk.WannabeSize());
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendRoomJoinFailedPacket(const iconer::app::RoomContract& reason)
const
{
	const iconer::app::packets::SC_RoomJoinFailedPacket pk{ reason };

	return SendGeneralData(pk.Serialize(), pk.WannabeSize());
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendRoomLeftPacket(IdType who, bool is_self)
const
{
	const iconer::app::packets::SC_RoomLeftPacket pk{ who };

	auto ctx = SendContextPool::Pop();
	ctx->SetBlob(pk.Serialize());
	ctx->SetSize(pk.WannabeSize());

	if (is_self)
	{
		ctx->SetOperation(AsyncOperations::OpLeaveRoom);
	}

	return { mySocket.Send(*ctx, ctx->GetBlob().get(), pk.WannabeSize()), std::move(ctx) };
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendCannotStartGamePacket(const GameContract& reason)
const
{
	const iconer::app::packets::SC_FailedGameStartingPacket pk{ static_cast<int>(reason) };

	return SendGeneralData(pk.Serialize(), pk.WannabeSize());
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendChangeTeamPacket(IdType user_id, bool is_red_team)
const
{
	static std::unordered_map<IdType, std::unique_ptr<std::byte[]>> packetmap{};

	const packets::SC_SetTeamPacket pk{ user_id, is_red_team ? 1 : 2 };
	if (not packetmap.contains(user_id))
	{
		return SendGeneralData((packetmap[user_id] = pk.Serialize()).get(), pk.WannabeSize());
	}
	else
	{
		const auto& elem = packetmap[user_id];
		const auto ptr = elem.get();

		pk.Write(ptr);
		return SendGeneralData(ptr, pk.WannabeSize());
	}
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendMakeGameReadyPacket()
const
{
	static const auto pk = iconer::app::packets::Serialize(iconer::app::PacketProtocol::SC_GAME_GETTING_READY);

	auto ctx = SendContextPool::Pop();

	ctx->SetOperation(AsyncOperations::OpGameTicketing);

	// Preserve the serialized packet
	return { mySocket.Send(*ctx, pk.get(), BasicPacket::MinSize()), std::move(ctx) };
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendGameJustStartedPacket()
const
{
	static const auto pk = iconer::app::packets::Serialize(iconer::app::PacketProtocol::SC_GAME_START);

	auto ctx = SendContextPool::Pop();

	// Preserve the serialized packet
	return { mySocket.Send(*ctx, pk.get(), BasicPacket::MinSize()), std::move(ctx) };
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendPositionPacket(iconer::app::User::IdType id, float x, float y, float z) const
{
	const iconer::app::packets::SC_UpdatePositionPacket pk{ id, x, y, z };

	return SendGeneralData(pk.Serialize(), pk.WannabeSize());
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendRotationPacket(iconer::app::User::IdType id, float r, float y, float p)
const
{
	const iconer::app::packets::SC_UpdateRotationPacket pk{ id, r, y, p };

	return SendGeneralData(pk.Serialize(), pk.WannabeSize());
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendCreateCharactersPacket()
const
{
	static const auto pk = iconer::app::packets::Serialize(iconer::app::PacketProtocol::SC_CREATE_PLAYER);

	auto ctx = SendContextPool::Pop();

	return { mySocket.Send(*ctx, pk.get(), BasicPacket::MinSize()), std::move(ctx) };
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendRpcPacket(IdType id, iconer::app::RpcProtocol cat, std::int64_t arg0, std::int32_t arg1)
const
{
	const iconer::app::packets::SC_DeterRpcPacket pk{ id, std::move(cat), std::move(arg0), std::move(arg1) };

	return SendGeneralData(pk.Serialize(), pk.WannabeSize());
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendOldRpcPacket(iconer::app::User::IdType id, std::string&& script, long long arg)
const
{
	const iconer::app::packets::SC_RpcPacket pk{ id, std::move(script), arg };

	return SendGeneralData(pk.Serialize(), pk.WannabeSize());
}

iconer::app::User::BorrowedIoResult
iconer::app::User::SendOldRpcPacket(iconer::app::User::IdType id, std::string_view script, long long arg)
const
{
	const iconer::app::packets::SC_RpcPacket pk{ id, script.data(), script.length(), arg };

	return SendGeneralData(pk.Serialize(), pk.WannabeSize());
}
