export module Iconer.Utility.BehaviourTree;
import Iconer.Utility.Constraints;
import <concepts>;
import <variant>;
import <atomic>;

export namespace iconer::util
{
	struct DefaultBehaviour
	{
		template<typename OtherNode>
		static inline constexpr bool Transient = false;
	};

	template<typename Node>
	struct BehaviourTraits
	{
		template<typename OtherNode>
		static inline constexpr bool IsTransient = Node::template Transient<OtherNode>;
	};

	template<typename... NodeTs>
		requires argument_available<NodeTs...>
	class [[nodiscard]] BehaviourTree final
	{
	private:
		using data_t = std::variant<NodeTs...>;

	public:
		static_assert(default_initializables<NodeTs...>);

		constexpr BehaviourTree() noexcept = default;
		constexpr ~BehaviourTree() noexcept = default;

		template<typename U>
		constexpr BehaviourTree(U&& node) noexcept(std::is_nothrow_constructible_v<data_t, U&&>)
			: currentState(std::forward<U>(node))
		{
		}

		template<typename Node>
		constexpr bool TryTranslate() noexcept(std::is_nothrow_assignable_v<data_t, Node>)
		{
			data_t state = currentState.load(std::memory_order_acquire);

			if (std::holds_alternative<Node>(state))
			{
				currentState.store(state, std::memory_order_release);
				return true;
			}
			else if (state.valueless_by_exception())
			{
				state = Node{};
				currentState.store(state, std::memory_order_release);
				return true;
			}
			else
			{
				static constexpr auto visitor = []<typename V>(V && arg) noexcept -> bool
				{
					if constexpr (BehaviourTraits<std::decay_t<V>>::template IsTransient<Node>)
					{
						return true;
					}
					else
					{
						return false;
					}
				};

				const bool result = std::visit(visitor, state);
				if (result)
				{
					state = Node{};
				}
				currentState.store(state, std::memory_order_release);
				return result;
			}
		}

		template<typename Fn>
		constexpr auto Visit(Fn&& fn) noexcept(noexcept(std::visit(std::forward<Fn>(fn), std::declval<data_t>())))
		{
			if constexpr (std::is_same_v<decltype(std::visit(std::forward<Fn>(fn), std::declval<data_t>())), void>)
			{
				data_t state = currentState.load(std::memory_order_acquire);
				std::visit(std::forward<Fn>(fn), std::move(state));
				currentState.store(state, std::memory_order_release);
			}
			else
			{
				data_t state = currentState.load(std::memory_order_acquire);

				auto result = std::visit(std::forward<Fn>(fn), std::move(state));
				currentState.store(state, std::memory_order_release);

				return result;
			}
		}

		template<typename Fn>
		constexpr auto Visit(Fn&& fn) const noexcept(noexcept(std::visit(std::forward<Fn>(fn), std::declval<data_t>())))
		{
			if constexpr (std::is_same_v<decltype(std::visit(std::forward<Fn>(fn), std::declval<data_t>())), void>)
			{
				std::visit(std::forward<Fn>(fn), currentState.load(std::memory_order_acq_rel));
			}
			else
			{
				return std::visit(std::forward<Fn>(fn), currentState.load(std::memory_order_acq_rel));
			}
		}

		constexpr BehaviourTree(const BehaviourTree&) noexcept = default;
		constexpr BehaviourTree(BehaviourTree&&) noexcept = default;
		constexpr BehaviourTree& operator=(const BehaviourTree&) noexcept = default;
		constexpr BehaviourTree& operator=(BehaviourTree&&) noexcept = default;

	private:
		std::atomic<data_t> currentState;
	};
}
