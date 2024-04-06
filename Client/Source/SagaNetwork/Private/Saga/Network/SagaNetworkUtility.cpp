#include "Saga/Network/SagaNetworkUtility.h"
#include "Templates/UnrealTemplate.h"
#include "Templates/SharedPointer.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

FSocket*
saga::CreateTcpSocket()
{
	return ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
}

ESocketErrors
saga::GetLastError()
noexcept
{
	return ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLastErrorCode();
}

FString
saga::GetLastErrorContents()
{
	auto err_code = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLastErrorCode();

	return FString{ ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetSocketError(err_code) };
}

TSharedRef<FInternetAddr>
saga::MakeEndPoint(const FIPv4Address IpAddress, const int32 Port)
{
	auto Result = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	Result->SetIp(IpAddress.Value);
	Result->SetPort(Port);

	return Result;
}

TSharedRef<FInternetAddr>
saga::MakeEndPointFrom(const FString String, const int32 Port)
{
	FIPv4Address IpAddress;
	FIPv4Address::Parse(String, IpAddress);

	return MakeEndPoint(IpAddress, Port);
}

FIPv4Address
saga::MakeIpAddress(const FString String)
{
	FIPv4Address Result;
	FIPv4Address::Parse(String, Result);

	return Result;
}

int32
saga::RawSend(FSocket* socket, const uint8* buffer, int32 size)
{
	int32 bytes{};
	socket->Send(buffer, size, bytes);

	return bytes;
}

int32
saga::RawSend(FSocket& socket, const uint8* buffer, int32 size)
{
	int32 bytes{};
	socket.Send(buffer, size, bytes);

	return bytes;
}

int32
saga::RawSend(FSocket* socket, const std::byte* buffer, int32 size)
{
	int32 bytes{};
	socket->Send(reinterpret_cast<const uint8*>(buffer), size, bytes);

	return bytes;
}

int32
saga::RawSend(FSocket& socket, const std::byte* buffer, int32 size)
{
	int32 bytes{};
	socket.Send(reinterpret_cast<const uint8*>(buffer), size, bytes);

	return bytes;
}

int32
saga::Send(const TSharedRef<FSocket>& socket, const TUniquePtr<uint8[]>& buffer, int32 size)
{
	if (buffer.IsValid())
	{
		return RawSend(*socket, buffer.Get(), size);
	}
	else
	{
		return 0;
	}
}

int32
saga::Send(const TSharedRef<FSocket>& socket, TUniquePtr<uint8[]>&& buffer, int32 size)
{
	auto bf = MoveTempIfPossible(buffer);
	if (bf.IsValid())
	{
		return RawSend(*socket, buffer.Get(), size);
	}
	else
	{
		return 0;
	}
}

int32
saga::Send(const TSharedRef<FSocket>& socket, const TSharedPtr<uint8[]>& buffer, int32 size)
{
	if (buffer.IsValid())
	{
		return RawSend(*socket, *buffer, size);
	}
	else
	{
		return 0;
	}
}

int32
saga::Send(const TSharedRef<FSocket>& socket, const TSharedRef<uint8[]>& buffer, int32 size)
{
	return Send(socket, buffer.ToSharedPtr(), size);
}
