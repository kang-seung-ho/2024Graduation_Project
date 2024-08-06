module;
#include <concepts>
#include <memory>
#include <string>
#include <string_view>

export module Iconer.Application.IObject;
import Iconer.Utility.Constraints;
import Iconer.Utility.Handler;
import Iconer.Utility.NamedObject;
import Iconer.Utility.Property;
import Iconer.Utility.Atomic;
import Iconer.Utility.MovableAtomic;

export namespace iconer::app
{
	template<typename I, typename S, typename NameChar>
	class [[nodiscard]] IObject
		: protected iconer::util::Handler<I>, protected iconer::util::NamedObject<NameChar>
	{
	public:
		using Super = iconer::util::Handler<I>;
		using HandleType = Super::HandleType;
		using IdType = Super::HandleType;
		using StatusType = S;
		using AtomicType = iconer::util::MovableAtomic<StatusType>;
		using NameType = iconer::util::NamedObject<NameChar>;

		static inline constexpr bool IsNothrowDefaultConstructible = nothrow_default_constructibles<AtomicType>;
		static inline constexpr bool IsNothrowDestructible = nothrow_destructibles<AtomicType>;

		explicit constexpr IObject() noexcept(IsNothrowDefaultConstructible) = default;
		~IObject() noexcept(IsNothrowDestructible) = default;

		explicit constexpr IObject(const IdType& handle)
			noexcept(nothrow_constructible<Super, const IdType&> and nothrow_default_constructibles<AtomicType>)
			: Super(handle), NameType(L"Session")
			, myState()
		{
		}

		explicit constexpr IObject(IdType&& handle)
			noexcept(nothrow_constructible<Super, IdType&&> and nothrow_default_constructibles<AtomicType>)
			: Super(std::move(handle)), NameType(L"Session")
			, myState()
		{
		}

		IObject(IObject&& other)
			noexcept(nothrow_move_constructibles<Super, StatusType, AtomicType, NameType>)
			: Super(std::move(other)), NameType(std::move(other))
			, myState()
		{
			myState = std::move(other.myState);
		}

		IObject& operator=(IObject&& other)
			noexcept(nothrow_move_assignables<Super, StatusType, AtomicType, NameType>)
		{
			Super::operator=(std::move(other));
			NameType::operator=(std::move(other));
			myState = std::move(other.myState);
			return *this;
		}

		using NameType::SetName;
		using NameType::GetName;

		void SetState(const StatusType& state, std::memory_order order = std::memory_order_relaxed)
			noexcept(nothrow_copy_assignables<StatusType> and noexcept(std::declval<AtomicType>().Store(std::declval<const StatusType&>(), std::declval<std::memory_order>())))
			requires copyable<StatusType>
		{
			myState.Store(state, order);
		}

		void SetState(StatusType&& state, std::memory_order order = std::memory_order_relaxed)
			noexcept(nothrow_move_assignables<StatusType> and noexcept(std::declval<AtomicType>().Store(std::declval<StatusType&&>(), std::declval<std::memory_order>())))
			requires movable<StatusType>
		{
			myState.Store(std::move(state), order);
		}

		void SetState(const StatusType& state, std::memory_order order = std::memory_order_relaxed) volatile
			noexcept(nothrow_copy_assignables<StatusType> and noexcept(std::declval<AtomicType>().Store(std::declval<const StatusType&>(), std::declval<std::memory_order>())))
			requires copyable<StatusType>
		{
			myState.Store(state, order);
		}

		void SetState(StatusType&& state, std::memory_order order = std::memory_order_relaxed) volatile
			noexcept(nothrow_move_assignables<StatusType> and noexcept(std::declval<AtomicType>().Store(std::declval<StatusType&&>(), std::declval<std::memory_order>())))
			requires movable<StatusType>
		{
			myState.Store(std::move(state), order);
		}

		StatusType GetState(std::memory_order order = std::memory_order_relaxed) const
			noexcept(noexcept(std::declval<AtomicType>().Load(std::declval<std::memory_order>())))
		{
			return myState.Load(order);
		}

		StatusType GetState(std::memory_order order = std::memory_order_relaxed) const volatile
			noexcept(noexcept(std::declval<AtomicType>().Load(std::declval<std::memory_order>())))
		{
			return myState.Load(order);
		}

		StatusType AcquireState() const
			noexcept(noexcept(GetState(std::declval<std::memory_order>())))
		{
			return GetState(std::memory_order_acquire);
		}

		StatusType AcquireState() const volatile
			noexcept(noexcept(GetState(std::declval<std::memory_order>())))
		{
			return GetState(std::memory_order_acquire);
		}

		void ReleaseState(const StatusType& state)
			noexcept(noexcept(SetState(std::declval<StatusType>(), std::declval<std::memory_order>())))
			requires copyable<StatusType>
		{
			SetState(state, std::memory_order_release);
		}

		void ReleaseState(StatusType&& state)
			noexcept(noexcept(SetState(std::declval<StatusType>(), std::declval<std::memory_order>())))
			requires movable<StatusType>
		{
			SetState(std::move(state), std::memory_order_release);
		}

		void ReleaseState(const StatusType& state) volatile
			noexcept(noexcept(SetState(std::declval<StatusType>(), std::declval<std::memory_order>())))
			requires copyable<StatusType>
		{
			SetState(state, std::memory_order_release);
		}

		void ReleaseState(StatusType&& state) volatile
			noexcept(noexcept(SetState(std::declval<StatusType>(), std::declval<std::memory_order>())))
			requires movable<StatusType>
		{
			SetState(std::move(state), std::memory_order_release);
		}

		bool TryChangeState(const StatusType& from_state, StatusType&& to_state, std::memory_order order = std::memory_order_relaxed) volatile noexcept
			requires movable<StatusType>
		{
			return myState.CompareAndSet(from_state, std::move(to_state), order);
		}

		bool TryChangeState(const StatusType& from_state, const StatusType& to_state, std::memory_order order = std::memory_order_relaxed) noexcept
			requires copyable<StatusType>
		{
			return myState.CompareAndSet(from_state, to_state, order);
		}

		bool TryChangeState(const StatusType& from_state, StatusType&& to_state, std::memory_order order = std::memory_order_relaxed) noexcept
			requires movable<StatusType>
		{
			return myState.CompareAndSet(from_state, std::move(to_state), order);
		}

		bool TryChangeState(const StatusType& from_state, const StatusType& to_state, std::memory_order order = std::memory_order_relaxed) volatile noexcept
			requires copyable<StatusType>
		{
			return myState.CompareAndSet(from_state, to_state, order);
		}

		[[nodiscard]]
		constexpr const IdType& GetID() const& noexcept
		{
			return Super::GetHandle();
		}
		
		[[nodiscard]]
		constexpr const volatile IdType& GetID() const volatile& noexcept
		{
			return Super::GetHandle();
		}
		
		[[nodiscard]]
		constexpr IdType&& GetID() && noexcept
		{
			return std::move(*this).Super::GetHandle();
		}
		
		[[nodiscard]]
		constexpr volatile IdType&& GetID() volatile&& noexcept
		{
			return std::move(*this).Super::GetHandle();
		}

		template<typename S>
		friend class SessionFactory;
		template<typename S>
		friend class ISessionManager;

	protected:
		volatile AtomicType myState;

	private:
		IObject(const IObject&) = delete;
		void operator=(const IObject&) = delete;
	};

	template<typename S>
	class SessionFactory
	{
	public:
		template<typename... Args>
		[[nodiscard]]
		static constexpr S Create(Args&&... args) noexcept(nothrow_constructible<S, Args&&...>)
		{
			S result = S{ std::forward<Args>(args)... };
			result.Awake();
			return result;
		}

		[[nodiscard]]
		static constexpr S* Allocate(size_t count)
		{
			std::allocator<S> alloc{};

			return alloc.allocate(count);
		}

		static constexpr void Deallocate(S* memory, size_t count)
		{
			std::allocator<S> alloc{};

			alloc.deallocate(memory, count);
		}

		template<typename... Args>
		static constexpr S* ConstructAt(S* handle, Args&&... args)
		{
			static_assert(constructible_from<S, Args&&...>);

			S* result = std::construct_at(handle, std::forward<Args>(args)...);
			result->Awake();
			return result;
		}

		static constexpr void DestructAt(S* handle)
			noexcept(noexcept(std::destroy_at(std::declval<S*>())))
		{
			std::destroy_at(handle);
		}
	};
}
