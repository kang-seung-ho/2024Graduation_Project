#include "Saga/Network/SagaTaskWorker.h"
#include <Logging/LogMacros.h>
#include <UObject/Object.h>

#include "Saga/Network/SagaNetworkSubSystem.h"

FSagaTaskWorker::FSagaTaskWorker(TObjectPtr<USagaNetworkSubSystem> instance)
	: MyThread()
	, SubSystemInstance(instance)
{
	MyThread = FRunnableThread::Create(this, TEXT("Saga Task Worker Thread"), 128, EThreadPriority::TPri_AboveNormal);
}

FSagaTaskWorker::~FSagaTaskWorker()
{
	if (MyThread)
	{
		UE_LOG(LogSagaNetwork, Log, TEXT("[Worker] Task worker is destroyed."));

		MyThread->Kill();
		delete MyThread;

		MyThread = nullptr;
	}
}

bool
FSagaTaskWorker::Init()
{
	return true;
}

uint32
FSagaTaskWorker::Run()
{
	while (true)
	{
		if (not IsValid(SubSystemInstance))
		{
			UE_LOG(LogSagaNetwork, Error, TEXT("[Worker] The network subsystem is broken."));
			return 1;
		}

		if (not SubSystemInstance->ProcessPackets())
		{
			break;
		}
	}

	UE_LOG(LogSagaNetwork, Log, TEXT("[Worker] Task worker is done."));

	return 0;
}

void
FSagaTaskWorker::Exit()
{}

void
FSagaTaskWorker::Stop()
{
	UE_LOG(LogSagaNetwork, Log, TEXT("[Worker] Task worker is terminated."));
}
