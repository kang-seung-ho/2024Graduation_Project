export module Iconer.App.SendContext;
import Iconer.App.TaskContext;

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
	};
}
