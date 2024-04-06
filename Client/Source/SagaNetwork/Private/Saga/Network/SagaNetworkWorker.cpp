#include "Saga/Network/SagaNetworkWorker.h"
#include "Sockets.h"

#include "Saga/Network/SagaNetworkSettings.h"
#include "Saga/Network/SagaBasicPacket.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

FSagaNetworkWorker::FSagaNetworkWorker(TObjectPtr<USagaNetworkSubSystem> instance)
	: SubSystemInstance(MoveTempIfPossible(instance))
{
	MyThread = FRunnableThread::Create(this, TEXT("Worker Thread"));
}

FSagaNetworkWorker::~FSagaNetworkWorker()
{
	if (MyThread)
	{
		// 스레드 종료
		MyThread->WaitForCompletion();
		MyThread->Kill();
		delete MyThread;

		MyThread = nullptr;
	}
}

bool
FSagaNetworkWorker::Init()
{
	UE_LOG(LogSagaNetwork, Display, TEXT("Worker has been initialized."));

	return (MyThread != nullptr);
}

uint32
FSagaNetworkWorker::Run()
{
	auto& socket = SubSystemInstance->clientSocket;

	constexpr int32 MaxReceiveSize = 512;
	int32 recv_bytes{};
	uint8 recv_buffer[MaxReceiveSize]{};
	std::byte* alt_buffer = reinterpret_cast<std::byte*>(recv_buffer);

	auto PullReceiveBuffer = [&](int32 packet_size) {
		const auto memsz = static_cast<size_t>(MaxReceiveSize - packet_size);
		std::memcpy(recv_buffer, recv_buffer + recv_bytes, memsz);
		std::memset(recv_buffer + recv_bytes, 0, memsz);

		recv_bytes -= packet_size;
		UE_LOG(LogSagaNetwork, Log, TEXT("Remained receive bytes are %d"), recv_bytes);
		};

	while (true)
	{
#if true
		// Recv 작업 진행
		int32 temp_recv_bytes{};

		if constexpr (not saga::IsOfflineMode)
		{
			if (not socket->Recv(recv_buffer + recv_bytes
				, MaxReceiveSize - recv_bytes
				, temp_recv_bytes))
			{
				UE_LOG(LogSagaNetwork, Error, TEXT("Receiving has been failed!"));

				SubSystemInstance->CallPureFunctionOnGameThread([this]()
					{
						SubSystemInstance->BroadcastOnDisconnected();
					}
				);
				return 1;
			}
			else if (temp_recv_bytes <= 0)
			{
				UE_LOG(LogSagaNetwork, Error, TEXT("Received 0 byte!"));

				SubSystemInstance->CallPureFunctionOnGameThread([this]()
					{
						SubSystemInstance->BroadcastOnDisconnected();
					}
				);
				return 2;
			}

			recv_bytes += temp_recv_bytes;

			// 패킷 검증 필요
			while (FSagaBasicPacket::MinSize() <= recv_bytes)
			{
				FSagaBasicPacket basic_pk{ EPacketProtocol::UNKNOWN };
				basic_pk.Read(alt_buffer);

				if (basic_pk.mySize <= 0)
				{
					UE_LOG(LogSagaNetwork, Error, TEXT("Packet's size was zero!"));
					return 3;
				}

				if (recv_bytes <= basic_pk.mySize)
				{
					auto ename = UEnum::GetValueAsString(basic_pk.myProtocol);
					UE_LOG(LogSagaNetwork, Log, TEXT("Received a packet (%s)"), *ename);

					SubSystemInstance->RouteEvents(alt_buffer, basic_pk.myProtocol, basic_pk.mySize);

					UE_LOG(LogSagaNetwork, Log, TEXT("Packet's size was %d"), basic_pk.mySize);

					PullReceiveBuffer(basic_pk.mySize);
				}
				else
				{
					UE_LOG(LogSagaNetwork, Log, TEXT("A receive phase is done"));
					break; // while (true) #2
				}
			} // while (true) #2
		} // if constexpr (IsOfflineMode)
	} // while (true) #1
#endif

	return 0;
}

void
FSagaNetworkWorker::Exit()
{
}

void
FSagaNetworkWorker::Stop()
{
}
