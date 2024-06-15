module;
#include <type_traits>
#include <memory>
#include <utility>
#include <variant>

export module Iconer.Utility.Delegate;
import Iconer.Utility.TypeTraits;
import Iconer.Utility.InvokeTraits;

export namespace iconer::util
{
	void AA1(int) {}
	void AA2() {}

	template<typename T>
	class Delegate
	{
	public:
		void Broadcast(T arg)
		{
			auto AA4 = [this]() noexcept -> void
			{

			};

			using AA4_t = decltype(AA4);

			AA4.operator()();
		}
	};
}
