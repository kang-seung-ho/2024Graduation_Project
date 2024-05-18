#include "Saga/Network/SagaNetworkWorker.h"
#include "Saga/Network/SagaNetworkSubSystem.h"

FSagaNetworkWorker::FSagaNetworkWorker(TObjectPtr<USagaNetworkSubSystem> instance)
	: SubSystemInstance(instance)
	, MyThread()
{
	MyThread = FRunnableThread::Create(this, TEXT("Saga Network Worker Thread"), 128, EThreadPriority::TPri_AboveNormal);
}

FSagaNetworkWorker::~FSagaNetworkWorker()
{
	if (MyThread)
	{
		UE_LOG(LogSagaNetwork, Log, TEXT("[Worker] Network worker is destroyed."));

		MyThread->WaitForCompletion();
		MyThread->Kill();
		delete MyThread;

		MyThread = nullptr;
	}
}

bool
FSagaNetworkWorker::Init()
{
	if (MyThread != nullptr)
	{
		UE_LOG(LogSagaNetwork, Log, TEXT("[Worker] Network worker is created."));

	}
	else
	{
		UE_LOG(LogSagaNetwork, Error, TEXT("[Worker] Network worker was not created."));
	}

	return true;
}

uint32
FSagaNetworkWorker::Run()
{
	while (true)
	{
		if (not SubSystemInstance->Receive())
		{
			break;
		}
	}

	UE_LOG(LogSagaNetwork, Log, TEXT("[Worker] Network worker is done."));

	return 0;
}

void
FSagaNetworkWorker::Exit()
{
}

void
FSagaNetworkWorker::Stop()
{
	UE_LOG(LogSagaNetwork, Log, TEXT("[Worker] Network worker is terminated."));
}
