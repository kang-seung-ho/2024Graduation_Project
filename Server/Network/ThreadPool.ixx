module;
#include <type_traits>

#define LIKELY   [[likely]]
#define UNLIKELY [[unlikely]]

export module Iconer.Net.ThreadPool;
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
	class ThreadPool
	{
	public:
		using this_class = ThreadPool<Self, IManager, WorkerCount>;
		using manger_type = IManager;

		ThreadPool() = default;
		~ThreadPool() = default;

		std::expected<void, iconer::net::ErrorCode>
			Initialize()
		{
			if (auto io = IoCompletionPort::Create(); io)
			{
				ioCompletionPort = std::move(io.value());
			}
			else
			{
				return std::unexpected{ io.error() };
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

		bool TryRegister(net::Socket& socket, std::uintptr_t id, iconer::net::ErrorCode& error_code) noexcept
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
			static_assert(requires { std::declval<Self&>().OnProcessTask(std::declval<IManager&>(), std::declval<iconer::net::IoEvent>()); });

			if constexpr (requires{ std::declval<Self&>().OnInitializeWorker(std::declval<IManager&>(), size_t{}); })
			{
				self.OnInitializeWorker(framework, index);
			}
			else if constexpr (requires{ std::declval<Self&>().OnInitializeWorker(std::declval<IManager&>()); })
			{
				self.OnInitializeWorker(framework);
			}
			else if constexpr (requires{ std::declval<Self&>().OnInitializeWorker(size_t{}); })
			{
				self.OnInitializeWorker(index);
			}
			else if constexpr (requires{ std::declval<Self&>().OnInitializeWorker(); })
			{
				self.OnInitializeWorker();
			}
			else if constexpr (requires { Self::OnInitializeWorker(std::declval<IManager&>(), size_t{}); })
			{
				Self::OnInitializeWorker(framework, index);
			}
			else if constexpr (requires { Self::OnInitializeWorker(std::declval<IManager&>()); })
			{
				Self::OnInitializeWorker(framework);
			}
			else if constexpr (requires { Self::OnInitializeWorker(size_t{}); })
			{
				Self::OnInitializeWorker(index);
			}
			else if constexpr (requires { Self::OnInitializeWorker(); })
			{
				Self::OnInitializeWorker();
			}

			self.ArriveWorker();

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

				if (not self.OnProcessTask(framework, std::move(task))) UNLIKELY
				{
					if (not cancel_token.stop_requested()) UNLIKELY
					{
						std::println("An issue appeared at worker {}.", index);
					}

					break;
				}
#else
					if (not framework.ProcessTask(framework.AwaitForTask())) UNLIKELY
					{
						break;
				}
#endif
			}

			if constexpr (requires{ std::declval<Self&>().OnTerminateWorker(std::declval<IManager&>(), size_t{}); })
			{
				self.OnTerminateWorker(framework, index);
			}
			else if constexpr (requires{ std::declval<Self&>().OnTerminateWorker(std::declval<IManager&>()); })
			{
				self.OnTerminateWorker(framework);
			}
			else if constexpr (requires{ std::declval<Self&>().OnTerminateWorker(size_t{}); })
			{
				self.OnTerminateWorker(index);
			}
			else if constexpr (requires{ std::declval<Self&>().OnTerminateWorker(); })
			{
				self.OnTerminateWorker();
			}
			else if constexpr (requires{ Self::OnTerminateWorker(std::declval<IManager&>(), size_t{}); })
			{
				Self::OnTerminateWorker(framework, index);
			}
			else if constexpr (requires{ Self::OnTerminateWorker(std::declval<IManager&>()); })
			{
				Self::OnTerminateWorker(framework);
			}
			else if constexpr (requires{ Self::OnTerminateWorker(size_t{}); })
			{
				Self::OnTerminateWorker(index);
			}
			else if constexpr (requires{ Self::OnTerminateWorker(); })
			{
				Self::OnTerminateWorker();
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

		IoCompletionPort ioCompletionPort;

	private:
		std::array<std::jthread, WorkerCount> myWorkers;
		std::latch workerInitializationSynchronizer{ WorkerCount };

		void ArriveWorker() noexcept
		{
			workerInitializationSynchronizer.arrive_and_wait();
		}

		[[nodiscard]]
		iconer::net::IoEvent AwaitForTask() noexcept
		{
			return ioCompletionPort.WaitForIoResult();
		}

		ThreadPool(const ThreadPool&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;
		ThreadPool(ThreadPool&&) = delete;
		ThreadPool& operator=(ThreadPool&&) = delete;
	};
}
