#include "Saga/Network/SagaNetworkWorker.h"
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
	UE_LOG(LogSagaNetwork, Log, TEXT("[Worker] Worker is initialized."));

	return (MyThread != nullptr);
}

uint32
FSagaNetworkWorker::Run()
{
	while (SubSystemInstance->Receive())
	{
	}

	return 0;
}

void
FSagaNetworkWorker::Exit()
{}

void
FSagaNetworkWorker::Stop()
{
	UE_LOG(LogSagaNetwork, Log, TEXT("[Worker] Worker is terminated."));
}
