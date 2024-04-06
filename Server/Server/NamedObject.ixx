export module Iconer.Utility.NamedObject;
import <string>;
import <string_view>;

export namespace iconer::util
{
	template<typename Char, typename Traits = std::char_traits<Char>>
	class NamedObject
	{
	public:
		using NameType = std::basic_string<Char, Traits>;
		using NameViewType = std::basic_string_view<Char, Traits>;

		template<size_t Length>
		constexpr void SetName(const Char(&name)[Length])
		{
			myName.reserve(Length);
			myName = NameType{ name };
		}

		template<size_t Length>
		constexpr void SetName(Char(&& name)[Length])
		{
			myName.reserve(Length);
			myName = NameType{ std::move(name) };
		}

		constexpr void SetName(const NameType& name)
		{
			myName = name;
		}

		constexpr void SetName(NameType&& name) noexcept
		{
			myName = std::exchange(name, NameType{});
		}
		
		constexpr void SetName(NameViewType name)
		{
			myName.reserve(name.length());
			myName = name;
		}

		[[nodiscard]]
		constexpr const NameType& GetName() const& noexcept
		{
			return myName;
		}

		[[nodiscard]]
		constexpr NameType&& GetName() && noexcept
		{
			return std::move(myName);
		}

		NameType myName;
	};
}
