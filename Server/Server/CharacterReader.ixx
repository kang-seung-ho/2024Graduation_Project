export module Iconer.Utility.StringConverter:CharacterReader;
import <cstdint>;
import <span>;

export namespace iconer::util
{
	/// <summary>
	/// Read a utf-8 character from the character buffer
	/// </summary>
	/// <param name="buffer"></param>
	/// <returns>read utf-8 character</returns>
	/// <exception cref="std::out_of_range"/>
	[[nodiscard]]
	inline constexpr char8_t ReadUtf8Char(const char* buffer)
	{
		return static_cast<char8_t>(static_cast<std::uint8_t>(buffer[0]));
	}

	/// <summary>
	/// Read a utf-8 character from a span of character
	/// </summary>
	/// <returns>read utf-8 character</returns>
	/// <exception cref="std::out_of_range"/>
	[[nodiscard]]
	inline constexpr char8_t ReadUtf8Char(std::span<const char> buffer)
	{
		return static_cast<char8_t>(static_cast<std::uint8_t>(buffer[0]));
	}

	/// <summary>
	/// Read a utf-8 character from the character buffer
	/// </summary>
	/// <param name="buffer"></param>
	/// <param name="output"></param>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	inline constexpr const char* ReadChar(const char* buffer, char8_t& output)
	{
		output = static_cast<char8_t>(static_cast<std::uint8_t>(buffer[0]));

		return buffer + sizeof(char8_t);
	}

	/// <summary>
	/// Read a utf-8 character from a span of character
	/// </summary>
	/// <param name="buffer"></param>
	/// <param name="output"></param>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	inline constexpr std::span<const char> ReadChar(std::span<const char> buffer, char8_t& output)
	{
		output = static_cast<char8_t>(static_cast<std::uint8_t>(buffer[0]));

		return buffer.subspan(sizeof(char8_t));
	}

	/// <summary>
	/// Read a wide character from the character buffer
	/// </summary>
	/// <param name="buffer"></param>
	/// <returns>read wide character</returns>
	/// <exception cref="std::out_of_range"/>
	[[nodiscard]]
	inline constexpr wchar_t ReadWideChar(const char* buffer)
	{
		const std::uint32_t mid = (static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[0]));

		return static_cast<wchar_t>(mid);
	}

	/// <summary>
	/// Read a wide character from a span of character
	/// </summary>
	/// <returns>read wide character</returns>
	/// <exception cref="std::out_of_range"/>
	[[nodiscard]]
	inline constexpr wchar_t ReadWideChar(std::span<const char> buffer)
	{
		const std::uint32_t mid = (static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[0]));

		return static_cast<wchar_t>(mid);
	}

	/// <summary>
	/// Read a wide character from the character buffer
	/// </summary>
	/// <param name="buffer"></param>
	/// <param name="output"></param>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	inline constexpr const char* ReadChar(const char* buffer, wchar_t& output)
	{
		const std::uint32_t mid = (static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[0]));

		output = static_cast<wchar_t>(mid);

		return buffer + sizeof(wchar_t);
	}

	/// <summary>
	/// Read a wide character from a span of character
	/// </summary>
	/// <param name="buffer"></param>
	/// <param name="output"></param>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	inline constexpr std::span<const char> ReadChar(std::span<const char> buffer, wchar_t& output)
	{
		const std::uint32_t mid = (static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[0]));

		output = static_cast<wchar_t>(mid);

		return buffer.subspan(sizeof(wchar_t));
	}

	/// <summary>
	/// Read a utf-16 character from the character buffer
	/// </summary>
	/// <param name="buffer"></param>
	/// <returns>read utf-16 character</returns>
	/// <exception cref="std::out_of_range"/>
	[[nodiscard]]
	inline constexpr char16_t ReadUtf16Char(const char* buffer)
	{
		const std::uint32_t mid = (static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[0]));

		return static_cast<char16_t>(mid);
	}

	/// <summary>
	/// Read a utf-16 character from a span of character
	/// </summary>
	/// <returns>read utf-16 character</returns>
	/// <exception cref="std::out_of_range"/>
	[[nodiscard]]
	inline constexpr char16_t ReadUtf16Char(std::span<const char> buffer)
	{
		const std::uint32_t mid = (static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[0]));

		return static_cast<char16_t>(mid);
	}

	/// <summary>
	/// Read a utf-16 character from the character buffer
	/// </summary>
	/// <param name="buffer"></param>
	/// <param name="output"></param>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	inline constexpr const char* ReadChar(const char* buffer, char16_t& output)
	{
		const std::uint32_t mid = (static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[0]));

		output = static_cast<char16_t>(mid);

		return buffer + sizeof(char16_t);
	}

	/// <summary>
	/// Read a utf-16 character from a span of character
	/// </summary>
	/// <param name="buffer"></param>
	/// <param name="output"></param>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	inline constexpr std::span<const char> ReadChar(std::span<const char> buffer, char16_t& output)
	{
		const std::uint32_t mid = (static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint32_t>(static_cast<std::uint8_t>(buffer[0]));

		output = static_cast<char16_t>(mid);

		return buffer.subspan(sizeof(char16_t));
	}

	/// <summary>
	/// Read a utf-32 character from the character buffer
	/// </summary>
	/// <param name="buffer"></param>
	/// <returns>read utf-32 character</returns>
	/// <exception cref="std::out_of_range"/>
	[[nodiscard]]
	inline constexpr char32_t ReadUtf32Char(const char* buffer)
	{
		const std::uint64_t mid = (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[3])) << 24U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[2])) << 16U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[0]));

		return static_cast<char32_t>(mid);
	}

	/// <summary>
	/// Read a utf-32 character from a span of character
	/// </summary>
	/// <returns>read utf-32 character</returns>
	/// <exception cref="std::out_of_range"/>
	[[nodiscard]]
	inline constexpr char32_t ReadUtf32Char(std::span<const char> buffer)
	{
		const std::uint64_t mid = (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[3])) << 24U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[2])) << 16U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[0]));

		return static_cast<char32_t>(mid);
	}

	/// <summary>
	/// Read a utf-32 character from the character buffer
	/// </summary>
	/// <param name="buffer"></param>
	/// <param name="output"></param>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	inline constexpr const char* ReadChar(const char* buffer, char32_t& output)
	{
		const std::uint64_t mid = (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[3])) << 24U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[2])) << 16U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[0]));

		output = static_cast<char32_t>(mid);

		return buffer + sizeof(char32_t);
	}

	/// <summary>
	/// Read a utf-32 character from a span of character
	/// </summary>
	/// <param name="buffer"></param>
	/// <param name="output"></param>
	/// <returns>last buffer pointer after dest</returns>
	/// <exception cref="std::out_of_range"/>
	inline constexpr std::span<const char> ReadChar(std::span<const char> buffer, char32_t& output)
	{
		const std::uint64_t mid = (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[3])) << 24U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[2])) << 16U)
			| (static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[1])) << 8U)
			| static_cast<std::uint64_t>(static_cast<std::uint8_t>(buffer[0]));

		output = static_cast<char32_t>(mid);

		return buffer.subspan(sizeof(char32_t));
	}
}
