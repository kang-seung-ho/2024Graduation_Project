#include "Saga/Network/SagaBasicPacket.h"
#include "Saga/Serializer.h"

std::unique_ptr<std::byte[]>
FSagaBasicPacket::Serialize()
const
{
	return saga::Serializes(static_cast<uint8>(myProtocol), mySize);
}

std::byte*
FSagaBasicPacket::Write(std::byte* buffer)
const
{
	return saga::Serialize(saga::Serialize(buffer, static_cast<uint8>(myProtocol)), mySize);
}

const std::byte*
FSagaBasicPacket::Read(const std::byte* buffer)
{
	return saga::Deserialize(saga::Deserialize(buffer, myProtocol), mySize);
}
