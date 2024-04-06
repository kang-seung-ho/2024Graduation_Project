export module Iconer.Utility.Logger;
import Iconer.Utility.Handler;
import Iconer.Utility.File;
export import <string>;
export import <string_view>;
import <format>;
import <filesystem>;

export namespace iconer::util
{
	class [[nodiscard]] Logger final : Handler<iconer::util::File>
	{
	public:
		using Super = Handler<iconer::util::File>;

		Logger() noexcept = default;

		void Awake(const std::filesystem::path& log_file) noexcept;
		void Cleanup() noexcept;

		template<int = 0, typename... Args>
		void Log(std::wformat_string<Args...> msg, Args&&... args) noexcept;
		template<int = 0, typename... Args>
		void DebugLog(std::wformat_string<Args...> msg, Args&&... args) noexcept;
		template<int = 0, typename... Args>
		void LogError(std::wformat_string<Args...> msg, Args&&... args) noexcept;
		template<int = 0, typename... Args>
		void DebugLogError(std::wformat_string<Args...> msg, Args&&... args) noexcept;
		template<int = 0, typename... Args>
		void LogWarning(std::wformat_string<Args...> msg, Args&&... args) noexcept;
		template<int = 0, typename... Args>
		void DebugLogWarning(std::wformat_string<Args...> msg, Args&&... args) noexcept;

		[[nodiscard]]
		bool IsAvailable() const noexcept;

	private:
		void LogInternal(std::wstring_view msg) const noexcept;
		void LogErrorInternal(std::wstring_view msg) const noexcept;
		void LogWarningInternal(std::wstring_view msg) const noexcept;

		Logger(const Logger&) = delete;
		Logger(Logger&&) = delete;
		void operator=(const Logger&) = delete;
		void operator=(Logger&&) = delete;
	};
}

namespace
{
	[[nodiscard]]
	consteval bool IsInDebugging() noexcept
	{
#if _DEBUG
		return true;
#else
		return false;
#endif
	}
}

namespace iconer::util
{
	template<int, typename ...Args>
	void Logger::Log(std::wformat_string<Args...> msg, Args && ...args) noexcept
	{
		if constexpr (0 < sizeof...(Args))
		{
			const std::wstring str = std::format(std::move(msg), std::forward<Args>(args)...);
			LogInternal(str);
		}
		else
		{
			const std::wstring_view str = msg.get(); // intended temporary variable
			LogInternal(str);
		}
	}

	template<int, typename ...Args>
	void Logger::DebugLog(std::wformat_string<Args...> msg, Args && ...args) noexcept
	{
		if constexpr (IsInDebugging())
		{
			Log(std::move(msg), std::forward<Args>(args)...);
		}
	}

	template<int, typename ...Args>
	void Logger::LogError(std::wformat_string<Args...> msg, Args && ...args) noexcept
	{
		if constexpr (0 < sizeof...(Args))
		{
			const std::wstring str = std::format(std::move(msg), std::forward<Args>(args)...);
			LogErrorInternal(str);
		}
		else
		{
			const std::wstring_view str = msg.get(); // intended temporary variable
			LogErrorInternal(str);
		}
	}

	template<int, typename ...Args>
	void Logger::DebugLogError(std::wformat_string<Args...> msg, Args && ...args) noexcept
	{
		if constexpr (IsInDebugging())
		{
			LogError(std::move(msg), std::forward<Args>(args)...);
		}
	}

	template<int, typename ...Args>
	void Logger::LogWarning(std::wformat_string<Args...> msg, Args && ...args) noexcept
	{
		if constexpr (0 < sizeof...(Args))
		{
			const std::wstring str = std::format(std::move(msg), std::forward<Args>(args)...);
			LogWarningInternal(str);
		}
		else
		{
			const std::wstring_view str = msg.get(); // intended temporary variable
			LogWarningInternal(str);
		}
	}

	template<int, typename ...Args>
	void Logger::DebugLogWarning(std::wformat_string<Args...> msg, Args && ...args) noexcept
	{
		if constexpr (IsInDebugging())
		{
			LogWarning(std::move(msg), std::forward<Args>(args)...);
		}
	}
}
