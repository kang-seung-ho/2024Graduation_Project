export module Iconer.App.TaskContext;
import Iconer.Net.IoContext;
import Iconer.Net.Socket;
import <atomic>;

export namespace iconer::app
{
	enum class [[nodiscard]] TaskCategory
	{
		None = 0,

	};

	class [[nodiscard]] TaskContext : public iconer::net::IoContext
	{
	public:
		std::atomic_bool isOccupied;
		std::atomic<TaskCategory> myCategory;
	};
}
