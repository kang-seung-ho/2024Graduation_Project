export module Iconer.App.PacketContext;
import Iconer.Utility.TypeTraits;
export import Iconer.App.TaskContext;
import <cstddef>;
import <memory>;

export namespace iconer::app
{
	class [[nodiscard]] PacketContext : public TaskContext
	{
	public:
		using super = iconer::app::TaskContext;
		using this_class = PacketContext;

		constexpr PacketContext() noexcept
			: super(TaskCategory::OpPacketProcess)
			, myData()
		{}
		
		constexpr PacketContext(std::unique_ptr<std::byte[]>&& data) noexcept
			: super(TaskCategory::OpPacketProcess)
			, myData(std::move(data))
		{}

		constexpr ~PacketContext() noexcept = default;

		std::unique_ptr<std::byte[]> myData;
	};
}
