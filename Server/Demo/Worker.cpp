module;
module Demo.Framework;
import Iconer.Application.User;
import Iconer.Application.Resources.String;

void
demo::Worker(demo::Framework& framework, size_t nth)
{
	auto& logger = framework.myLogger;

	framework.CacheSendContexts();

	logger.Log(iconer::app::GetResourceString<0>(), nth);
	framework.workerAwakens.arrive_and_wait();

	while (true)
	{
		auto io_event = framework.WaitForIoResult();

		auto& io_context = io_event.ioContext;
		const auto& io_bytes = io_event.ioBytes;
		const auto& io_id = io_event.eventId;

		if (framework.IsWorkerCancelled()) [[unlikely]] {
			break;
		};

		//logger.DebugLog(iconer::app::GetResourceString<1>(), nth, io_id);

		auto ctx = static_cast<iconer::app::IContext*>(io_context);
		if (not framework.RouteEvent(io_event.isSucceed, io_id, io_bytes, ctx)) [[unlikely]] {
			break;
		};
	}

	logger.DebugLog(iconer::app::GetResourceString<2>(), nth);
}
