#include "Saga/Network/SagaNetworkSubSystem.h"
#include <Sockets.h>
#include <Templates/UniquePtr.h>

#include "Saga/Network/SagaBasicPacket.h"

bool
USagaNetworkSubSystem::Receive()
{
	if (IsOfflineMode())
	{
		//UE_LOG(LogSagaNetwork, Log, TEXT("[Packet] Receiving... (Offline Mode)"));

		return true;
	}
	else // IF (IsOfflineMode)
	{
		//UE_LOG(LogSagaNetwork, Log, TEXT("[Packet] Receiving..."));

		const auto recv_buffer = recvBuffer.GetData();
		int32 read_bytes{};

		// Checks
		//taskBarrier.wait();

		if (not clientSocket->Recv(recv_buffer + recvBytes
			, recvLimit - recvBytes
			, read_bytes))
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("[Packet] Receiving has been failed!"));
			BroadcastOnDisconnected();

			// Return #1
			return false;
		}
		else if (read_bytes <= 0)
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("[Packet] Received %d byte!"), read_bytes);
			BroadcastOnDisconnected();

			// Return #2
			return false;
		}

		// Increases the bytes
		recvBytes += read_bytes;

		int32 read_offset{};

		// Peeks packets as many as possible
		while (FSagaBasicPacket::MinSize() <= recvBytes)
		{
			// The current state buffer
			const auto read_buffer = recv_buffer + read_offset;

			// Reads the protocol and size
			FSagaBasicPacket basic_pk{ EPacketProtocol::UNKNOWN };
			basic_pk.Read(reinterpret_cast<std::byte*>(read_buffer));

			// Validates
			if (basic_pk.mySize <= 0)
			{
				UE_LOG(LogSagaNetwork, Error, TEXT("[Packet] Packet's size was %d!"), basic_pk.mySize);

				// Return #3
				return false;
			}
			else if (basic_pk.mySize <= recvBytes)
			{
#if WITH_EDITOR
				//const auto ename = UEnum::GetValueAsString(basic_pk.myProtocol);
				//UE_LOG(LogSagaNetwork, Log, TEXT("[Packet] Received a packet (%s, %d)."), *ename, basic_pk.mySize);
#endif

				// Make a serialized packet
				{
					const auto size = static_cast<SIZE_T>(basic_pk.mySize);
					TUniquePtr<uint8[]> buffer = MakeUnique<uint8[]>(size);

					const auto dst = buffer.Get();
					std::memcpy(dst, read_buffer, static_cast<size_t>(size));

					//taskQueue.push(FSagaSerializedPacket{ MoveTemp(buffer) });
					taskQueue.push(MoveTemp(buffer));
				}

				recvBytes -= basic_pk.mySize;
				read_offset += basic_pk.mySize;
			}
			else
			{
				UE_LOG(LogSagaNetwork, Log, TEXT("[Packet] A receive phase is splited."));

				break;
			}
		} // WHILE (true)

		if (0 < read_offset)
		{
			const auto adv_buffer = recv_buffer + read_offset;
			const auto remained_size = static_cast<size_t>(recvLimit - read_offset);
			std::memcpy(recv_buffer, recv_buffer + read_offset, remained_size);
			std::memset(adv_buffer, 0, remained_size);
			//UE_LOG(LogSagaNetwork, Log, TEXT("[Packet] Buffer is pulled by %d bytes"), read_offset);
		}
	} // IF NOT (IsOfflineMode)

	return true;
}

bool
USagaNetworkSubSystem::ProcessPackets()
{
	// The network worker is an only producer
	// The task worker is an only consumer

	//UE_LOG(LogSagaNetwork, Log, TEXT("[Packet] Routing %d tasks..."), taskQueue.was_size());

	// Processing tasks continuosly
	while (true)
	{
		//FSagaSerializedPacket task{};
		TUniquePtr<uint8[]> task{};

		if (taskQueue.try_pop(task))
		{
			// Process a task
			{
				//const auto process_buffer = task.myData.Get();
				const auto process_buffer = task.Get();

				FSagaBasicPacket basic_pk{ EPacketProtocol::UNKNOWN };
				basic_pk.Read(reinterpret_cast<std::byte*>(process_buffer));

				if (basic_pk.mySize <= 0)
				{
					// Invalid task
					continue;
				}

				if (nullptr == this or not IsConnected())
				{
					return false;
				}

				// Routes stacked task
				//RouteTasks(MoveTemp(task.myData), basic_pk.myProtocol, basic_pk.mySize);
				RouteTasks(MoveTemp(task), basic_pk.myProtocol, basic_pk.mySize);
			}
			// Proceed a packet
		}
		else // IF (try_pop)
		{
			break;
		}
	} // wHILE (TRUE)

	return IsConnected();
}
