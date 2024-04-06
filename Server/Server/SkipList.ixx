export module Iconer.Collection.SkipList;
import Iconer.Utility.Constraints;

export namespace iconer::collection
{
	class SkipList
	{
	public:
		static inline constexpr size_t maxHeight = 5;

	private:
		struct Trail;
		struct Node;

		struct Node
		{
			int myValue;
			size_t myHeight;
			Node* mySibling;
		};

		struct Trail
		{
			Node myNodes[maxHeight];
			Trail* mySibling;
		};

	public:
		explicit constexpr SkipList() noexcept = default;
		~SkipList();

		[[nodiscard]] constexpr bool operator==(const SkipList&) const noexcept = default;

		Trail* myHead;
		size_t myLength;

	private:
	};
}
