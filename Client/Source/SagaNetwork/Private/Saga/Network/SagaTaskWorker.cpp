#include "Saga/Network/SagaTaskWorker.h"
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
	if (MyThread != nullptr)
	{
		UE_LOG(LogSagaNetwork, Log, TEXT("[Worker] Task worker is created."));
	}
	else
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("[Worker] Task worker was not created."));
	}

	return true;
}

uint32
FSagaTaskWorker::Run()
{
	while (true)
	{
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
{
}

void
FSagaTaskWorker::Stop()
{
	UE_LOG(LogSagaNetwork, Log, TEXT("[Worker] Task worker is terminated."));
}
