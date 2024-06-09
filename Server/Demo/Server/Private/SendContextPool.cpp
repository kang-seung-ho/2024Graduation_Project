module;
#include <memory>
#include <mutex>
#include <unordered_set>
#include <concurrent_queue.h>
#include <concurrent_unordered_set.h>
module Iconer.Application.SendContextPool;

namespace
{
	using queue_type = Concurrency::concurrent_queue<iconer::app::BorrowedSendContext*>;
	constinit queue_type* queue;

	using set_type = Concurrency::concurrent_unordered_set<iconer::app::BorrowedSendContext*>;
	constinit set_type* set;
	
	using sync_set_type = std::unordered_set<iconer::app::BorrowedSendContext*>;
	constinit sync_set_type* syncset;

	using cont_type = sync_set_type;

#if 1941 <= _MSC_VER
	constinit std::mutex contLock{};
#else
	std::mutex contLock{};
#endif
}

void
iconer::app::SendContextPool::Awake()
{
	auto& inst = SendContextPool::Instance();
	const auto address = std::addressof(inst);

	allocator_type allocator{};
	auto init_memory = allocator.allocate(initSendContextsNumber);

	auto build = [=](size_t index) noexcept -> pointer {
		return new (init_memory + index) value_type{};
	};

	for (size_t i = 0; i < initSendContextsNumber; ++i)
	{
		build(i);
	}

	std::allocator<pointer> node_allocator{};
	auto node_memory = node_allocator.allocate(initSendContextsNumber);

	for (size_t i = 0; i < initSendContextsNumber; ++i)
	{
		node_memory[i] = init_memory + i;
	}

	//queue = new cont_type{ node_memory, node_memory + initSendContextsNumber };

	//set = new cont_type{ node_memory, node_memory + initSendContextsNumber };

	syncset = new cont_type{ node_memory, node_memory + initSendContextsNumber };
}

void
iconer::app::SendContextPool::Add(iconer::app::Borrower&& context)
{
	std::lock_guard guard{ contLock };
	syncset->insert(std::exchange(context, Borrower{}));
	//set->insert(std::exchange(context, Borrower{}));
	//queue->push(std::exchange(context, nullptr));
}

iconer::app::Borrower
iconer::app::SendContextPool::Pop()
{
	pointer out = nullptr;

	std::lock_guard guard{ contLock };
	//while (not queue->try_pop(out));

	return Borrower{ syncset->extract(syncset->begin()).value() };
	//return Borrower{ out };
}

bool
iconer::app::SendContextPool::TryPop(iconer::app::Borrower& out)
{
	pointer outro = nullptr;
	//const auto result = queue->try_pop(outro);
	//out = outro;
	//return result;
	return false;
}

void
iconer::app::BorrowedSendContext::ReturnToBase()
{
	Destroy();

	std::lock_guard guard{ contLock };
	syncset->insert(this);
	//queue->push(this);
}

#if false
class EBRMANAGER;

namespace
{
	thread_local constinit size_t threadId = 0;
	thread_local constexpr size_t threadsCount = 8;
	constinit EBRMANAGER ebr[threadsCount];
	constinit long long g_epoch = 0;
}

struct EBRNODE
{
public:
	constexpr EBRNODE() noexcept = default;
	constexpr ~EBRNODE() noexcept = default;

	constexpr EBRNODE(int value) noexcept
		: myValue(value), myNext(nullptr)
		, myEpoch(0)
	{
	}

	int myValue;
	EBRNODE* volatile myNext;
	long long myEpoch;
};

class alignas(64) EBRMANAGER
{
public:
	constexpr EBRMANAGER() = default;

	constexpr ~EBRMANAGER() noexcept
	{
		if (nullptr == localTail) return;

		while (localTail != localHead)
		{
			EBRNODE* t = localHead;
			localHead = localHead->myNext;
			delete t;
		}

		delete localHead;
	}

	constexpr EBRNODE* CreateNode(int key) volatile
	{
		if (localTail != localHead)
		{
			if (localHead->myEpoch < GetLastEpoch())
			{
				EBRNODE* e = localHead;
				localHead = e->myNext;

				e->myValue = key;
				e->myNext = nullptr;
				e->myEpoch = localEpoch;
				return e;
			}
		}

		return new EBRNODE{ key };
	}

	constexpr void RetireNode(EBRNODE* volatile ptr) volatile noexcept
	{
		ptr->myEpoch = localEpoch;
		if (nullptr != localTail)
		{
			localTail->myNext = ptr;
		}

		localTail = ptr;
		if (nullptr == localHead)
		{
			localHead = localTail;
		}
	}

	constexpr void UpdateEpoch() volatile noexcept
	{
		localCounter++;
		if (localCounter < 1000) return;
		localCounter = 0;

		localEpoch = g_epoch++;
	}

private:
	[[nodiscard]]
	constexpr long long GetLastEpoch() const volatile noexcept
	{
		long long oldest_epoch = INT_MAX;

		for (size_t i = 0; i < threadsCount; ++i)
		{
			if (oldest_epoch > ebr[i].localEpoch)
			{
				oldest_epoch = ebr[i].localEpoch;
			}
		}
		return oldest_epoch;
	}

	EBRNODE* localHead, * localTail;
	long long localEpoch;
	int localCounter;
};

class EBR_LFQUEUE
{
public:
	constexpr EBR_LFQUEUE() noexcept
	{
		(void)++threadId;
	}

	~EBR_LFQUEUE() noexcept
	{
		Clear();
		delete head;
	}

	void Awake()
	{
		head = tail = new EBRNODE();
	}

	bool CAS(volatile std::atomic<EBRNODE*>& next, EBRNODE* o_node, EBRNODE* n_node)
	{
		return std::atomic_compare_exchange_strong(
			reinterpret_cast<volatile std::atomic_uintptr_t*>(&next),
			reinterpret_cast<uintptr_t*>(&o_node),
			reinterpret_cast<uintptr_t>(n_node)
		);
	}

	bool CAS(EBRNODE* volatile* next, EBRNODE* o_node, EBRNODE* n_node)
	{
		return std::atomic_compare_exchange_strong(
			reinterpret_cast<volatile std::atomic_uintptr_t*>(next),
			reinterpret_cast<uintptr_t*>(&o_node),
			reinterpret_cast<uintptr_t>(n_node)
		);
	}

	void Enq(const int& value)
	{
		auto& currentPage = ebr[threadId];

		EBRNODE* e = currentPage.CreateNode(value);
		while (true)
		{
			EBRNODE* last = tail;
			EBRNODE* next = last->myNext;

			if (last != tail) continue;

			if (nullptr == next)
			{
				if (true == CAS(&last->myNext, nullptr, e))
				{
					CAS(tail, last, e);
					currentPage.UpdateEpoch();
					return;
				}
			}
			else
			{
				CAS(tail, last, next);
			}
		}
	}

	int Deq()
	{
		auto& currentPage = ebr[threadId];

		while (true)
		{
			EBRNODE* first = head;
			EBRNODE* last = tail;
			if (first != head) continue;

			EBRNODE* next = first->myNext;
			if (nullptr == next)
			{
				currentPage.UpdateEpoch();
				return -1; // EMPTY
			}

			if (first == last)
			{
				CAS(tail, last, next);
				continue;
			}

			int value = next->myValue;
			if (false == CAS(head, first, next))
			{
				continue;
			}

			currentPage.RetireNode(first);
			currentPage.UpdateEpoch();
			return value;
		}
	}

	void Clear() volatile noexcept
	{
		EBRNODE* it = head;
		while (it != tail)
		{
			auto next = it->myNext;
			delete it;

			if (nullptr == next) break;
			it = next;
		}
	}

	void display20()
	{
		EBRNODE* ptr = head.load()->myNext;
		for (int i = 0; i < 20; ++i)
		{
			if (nullptr == ptr) break;

			//cout << ptr->key << ", ";
			ptr = ptr->myNext;
		}
		//cout << endl;
	}

private:
	alignas(64) volatile std::atomic<EBRNODE*> head;
	alignas(64) volatile std::atomic<EBRNODE*> tail;
};

EBR_LFQUEUE my_queue;
#endif
