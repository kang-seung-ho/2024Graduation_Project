module;
#include <cstdint>
#include <cstdlib>
#include <span>
#include <string>
#include <string_view>
#include <filesystem>
export module Iconer.Utility.File;

export namespace iconer::util
{
	using FilePath = std::filesystem::path;

	namespace file
	{
		enum [[nodiscard]] ResultCode : std::int32_t
		{
			None = 0,
			Success = 0,
			EndOfFile = -1,
			Error = -2,
			ErrorOnBuffer = -3,
			ErrorOnRead = -4,
			ErrorOnWrite = -5,
			FileNotFound = -1000,
		};

		enum class [[nodiscard]] OpenModes : std::int32_t
		{
			Read = 0b0000,
			Write = 0b0001,
			Append = 0b0010,
			Binary = 0b0100,
		};
	}

	[[nodiscard]]
	constexpr file::OpenModes operator|(file::OpenModes lhs, file::OpenModes rhs) noexcept
	{
		return static_cast<file::OpenModes>(static_cast<std::int32_t>(lhs) | static_cast<std::int32_t>(rhs));
	}

	[[nodiscard]]
	constexpr file::OpenModes operator&(file::OpenModes lhs, file::OpenModes rhs) noexcept
	{
		return static_cast<file::OpenModes>(static_cast<std::int32_t>(lhs) & static_cast<std::int32_t>(rhs));
	}

	[[nodiscard]]
	constexpr file::OpenModes operator^(file::OpenModes lhs, file::OpenModes rhs) noexcept
	{
		return static_cast<file::OpenModes>(static_cast<std::int32_t>(lhs) ^ static_cast<std::int32_t>(rhs));
	}

	[[nodiscard]]
	constexpr file::OpenModes operator~(file::OpenModes rhs) noexcept
	{
		return static_cast<file::OpenModes>(~static_cast<std::int32_t>(rhs));
	}

	constexpr file::OpenModes& operator|=(file::OpenModes& lhs, const file::OpenModes& rhs) noexcept
	{
		return lhs = static_cast<file::OpenModes>(static_cast<std::int32_t>(lhs) | static_cast<std::int32_t>(rhs));
	}

	constexpr file::OpenModes& operator&=(file::OpenModes& lhs, const file::OpenModes& rhs) noexcept
	{
		return lhs = static_cast<file::OpenModes>(static_cast<std::int32_t>(lhs) & static_cast<std::int32_t>(rhs));
	}

	constexpr file::OpenModes& operator^=(file::OpenModes& lhs, const file::OpenModes& rhs) noexcept
	{
		return lhs = static_cast<file::OpenModes>(static_cast<std::int32_t>(lhs) ^ static_cast<std::int32_t>(rhs));
	}

	[[nodiscard]]
	constexpr bool HasMode(const file::OpenModes& lhs, const file::OpenModes& rhs) noexcept
	{
		return (static_cast<std::int32_t>(lhs) & static_cast<std::int32_t>(rhs)) != 0;
	}

	class [[nodiscard]] File final
	{
	public:
		explicit File() noexcept = default;
		explicit File(const FilePath& filepath, file::OpenModes mode) noexcept;
		explicit File(FilePath&& filepath, file::OpenModes mode) noexcept;
		~File() noexcept;

		bool Open(const FilePath& filename, file::OpenModes mode = file::OpenModes::Read | file::OpenModes::Binary) noexcept;
		void Close() noexcept;

		[[nodiscard]]
		std::string Contents() const noexcept;
		[[nodiscard]]
		std::string ReadLine() const noexcept;
		[[nodiscard]]
		std::string Read(char delimiter = '\0') const noexcept;
		[[nodiscard]]
		std::string Read(std::string_view delimiter_str) const noexcept;
		[[nodiscard]]
		std::size_t Read(std::string& target_buffer, const std::size_t& size) const noexcept;
		[[nodiscard]]
		std::size_t Read(std::string& target_buffer, char delimiter = '\0') const noexcept;
		[[nodiscard]]
		std::size_t Read(void* const& buffer, const std::size_t& size) const noexcept;
		std::size_t Write(std::string_view content) const noexcept;
		std::size_t Write(std::wstring_view content) const noexcept;

		void Rewind() const noexcept;

		[[nodiscard]]
		std::size_t AcquireSize() const noexcept;

		[[nodiscard]]
		constexpr file::ResultCode GetLastResult() const noexcept
		{
			return lastResult;
		}

		[[nodiscard]]
		constexpr bool IsEmpty() const noexcept
		{
			return myHandle == nullptr;
		}

		[[nodiscard]]
		constexpr bool IsOpened() const noexcept
		{
			return myHandle != nullptr;
		}

		[[nodiscard]]
		bool IsEndOfFile() const noexcept
		{
			return std::feof(myHandle) != 0;
		}

		explicit File(File&&) noexcept = default;
		File& operator=(File&&) noexcept = default;

	private:
		File(const File&) = delete;
		void operator=(const File&) = delete;

		mutable file::ResultCode lastResult;
		mutable ::errno_t lastError;

		std::FILE* myHandle;
	};
}
