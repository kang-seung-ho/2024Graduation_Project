export module Iconer.App.ISession;
import <cstdint>;

export namespace iconer::app
{
	class [[nodiscard]] ISession
	{
	public:
		using id_type = std::int32_t;

		explicit ISession() noexcept = default;
		constexpr ~ISession() noexcept = default;

		explicit constexpr ISession(id_type id) noexcept
			: myId(id)
		{}

		[[nodiscard]]
		constexpr id_type GetID() const noexcept
		{
			return myId;
		}

		ISession(ISession&&) noexcept = default;
		ISession& operator=(ISession&&) noexcept = default;

	private:
		id_type myId;

		ISession(const ISession&) = delete;
		ISession& operator=(const ISession&) = delete;
	};
}
