module;
#include <cstdlib>
#include <cstdio>
#include <string_view>
#include <string>
module Iconer.Utility.File;

namespace
{
	inline constexpr std::wstring_view ModeStrings[] =
	{
		L"r+" // 0
		, L"w+" // 1
		, L"a+" // 2
		, L"a+" // 3
		, L"rb+" // 4
		, L"wb+" // 5
		, L"ab+" // 6
	};
}

iconer::util::File::File(const FilePath& filepath, file::OpenModes mode)
noexcept
	: myHandle(nullptr)
{
	Open(filepath, mode);
}

iconer::util::File::File(FilePath&& filepath, file::OpenModes mode)
noexcept
	: myHandle(nullptr)
{
	Open(std::move(filepath), mode);
}

iconer::util::File::~File()
noexcept
{
	Close();
}

bool
iconer::util::File::Open(const FilePath& filename, file::OpenModes mode)
noexcept
{
	::errno_t err = ::_wfopen_s(std::addressof(myHandle), filename.c_str(), ModeStrings[static_cast<std::size_t>(mode)].data());
	lastError = err;

	if (0 != err)
	{
		lastResult = file::ResultCode::FileNotFound;
		return false;
	}

	lastResult = file::ResultCode::Success;
	return true;
}

void
iconer::util::File::Close()
noexcept
{
	if (myHandle)
	{
		std::fclose(std::exchange(myHandle, nullptr));
	}
}

std::string
iconer::util::File::Contents()
const noexcept
{
	if (nullptr == myHandle)
	{
		lastError = 0;
		lastResult = file::ResultCode::FileNotFound;
		return {};
	}

	std::string buffer;
	std::size_t size;
	try
	{
		size = AcquireSize();
		buffer.resize(size);
	}
	catch (...)
	{
		lastError = errno;
		lastResult = file::ResultCode::ErrorOnBuffer;
		return {};
	}

	try
	{
		std::fread(buffer.data(), 1, size, myHandle);
	}
	catch (...)
	{
		lastError = errno;
		lastResult = file::ResultCode::ErrorOnRead;
		return {};
	}

	return buffer;
}

std::string
iconer::util::File::ReadLine()
const noexcept
{
	std::string result = Read('\n');

	try
	{
		result.shrink_to_fit();
	}
	catch (...)
	{
		lastError = errno;
		lastResult = file::ResultCode::Error;
		return {};
	}

	return result;
}

std::string
iconer::util::File::Read(char delimiter)
const noexcept
{
	return Read(std::to_string(delimiter));
}

std::string
iconer::util::File::Read(std::string_view delimiter_str)
const noexcept
{
	if (nullptr == myHandle)
	{
		lastError = 0;
		lastResult = file::ResultCode::FileNotFound;
		return {};
	}

	std::string buffer;
	try
	{
		buffer.reserve(128);
	}
	catch (...)
	{
		lastError = errno;
		lastResult = file::ResultCode::ErrorOnBuffer;
		return {};
	}

	try
	{
		while (true)
		{
			const int result = std::fgetc(myHandle);
			if (result == file::ResultCode::EndOfFile)
			{
				break;
			}

			buffer.push_back(static_cast<char>(result));
			if (buffer.ends_with(delimiter_str))
			{
				break;
			}
		}
	}
	catch (...)
	{
		lastError = errno;
		lastResult = file::ResultCode::ErrorOnRead;
		return {};
	}

	return buffer;
}

std::size_t
iconer::util::File::Read(std::string& target_buffer, const std::size_t& size)
const noexcept
{
	if (nullptr == myHandle)
	{
		lastError = 0;
		lastResult = file::ResultCode::FileNotFound;
		return 0;
	}

	try
	{
		target_buffer.resize(size);
	}
	catch (...)
	{
		lastError = errno;
		lastResult = file::ResultCode::ErrorOnBuffer;
		return 0;
	}

	return Read(target_buffer.data(), size);
}

std::size_t
iconer::util::File::Read(std::string& target_buffer, char delimiter)
const noexcept
{
	target_buffer = Read(delimiter);

	return target_buffer.size();
}

std::size_t
iconer::util::File::Read(void* const& buffer, const std::size_t& size)
const noexcept
{
	if (nullptr == myHandle)
	{
		lastError = 0;
		lastResult = file::ResultCode::FileNotFound;
		return 0;
	}

	size_t read_bytes;
	try
	{
		read_bytes = std::fread(buffer, 1, size, myHandle);
	}
	catch (...)
	{
		lastError = errno;
		lastResult = file::ResultCode::ErrorOnRead;
		return 0;
	}

	return read_bytes;
}

std::size_t
iconer::util::File::Write(std::string_view content)
const noexcept
{
	if (nullptr == myHandle)
	{
		lastError = 0;
		lastResult = file::ResultCode::FileNotFound;
		return 0;
	}

	const int length = std::fprintf(myHandle, "%s", content.data());
	if (length == file::ResultCode::EndOfFile)
	{
		lastError = errno;
		lastResult = file::ResultCode::ErrorOnWrite;
		return 0;
	}

	return static_cast<std::size_t>(length);
}

std::size_t
iconer::util::File::Write(std::wstring_view content)
const noexcept
{
	if (nullptr == myHandle)
	{
		lastError = 0;
		lastResult = file::ResultCode::FileNotFound;
		return 0;
	}

	const int length = std::fwprintf(myHandle, L"%s", content.data());
	if (length == file::ResultCode::EndOfFile)
	{
		lastError = errno;
		lastResult = file::ResultCode::ErrorOnWrite;
		return 0;
	}

	return static_cast<std::size_t>(length);
}

void
iconer::util::File::Rewind()
const noexcept
{
	std::rewind(myHandle);
}

std::size_t
iconer::util::File::AcquireSize()
const noexcept
{
	if (nullptr == myHandle)
	{
		return std::size_t{};
	}

	const std::int32_t current = std::ftell(myHandle);
	std::fseek(myHandle, 0, SEEK_END);
	const std::int32_t size = std::ftell(myHandle);
	std::fseek(myHandle, current, SEEK_SET);

	return static_cast<size_t>(size);
}
