export module Iconer.App.Settings;
import <cstdint>;

export namespace iconer::app
{
	class Settings
	{
	public:
		static inline constexpr std::uint16_t serverPort = 40000;
		static inline constexpr size_t usersLimit = 10;
		static inline constexpr size_t roomMembersLimit = 4;
		static inline constexpr size_t nickNameLength = 16;
		static inline constexpr size_t roomTitleLength = 16;

	private:
		Settings() = delete;
		~Settings() = delete;
		Settings(const Settings&) = delete;
		Settings(Settings&&) = delete;
		Settings& operator=(const Settings&) = delete;
		Settings& operator=(Settings&&) = delete;
	};
}
