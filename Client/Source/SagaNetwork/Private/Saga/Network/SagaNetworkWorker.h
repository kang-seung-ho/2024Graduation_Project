#pragma once
#include "SagaNetwork.h"
#include <UObject/ObjectPtr.h>
#include <HAL/Runnable.h>
#include <HAL/RunnableThread.h>

class SAGANETWORK_API FSagaNetworkWorker final : public FRunnable, public FNoncopyable
{
public:
	FSagaNetworkWorker(TObjectPtr<class USagaNetworkSubSystem> instance);
	~FSagaNetworkWorker();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	virtual void Stop() override;

private:
	FRunnableThread* MyThread;

	TObjectPtr<class USagaNetworkSubSystem> SubSystemInstance;
};
