module;
#include <type_traits>

#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

export module Iconer.Net.IThreadPool;
import Iconer.Utility.Delegate;
export import Iconer.Net.Socket;
export import Iconer.Net.IoContext;
export import Iconer.Net.IoCompletionPort;
import <expected>;
import <array>;
import <thread>;
import <latch>;

#if _DEBUG
import <print>;
#endif

export namespace iconer::net
{
	template<typename Self, typename IManager, size_t WorkerCount>
	class IThreadPool
	{
	public:
		using this_class = IThreadPool<Self, IManager, WorkerCount>;
		using manger_type = IManager;

		static inline constexpr size_t workerCount = WorkerCount;

		IoCompletionPort ioCompletionPort;

		iconer::util::Delegate<void> eventOnInitialied;
		iconer::util::Delegate<void, size_t> eventOnStarted;
		iconer::util::Delegate<void> eventOnDestructed;
		iconer::util::Delegate<void, IoContext*, std::uint64_t, std::uint32_t> eventOnTaskSucceed;
		iconer::util::Delegate<void, IoContext*, std::uint64_t, std::uint32_t> eventOnTaskFailure;

		IThreadPool() = default;
		~IThreadPool() = default;

		std::expected<void, iconer::net::ErrorCode> Initialize()
		{
			if (auto io = IoCompletionPort::Create(); io) LIKELY
			{
				ioCompletionPort = std::move(io.value());
			}
			else UNLIKELY
			{
				return std::unexpected{ io.error() };
			};

			if (eventOnInitialied.IsBound())
			{
				eventOnInitialied.Broadcast();
			}

			return {};
		}

		void Startup(IManager& manager)
		{
			size_t index{};
			for (auto& th : myWorkers)
			{
				th = std::jthread
				{
					&this_class::Worker,
					std::ref(this->Cast()),
					std::ref(manager),
					++index
				};
			}
		}

		void Cleanup()
		{
			for (auto& th : myWorkers)
			{
				th.request_stop();
			}

			ioCompletionPort.Destroy();
		}

		std::expected<void, iconer::net::ErrorCode> Register(iconer::net::Socket& socket, const std::uintptr_t& id) noexcept
		{
			return ioCompletionPort.Register(socket, id);
		}

		std::expected<void, iconer::net::ErrorCode> Register(iconer::net::Socket& socket, std::uintptr_t&& id) noexcept
		{
			return ioCompletionPort.Register(socket, std::move(id));
		}

		bool TryRegister(iconer::net::Socket& socket, std::uintptr_t id, iconer::net::ErrorCode& error_code) noexcept
		{
			return ioCompletionPort.TryRegister(socket, id, error_code);
		}

		bool Schedule(IoContext& context, std::uintptr_t id, unsigned long infobytes = 0) noexcept
		{
			return ioCompletionPort.Schedule(context, id, infobytes);
		}

		bool Schedule(IoContext* const context, std::uintptr_t id, unsigned long infobytes = 0) noexcept
		{
			return ioCompletionPort.Schedule(context, id, infobytes);
		}

		bool Schedule(volatile IoContext& context, std::uintptr_t id, unsigned long infobytes = 0) noexcept
		{
			return ioCompletionPort.Schedule(context, id, infobytes);
		}

		bool Schedule(volatile IoContext* const context, std::uintptr_t id, unsigned long infobytes = 0) noexcept
		{
			return ioCompletionPort.Schedule(context, id, infobytes);
		}

	protected:
		static void Worker(std::stop_token&& cancel_token, Self& self, IManager& framework, size_t index)
		{
			self.ArriveWorker();

			if (self.eventOnStarted.IsBound())
			{
				self.eventOnStarted.Broadcast(index);
			}

			while (true)
			{
				if (cancel_token.stop_requested()) UNLIKELY
				{
					break;
				}

#if _DEBUG
				iconer::net::IoEvent task = self.AwaitForTask();
				std::println("[Task({})] id={} ({} bytes)"
					, task.isSucceed, task.eventId, task.ioBytes);

				if (not self.ProcessTask(std::move(task))) UNLIKELY
				{
					if (not cancel_token.stop_requested()) UNLIKELY
					{
						std::println("An issue appeared at worker {}.", index);
					}

					break;
				}
#else
					if (not ProcessTask(framework.AwaitForTask())) UNLIKELY
					{
						break;
					}
#endif
			}

			if (self.eventOnDestructed.IsBound())
			{
				self.eventOnDestructed.Broadcast();
			}
		}

		[[nodiscard]]
		constexpr Self& Cast() & noexcept
		{
			return static_cast<Self&>(*this);
		}

		[[nodiscard]]
		constexpr const Self& Cast() const& noexcept
		{
			return static_cast<const Self&>(*this);
		}

		[[nodiscard]]
		constexpr Self&& Cast() && noexcept
		{
			return static_cast<Self&&>(*this);
		}

		[[nodiscard]]
		constexpr const Self&& Cast() const&& noexcept
		{
			return static_cast<const Self&&>(*this);
		}

	private:
		std::array<std::jthread, WorkerCount> myWorkers;
		std::latch workerInitializationSynchronizer{ WorkerCount };

		bool ProcessTask(iconer::net::IoEvent task)
		{
			if (task.isSucceed) LIKELY
			{
				if (eventOnTaskSucceed.IsBound())
				{
					eventOnTaskSucceed.Broadcast(task.ioContext, task.eventId, task.ioBytes);
				}
			}
			else UNLIKELY
			{
				if (eventOnTaskFailure.IsBound())
				{
					eventOnTaskFailure.Broadcast(task.ioContext, task.eventId, task.ioBytes);
				}
			}

			return task.isSucceed;
		}

		void ArriveWorker() noexcept
		{
			workerInitializationSynchronizer.arrive_and_wait();
		}

		[[nodiscard]]
		iconer::net::IoEvent AwaitForTask() noexcept
		{
			return ioCompletionPort.WaitForIoResult();
		}

		IThreadPool(const IThreadPool&) = delete;
		IThreadPool& operator=(const IThreadPool&) = delete;
		IThreadPool(IThreadPool&&) = delete;
		IThreadPool& operator=(IThreadPool&&) = delete;
	};
}
