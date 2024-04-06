export module Iconer.Utility.MovableAtomic:Storage;
import Iconer.Utility.Constraints;
import <atomic>;

namespace iconer::util
{
	template<specializations<std::atomic> Atomic>
	class MovableAtomicStorage
	{
	public:
		using value_type = Atomic;
		using integral_type = Atomic::value_type;
	};
}
