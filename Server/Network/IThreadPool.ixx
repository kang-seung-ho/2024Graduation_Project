export module Iconer.Net.IThreadPool;
import Iconer.Utility.Delegate;
export import Iconer.Net.Socket;
export import Iconer.Net.IoContext;
export import Iconer.Net.IoCompletionPort;
import <expected>;
import <vector>;
import <thread>;
import <latch>;

export namespace iconer::net
{
	class IThreadPool
	{
	public:
		using this_class = IThreadPool;

		IoCompletionPort ioCompletionPort;

		iconer::util::Delegate<void, size_t> eventOnWorkerInitialized;
		iconer::util::Delegate<void, size_t> eventOnWorkerAnnihilated;

		iconer::util::Delegate<void, IoContext*, std::uint64_t, std::uint32_t> eventOnTaskSucceed;
		iconer::util::Delegate<void, IoContext*, std::uint64_t, std::uint32_t> eventOnTaskFailure;

		constexpr IThreadPool(const ptrdiff_t concurrency)
			: ioCompletionPort()
			, eventOnWorkerInitialized(), eventOnWorkerAnnihilated()
			, eventOnTaskSucceed(), eventOnTaskFailure()
			, maxWorkerNumber(static_cast<size_t>(concurrency))
			, myWorkers(), workerInitializationSynchronizer(concurrency)
		{
			myWorkers.reserve(maxWorkerNumber);
		}

		virtual ~IThreadPool() = default;

		virtual std::expected<void, iconer::net::ErrorCode> Initialize();
		virtual void Startup();
		virtual void Cleanup();

		std::expected<void, iconer::net::ErrorCode>
			Register(iconer::net::Socket& socket, const std::uintptr_t& id)
			const noexcept
		{
			return ioCompletionPort.Register(socket, id);
		}

		std::expected<void, iconer::net::ErrorCode>
			Register(iconer::net::Socket& socket, std::uintptr_t&& id)
			const noexcept
		{
			return ioCompletionPort.Register(socket, std::move(id));
		}

		bool
			TryRegister(iconer::net::Socket& socket, std::uintptr_t id, iconer::net::ErrorCode& error_code)
			const noexcept
		{
			return ioCompletionPort.TryRegister(socket, id, error_code);
		}

		bool
			Schedule(IoContext& context, std::uintptr_t id, unsigned long infobytes = 0)
			const noexcept
		{
			return ioCompletionPort.Schedule(context, id, infobytes);
		}

		bool
			Schedule(IoContext* const context, std::uintptr_t id, unsigned long infobytes = 0)
			const noexcept
		{
			return ioCompletionPort.Schedule(context, id, infobytes);
		}

		bool
			Schedule(volatile IoContext& context, std::uintptr_t id, unsigned long infobytes = 0)
			const noexcept
		{
			return ioCompletionPort.Schedule(context, id, infobytes);
		}

		bool
			Schedule(volatile IoContext* const context, std::uintptr_t id, unsigned long infobytes = 0)
			const noexcept
		{
			return ioCompletionPort.Schedule(context, id, infobytes);
		}

		void StopWorkers() noexcept
		{
			if (0 < myWorkers.size())
			{
				for (auto& th : myWorkers)
				{
					th.request_stop();
				}
			}
		}

		[[nodiscard]]
		constexpr size_t GetMaxWorkerNumber() const noexcept
		{
			return maxWorkerNumber;
		}
		
		[[nodiscard]]
		constexpr size_t GetWorkerCount() const noexcept
		{
			return myWorkers.size();
		}

	protected:
		static void Worker(std::stop_token&& cancel_token, IThreadPool& self, size_t index);

	private:
		size_t maxWorkerNumber;
		std::vector<std::jthread> myWorkers;
		std::latch workerInitializationSynchronizer;

		bool ProcessTask(iconer::net::IoEvent task);

		void ArriveWorker() noexcept
		{
			workerInitializationSynchronizer.arrive_and_wait();
		}

		[[nodiscard]]
		iconer::net::IoEvent AwaitForTask() const noexcept
		{
			return ioCompletionPort.WaitForIoResult();
		}

		IThreadPool(const IThreadPool&) = delete;
		IThreadPool& operator=(const IThreadPool&) = delete;
		IThreadPool(IThreadPool&&) = delete;
		IThreadPool& operator=(IThreadPool&&) = delete;
	};
}
