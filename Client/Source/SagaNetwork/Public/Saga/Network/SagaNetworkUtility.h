#pragma once
#include "SagaNetwork.h"
#include "Interfaces/IPv4/IPv4Address.h"

namespace saga
{
	[[nodiscard]]
	FSocket* CreateTcpSocket();

	[[nodiscard]]
	ESocketErrors GetLastError() noexcept;

	[[nodiscard]]
	FString GetLastErrorContents();

	[[nodiscard]]
	TSharedRef<FInternetAddr> MakeEndPoint(const FIPv4Address IpAddress, const int32 Port);

	[[nodiscard]]
	TSharedRef<FInternetAddr> MakeEndPointFrom(const FString String, const int32 Port);

	[[nodiscard]]
	FIPv4Address MakeIpAddress(const FString String);

	int32 RawSend(FSocket* socket, const uint8* buffer, int32 size);
	int32 RawSend(FSocket& socket, const uint8* buffer, int32 size);
	int32 RawSend(FSocket* socket, const std::byte* buffer, int32 size);
	int32 RawSend(FSocket& socket, const std::byte* buffer, int32 size);

	int32 Send(const TSharedRef<FSocket>& socket, const TUniquePtr<uint8[]>& buffer, int32 size);
	int32 Send(const TSharedRef<FSocket>& socket, TUniquePtr<uint8[]>&& buffer, int32 size);
	int32 Send(const TSharedRef<FSocket>& socket, const TSharedPtr<uint8[]>& buffer, int32 size);
	int32 Send(const TSharedRef<FSocket>& socket, const TSharedRef<uint8[]>& buffer, int32 size);
};
