export module Iconer.App.SendContext;
import Iconer.App.TaskContext;
import <cstddef>;
import <memory>;

export namespace iconer::app
{
	class [[nodiscard]] SendContext : public TaskContext
	{
	public:
		using super = TaskContext;
		using this_class = SendContext;

		constexpr SendContext() noexcept
			: super(TaskCategory::OpSend)
		{}

		std::unique_ptr<std::byte[]> myBuffer{};
	};
}
