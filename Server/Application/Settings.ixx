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

	private:
		Settings() = delete;
		~Settings() = delete;
		Settings(const Settings&) = delete;
		Settings(Settings&&) = delete;
		Settings& operator=(const Settings&) = delete;
		Settings& operator=(Settings&&) = delete;
	};
}
