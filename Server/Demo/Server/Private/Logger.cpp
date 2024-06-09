module;
#include <cstdio>
#include <cstdlib>
#include <string>
#include <string_view>
#include <format>
#include <filesystem>
#include <mutex>

module Iconer.Utility.Logger;
import Iconer.Utility.ColourfulConsole;

namespace
{
	::FILE* STDOUT = stdout;
	::FILE* STDIN = stdin;
	::FILE* STDERR = stderr;

	iconer::util::cfc::Palette logPalette{ iconer::util::cfc::colors::BrightYellow, iconer::util::cfc::colors::Black };
	iconer::util::cfc::Palette wrnPalette{ iconer::util::cfc::colors::LightRed, iconer::util::cfc::colors::Black };
	iconer::util::cfc::Palette errPalette{ iconer::util::cfc::colors::Red, iconer::util::cfc::colors::Black };

	std::mutex consoleLock{};
}

void
iconer::util::Logger::Awake(const std::filesystem::path& log_file)
noexcept
{
	GetHandle().Open(log_file, util::file::OpenModes::Write);
}

void
iconer::util::Logger::Cleanup()
noexcept
{
	GetHandle().Close();
}

bool
iconer::util::Logger::IsAvailable()
const noexcept
{
	return GetHandle().IsOpened();
}

void
iconer::util::Logger::LogInternal(std::wstring_view msg)
const noexcept
{
	const auto str = std::wstring{ msg.data() };

	iconer::util::cfc::Palette before = iconer::util::cfc::GetConsoleColour();

	std::scoped_lock lock { consoleLock };
	iconer::util::cfc::SetConsoleColour(logPalette);
	std::fwprintf(stdout, str.c_str());
	iconer::util::cfc::SetConsoleColour(before);

	GetHandle().Write(str);
}

void
iconer::util::Logger::LogErrorInternal(std::wstring_view msg)
const noexcept
{
	const auto str = msg.data();

	iconer::util::cfc::Palette before = iconer::util::cfc::GetConsoleColour();

	std::scoped_lock lock{ consoleLock };
	iconer::util::cfc::SetConsoleColour(errPalette);
	std::fwprintf(stderr, str);
	iconer::util::cfc::SetConsoleColour(before);

	GetHandle().Write(str);
}

void
iconer::util::Logger::LogWarningInternal(std::wstring_view msg)
const noexcept
{
	const auto str = msg.data();

	iconer::util::cfc::Palette before = iconer::util::cfc::GetConsoleColour();

	std::scoped_lock lock{ consoleLock };
	iconer::util::cfc::SetConsoleColour(wrnPalette);
	std::fwprintf(stderr, str);
	iconer::util::cfc::SetConsoleColour(before);

	GetHandle().Write(str);
}
