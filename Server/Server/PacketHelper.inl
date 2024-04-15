#pragma region Foundations
#define MAKE_WRITE_METHODS(...) \
[[nodiscard]] constexpr auto Serialize() const \
{ \
	return iconer::util::Serializes(myProtocol, mySize, __VA_ARGS__); \
} \
constexpr std::byte* Write(std::byte* buffer) const \
{ \
	return iconer::util::Serializes(Super::Write(buffer), __VA_ARGS__); \
}

#define MAKE_READ_METHOD_V1(v1) \
constexpr const std::byte* Read(const std::byte* buffer) \
{ \
	return iconer::util::Deserialize(Super::Read(buffer), v1); \
}

#define MAKE_READ_METHOD_V2(v1, v2) \
constexpr const std::byte* Read(const std::byte* buffer) \
{ \
	return iconer::util::Deserialize(iconer::util::Deserialize(Super::Read(buffer), v1), v2); \
}

#define MAKE_READ_METHOD_V3(v1, v2, v3) \
constexpr const std::byte* Read(const std::byte* buffer) \
{ \
	return iconer::util::Deserialize(iconer::util::Deserialize(iconer::util::Deserialize(Super::Read(buffer), v1), v2), v3); \
}

#define MAKE_READ_METHOD_V4(v1, v2, v3, v4) \
constexpr const std::byte* Read(const std::byte* buffer) \
{ \
	return iconer::util::Deserialize(iconer::util::Deserialize(iconer::util::Deserialize(iconer::util::Deserialize(Super::Read(buffer), v1), v2), v3), v4); \
}

#define MAKE_READ_METHOD_V5(v1, v2, v3, v4, v5) \
constexpr const std::byte* Read(const std::byte* buffer) \
{ \
	return iconer::util::Deserialize(iconer::util::Deserialize(iconer::util::Deserialize(iconer::util::Deserialize(iconer::util::Deserialize(Super::Read(buffer), v1), v2), v3), v4), v5); \
}
#pragma endregion

#pragma region No field
#define MAKE_EMPTY_PACKET(name, protocol) \
struct [[nodiscard]] name : public BasicPacket \
{ \
	using Super = BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept { return Super::MinSize(); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept { return Super::SignedMinSize(); } \
 \
	constexpr name() noexcept \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
	{ \
	} \
 \
	[[nodiscard]] \
	constexpr auto Serialize() const { return iconer::util::Serializes(myProtocol, mySize); } \
}
#pragma endregion

#pragma region 1 Field
#pragma region 1 Field with initializers
#define MAKE_PACKET_1VAR_WITH_DEFAULT(name, protocol, var1_type, var1_name, var1_decl, param1_name, var1_default_value, ...) \
struct name : public iconer::app::BasicPacket \
{ \
	using Super = iconer::app::BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	template<std::enable_if_t<std::is_constructible_v<var1_type, decltype(var1_default_value)>, int> = 0> \
	constexpr name() \
		noexcept(std::is_nothrow_constructible_v<var1_type, decltype(var1_default_value)>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((var1_default_value)) \
	{} \
 \
	template<std::enable_if_t<std::is_copy_constructible_v<var1_type>, int> = 0>\
	constexpr name(const var1_type& param1_name) \
		noexcept(std::is_nothrow_copy_constructible_v<var1_type>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
	{} \
 \
	template<std::enable_if_t<std::is_move_constructible_v<var1_type>, int> = 0>\
	constexpr name(var1_type&& param1_name) \
		noexcept(std::is_nothrow_move_constructible_v<var1_type>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
	{} \
 \
	MAKE_READ_METHOD_V1(var1_name); \
	MAKE_WRITE_METHODS(var1_name); \
	__VA_ARGS__; \
 \
	var1_type var1_decl; \
}

#define MAKE_EMPTY_PACKET_1VAR_WITH_DEFAULT(name, protocol, var1_type, var1_name, var1_decl, var1_default_value, ...) \
struct name : public iconer::app::BasicPacket \
{ \
	using Super = iconer::app::BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	constexpr name() \
		noexcept(std::is_nothrow_constructible_v<var1_type, decltype(var1_default_value)>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(var1_default_value) \
	{} \
 \
	MAKE_READ_METHOD_V1(var1_name); \
	__VA_ARGS__; \
 \
	var1_type var1_decl; \
}
#pragma endregion

#pragma region 1 Fields with default initializations
#define MAKE_PACKET_1VAR(name, protocol, var1_type, var1_name, var1_decl, param1_name, ...) \
struct name : public iconer::app::BasicPacket \
{ \
	using Super = iconer::app::BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	template<std::enable_if_t<std::is_default_constructible_v<var1_type>, int> = 0> \
	constexpr name() \
		noexcept(std::is_nothrow_default_constructible_v<var1_type>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name() \
	{} \
 \
	template<std::enable_if_t<std::is_copy_constructible_v<var1_type>, int> = 0>\
	constexpr name(const var1_type& param1_name) \
		noexcept(std::is_nothrow_copy_constructible_v<var1_type>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
	{} \
 \
	template<std::enable_if_t<std::is_move_constructible_v<var1_type>, int> = 0>\
	constexpr name(var1_type&& param1_name) \
		noexcept(std::is_nothrow_move_constructible_v<var1_type>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
	{} \
 \
	MAKE_READ_METHOD_V1(var1_name); \
	MAKE_WRITE_METHODS(var1_name); \
	__VA_ARGS__; \
 \
	var1_type var1_decl; \
}

#define MAKE_EMPTY_PACKET_1VAR(name, protocol, var1_type, var1_name, var1_decl, ...) \
struct name : public iconer::app::BasicPacket \
{ \
	using Super = iconer::app::BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	template<std::enable_if_t<std::is_default_constructible_v<var1_type>, int> = 0> \
	constexpr name() \
		noexcept(std::is_nothrow_default_constructible_v<var1_type>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name() \
	{} \
 \
	MAKE_READ_METHOD_V1(var1_name); \
	__VA_ARGS__; \
 \
	var1_type var1_decl; \
}
#pragma endregion
#pragma endregion

#pragma region 2 Fields
#pragma region 2 Field with initializers
#define MAKE_PACKET_2VAR_WITH_DEFAULT(name, protocol, var1_type, var1_name, param1_name, var1_default_value, var2_type, var2_name, param2_name, var2_default_value, ...) \
struct name : public iconer::app::BasicPacket \
{ \
	using Super = iconer::app::BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type) + sizeof(var2_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_constructible<var1_type, decltype(var1_default_value)>, std::is_constructible<var2_type, decltype(var2_default_value)>>, int> = 0>\
	constexpr name() \
		noexcept(std::conjunction_v<std::is_nothrow_constructible<var1_type, decltype(var1_default_value)>, std::is_nothrow_constructible<var2_type, decltype(var2_default_value)>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((var1_default_value)) \
		, var2_name((var2_default_value)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name((param2_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, const var2_type& param2_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name((param2_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name(std::move(param2_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
	{} \
 \
	MAKE_READ_METHOD_V2(var1_name, var2_name); \
	MAKE_WRITE_METHODS(var1_name, var2_name); \
	__VA_ARGS__; \
 \
	var1_type var1_name; \
	var2_type var2_name; \
}

#define MAKE_EMPTY_PACKET_2VAR_WITH_DEFAULT(name, protocol, var1_type, var1_name, var1_default_value, var2_type, var2_name, var2_default_value, ...) \
struct name : public iconer::app::BasicPacket \
{ \
	using Super = iconer::app::BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type) + sizeof(var2_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_constructible<var1_type, decltype(var1_default_value)>, std::is_constructible<var2_type, decltype(var2_default_value)>>, int> = 0>\
	constexpr name() \
		noexcept(std::conjunction_v<std::is_nothrow_constructible<var1_type, decltype(var1_default_value)>, std::is_nothrow_constructible<var2_type, decltype(var2_default_value)>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((var1_default_value)) \
		, var2_name((var2_default_value)) \
	{} \
 \
	MAKE_READ_METHOD_V2(var1_name, var2_name); \
	__VA_ARGS__; \
 \
	var1_type var1_name; \
	var2_type var2_name; \
}
#pragma endregion

#pragma region 2 Fields with default initializations
#define MAKE_PACKET_2VAR(name, protocol, var1_type, var1_name, param1_name, var2_type, var2_name, param2_name, ...) \
struct name : public iconer::app::BasicPacket \
{ \
	using Super = iconer::app::BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type) + sizeof(var2_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_default_constructible<var1_type>, std::is_default_constructible<var2_type>>, int> = 0>\
	constexpr name() \
		noexcept(std::conjunction_v<std::is_nothrow_default_constructible<var1_type>, std::is_nothrow_default_constructible<var2_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name() \
		, var2_name() \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name((param2_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_copy_constructible<var2_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, const var2_type& param2_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name((param2_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name(std::move(param2_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
	{} \
 \
	MAKE_READ_METHOD_V2(var1_name, var2_name); \
	MAKE_WRITE_METHODS(var1_name, var2_name); \
	__VA_ARGS__; \
 \
	var1_type var1_name; \
	var2_type var2_name; \
}

#define MAKE_EMPTY_PACKET_2VAR(name, protocol, var1_type, var1_name, var2_type, var2_name, ...) \
struct name : public iconer::app::BasicPacket \
{ \
	using Super = iconer::app::BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type) + sizeof(var2_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_default_constructible<var1_type>, std::is_default_constructible<var2_type>>, int> = 0>\
	constexpr name() \
		noexcept(std::conjunction_v<std::is_nothrow_default_constructible<var1_type>, std::is_nothrow_default_constructible<var2_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name() \
		, var2_name() \
	{} \
 \
	MAKE_READ_METHOD_V2(var1_name, var2_name); \
	__VA_ARGS__; \
 \
	var1_type var1_name; \
	var2_type var2_name; \
}
#pragma endregion
#pragma endregion

#pragma region 3 Fields
#pragma region 3 Fields with initializers
#define MAKE_PACKET_3VAR_WITH_DEFAULT(name, protocol, var1_type, var1_name, param1_name, var1_default_value, var2_type, var2_name, param2_name, var2_default_value, var3_type, var3_name, param3_name, var3_default_value, ...) \
struct name : public iconer::app::BasicPacket \
{ \
	using Super = iconer::app::BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type) + sizeof(var2_type) + sizeof(var3_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_constructible<var1_type, decltype(var1_default_value)>, std::is_constructible<var2_type, decltype(var2_default_value)>, std::is_constructible<var3_type, decltype(var3_default_value)>>, int> = 0>\
	constexpr name() \
		noexcept(std::conjunction_v<std::is_nothrow_constructible<var1_type, decltype(var1_default_value)>, std::is_nothrow_constructible<var2_type, decltype(var2_default_value)>, std::is_nothrow_constructible<var3_type, decltype(var3_default_value)>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((var1_default_value)) \
		, var2_name((var2_default_value)) \
		, var3_name((var3_default_value)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_copy_constructible<var3_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name, const var3_type& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name((param2_name)) \
		, var3_name((param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_copy_constructible<var3_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, const var2_type& param2_name, const var3_type& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name((param2_name)) \
		, var3_name((param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_copy_constructible<var3_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name, const var3_type& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name((param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_move_constructible<var3_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name, var3_type&& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name((param2_name)) \
		, var3_name(std::move(param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_copy_constructible<var3_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name, const var3_type& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name((param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_move_constructible<var3_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name, var3_type&& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name(std::move(param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_move_constructible<var3_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, const var2_type& param2_name, var3_type&& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name((param2_name)) \
		, var3_name(std::move(param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_move_constructible<var3_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name, var3_type&& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name(std::move(param3_name)) \
	{} \
 \
	MAKE_READ_METHOD_V3(var1_name, var2_name, var3_name); \
	MAKE_WRITE_METHODS(var1_name, var2_name, var3_name); \
	__VA_ARGS__; \
 \
	var1_type var1_name; \
	var2_type var2_name; \
	var3_type var3_name; \
}

#define MAKE_EMPTY_PACKET_3VAR_WITH_DEFAULT(name, protocol, var1_type, var1_name, var1_default_value, var2_type, var2_name, var2_default_value, var3_type, var3_name, var3_default_value, ...) \
struct name : public iconer::app::BasicPacket \
{ \
	using Super = iconer::app::BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type) + sizeof(var2_type) + sizeof(var3_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_constructible<var1_type, decltype(var1_default_value)>, std::is_constructible<var2_type, decltype(var2_default_value)>, std::is_constructible<var3_type, decltype(var3_default_value)>>, int> = 0>\
	constexpr name() \
		noexcept(std::conjunction_v<std::is_nothrow_constructible<var1_type, decltype(var1_default_value)>, std::is_nothrow_constructible<var2_type, decltype(var2_default_value)>, std::is_nothrow_constructible<var3_type, decltype(var3_default_value)>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((var1_default_value)) \
		, var2_name((var2_default_value)) \
		, var3_name((var3_default_value)) \
	{} \
 \
	MAKE_READ_METHOD_V2(var1_name, var2_name, var3_name); \
	__VA_ARGS__; \
 \
	var1_type var1_name; \
	var2_type var2_name; \
	var3_type var3_name; \
}
#pragma endregion

#pragma region 3 Fields with default initializations
#define MAKE_PACKET_3VAR(name, protocol, var1_type, var1_name, param1_name, var2_type, var2_name, param2_name, var3_type, var3_name, param3_name, ...) \
struct name : public iconer::app::BasicPacket \
{ \
	using Super = iconer::app::BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type) + sizeof(var2_type) + sizeof(var3_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_default_constructible<var1_type>, std::is_default_constructible<var2_type>, std::is_default_constructible<var3_type>>, int> = 0>\
	constexpr name() \
		noexcept(std::conjunction_v<std::is_nothrow_default_constructible<var1_type>, std::is_nothrow_default_constructible<var2_type>, std::is_nothrow_default_constructible<var3_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name() \
		, var2_name() \
		, var3_name() \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_copy_constructible<var3_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name, const var3_type& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name((param2_name)) \
		, var3_name((param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_copy_constructible<var3_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, const var2_type& param2_name, const var3_type& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name((param2_name)) \
		, var3_name((param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_copy_constructible<var3_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name, const var3_type& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name((param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_move_constructible<var3_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name, var3_type&& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name((param2_name)) \
		, var3_name(std::move(param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_copy_constructible<var3_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name, const var3_type& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name((param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_move_constructible<var3_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name, var3_type&& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name(std::move(param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_move_constructible<var3_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, const var2_type& param2_name, var3_type&& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name((param2_name)) \
		, var3_name(std::move(param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_move_constructible<var3_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name, var3_type&& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name(std::move(param3_name)) \
	{} \
 \
	MAKE_READ_METHOD_V3(var1_name, var2_name, var3_name); \
	MAKE_WRITE_METHODS(var1_name, var2_name, var3_name); \
	__VA_ARGS__; \
 \
	var1_type var1_name; \
	var2_type var2_name; \
	var3_type var3_name; \
}

#define MAKE_EMPTY_PACKET_3VAR(name, protocol, var1_type, var1_name, var2_type, var2_name, var3_type, var3_name, ...) \
struct name : public iconer::app::BasicPacket \
{ \
	using Super = iconer::app::BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type) + sizeof(var2_type) + sizeof(var3_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	constexpr name() \
		noexcept(std::conjunction_v<std::is_nothrow_default_constructible<var1_type>, std::is_nothrow_default_constructible<var2_type>, std::is_nothrow_default_constructible<var3_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name() \
		, var2_name() \
		, var3_name() \
	{} \
 \
	MAKE_READ_METHOD_V3(var1_name, var2_name, var3_name); \
	__VA_ARGS__; \
 \
	var1_type var1_name; \
	var2_type var2_name; \
	var3_type var3_name; \
}
#pragma endregion
#pragma endregion

#pragma region 4 Fields
#pragma region 4 Field with initializers
#define MAKE_PACKET_4VAR_WITH_DEFAULT(name, protocol, var1_type, var1_name, param1_name, var1_default_value, var2_type, var2_name, param2_name, var2_default_value, var3_type, var3_name, param3_name, var3_default_value, var4_type, var4_name, param4_name, var4_default_value, ...) \
struct name : public iconer::app::BasicPacket \
{ \
	using Super = iconer::app::BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type) + sizeof(var2_type) + sizeof(var3_type) + sizeof(var4_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_constructible<var1_type, decltype(var1_default_value)>, std::is_constructible<var2_type, decltype(var2_default_value)>, std::is_constructible<var3_type, decltype(var3_default_value)>, std::is_constructible<var4_type, decltype(var4_default_value)>>, int> = 0>\
	constexpr name() \
		noexcept(std::conjunction_v<std::is_nothrow_constructible<var1_type, decltype(var1_default_value)>, std::is_nothrow_constructible<var2_type, decltype(var2_default_value)>, std::is_nothrow_constructible<var3_type, decltype(var3_default_value)>, std::is_nothrow_constructible<var4_type, decltype(var4_default_value)>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(var1_default_value) \
		, var2_name(var2_default_value) \
		, var3_name(var3_default_value) \
		, var4_name(var4_default_value) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_copy_constructible<var3_type>, std::is_copy_constructible<var4_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name, const var3_type& param3_name, const var4_type& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>, std::is_nothrow_copy_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(param1_name) \
		, var2_name(param2_name) \
		, var3_name(param3_name) \
		, var4_name(param4_name) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_copy_constructible<var3_type>, std::is_copy_constructible<var4_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, const var2_type& param2_name, const var3_type& param3_name, const var4_type& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>, std::is_nothrow_copy_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(param2_name) \
		, var3_name(param3_name) \
		, var4_name(param4_name) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_copy_constructible<var3_type>, std::is_copy_constructible<var4_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name, const var3_type& param3_name, const var4_type& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>, std::is_nothrow_copy_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(param1_name) \
		, var2_name(std::move(param2_name)) \
		, var3_name(param3_name) \
		, var4_name(param4_name) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_move_constructible<var3_type>, std::is_copy_constructible<var4_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name, var3_type&& param3_name, const var4_type& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>, std::is_nothrow_copy_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(param1_name) \
		, var2_name(param2_name) \
		, var3_name(std::move(param3_name)) \
		, var4_name(param4_name) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_copy_constructible<var3_type>, std::is_move_constructible<var4_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name, const var3_type& param3_name, var4_type&& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>, std::is_nothrow_move_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(param1_name) \
		, var2_name(param2_name) \
		, var3_name(param3_name) \
		, var4_name(std::move(param4_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_copy_constructible<var3_type>, std::is_copy_constructible<var4_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name, const var3_type& param3_name, const var4_type& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>, std::is_nothrow_copy_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name(param3_name) \
		, var4_name(param4_name) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_move_constructible<var3_type>, std::is_copy_constructible<var4_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name, var3_type&& param3_name, const var4_type& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>, std::is_nothrow_copy_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(param1_name) \
		, var2_name(std::move(param2_name)) \
		, var3_name(std::move(param3_name)) \
		, var4_name(param4_name) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_move_constructible<var3_type>, std::is_move_constructible<var4_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name, var3_type&& param3_name, var4_type&& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>, std::is_nothrow_move_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(param1_name) \
		, var2_name(param2_name) \
		, var3_name(std::move(param3_name)) \
		, var4_name(std::move(param4_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_move_constructible<var3_type>, std::is_copy_constructible<var4_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, const var2_type& param2_name, var3_type&& param3_name, const var4_type& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>, std::is_nothrow_copy_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(param2_name) \
		, var3_name(std::move(param3_name)) \
		, var4_name(param4_name) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_copy_constructible<var3_type>, std::is_move_constructible<var4_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name, var3_type&& param3_name, var4_type&& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>, std::is_nothrow_move_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(param1_name) \
		, var2_name(std::move(param2_name)) \
		, var3_name(param3_name) \
		, var4_name(std::move(param4_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_move_constructible<var3_type>, std::is_copy_constructible<var4_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name, var3_type&& param3_name, const var4_type& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>, std::is_nothrow_copy_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name(std::move(param3_name)) \
		, var4_name(param4_name) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_move_constructible<var3_type>, std::is_move_constructible<var4_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name, var3_type&& param3_name, var4_type&& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>, std::is_nothrow_move_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(param1_name) \
		, var2_name(std::move(param2_name)) \
		, var3_name(std::move(param3_name)) \
		, var4_name(std::move(param4_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_move_constructible<var3_type>, std::is_move_constructible<var4_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, const var2_type& param2_name, var3_type&& param3_name, var4_type&& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>, std::is_nothrow_move_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(param2_name) \
		, var3_name(std::move(param3_name)) \
		, var4_name(std::move(param4_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_copy_constructible<var3_type>, std::is_move_constructible<var4_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name, const var3_type& param3_name, var4_type&& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>, std::is_nothrow_move_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name(param3_name) \
		, var4_name(std::move(param4_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_move_constructible<var3_type>, std::is_move_constructible<var4_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name, var3_type&& param3_name, var4_type&& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>, std::is_nothrow_move_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name(std::move(param3_name)) \
		, var4_name(std::move(param4_name)) \
	{} \
 \
	MAKE_READ_METHOD_V4(var1_name, var2_name, var3_name, var4_name); \
	MAKE_WRITE_METHODS(var1_name, var2_name, var3_name, var4_name); \
	__VA_ARGS__; \
 \
	var1_type var1_name; \
	var2_type var2_name; \
	var3_type var3_name; \
	var4_type var4_name; \
}

#define MAKE_EMPTY_PACKET_4VAR_WITH_DEFAULT(name, protocol, var1_type, var1_name, var1_default_value, var2_type, var2_name, var2_default_value, var3_type, var3_name, var3_default_value, var4_type, var4_name, var4_default_value, ...) \
struct name : public iconer::app::BasicPacket \
{ \
	using Super = iconer::app::BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type) + sizeof(var2_type) + sizeof(var3_type) + sizeof(var4_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_constructible<var1_type, decltype(var1_default_value)>, std::is_constructible<var2_type, decltype(var2_default_value)>, std::is_constructible<var3_type, decltype(var3_default_value)>, std::is_constructible<var4_type, decltype(var4_default_value)>>, int> = 0>\
	constexpr name() \
		noexcept(std::conjunction_v<std::is_nothrow_constructible<var1_type, decltype(var1_default_value)>, std::is_nothrow_constructible<var2_type, decltype(var2_default_value)>, std::is_nothrow_constructible<var3_type, decltype(var3_default_value)>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((var1_default_value)) \
		, var2_name((var2_default_value)) \
		, var3_name((var3_default_value)) \
		, var4_name((var4_default_value)) \
	{} \
 \
	MAKE_READ_METHOD_V4(var1_name, var2_name, var3_name, var4_type); \
	__VA_ARGS__; \
 \
	var1_type var1_name; \
	var2_type var2_name; \
	var3_type var3_name; \
	var4_type var4_name; \
}
#pragma endregion

#pragma region 4 Fields with default initializations
#define MAKE_PACKET_4VAR(name, protocol, var1_type, var1_name, param1_name, var2_type, var2_name, param2_name, var3_type, var3_name, param3_name, var4_type, var4_name, param4_name, ...) \
struct name : public iconer::app::BasicPacket \
{ \
	using Super = iconer::app::BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type) + sizeof(var2_type) + sizeof(var3_type) + sizeof(var4_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_default_constructible<var1_type>, std::is_default_constructible<var2_type>, std::is_default_constructible<var3_type>, std::is_default_constructible<var4_type>>, int> = 0>\
	constexpr name() \
		noexcept(std::conjunction_v<std::is_nothrow_default_constructible<var1_type>, std::is_nothrow_default_constructible<var2_type>, std::is_nothrow_default_constructible<var3_type>, std::is_nothrow_default_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name() \
		, var2_name() \
		, var3_name() \
		, var4_name() \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_copy_constructible<var3_type>, std::is_copy_constructible<var4_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name, const var3_type& param3_name, const var4_type& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>, std::is_nothrow_copy_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(param1_name) \
		, var2_name(param2_name) \
		, var3_name(param3_name) \
		, var4_name(param4_name) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_copy_constructible<var3_type>, std::is_copy_constructible<var4_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, const var2_type& param2_name, const var3_type& param3_name, const var4_type& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>, std::is_nothrow_copy_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(param2_name) \
		, var3_name(param3_name) \
		, var4_name(param4_name) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_copy_constructible<var3_type>, std::is_copy_constructible<var4_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name, const var3_type& param3_name, const var4_type& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>, std::is_nothrow_copy_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(param1_name) \
		, var2_name(std::move(param2_name)) \
		, var3_name(param3_name) \
		, var4_name(param4_name) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_move_constructible<var3_type>, std::is_copy_constructible<var4_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name, var3_type&& param3_name, const var4_type& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>, std::is_nothrow_copy_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(param1_name) \
		, var2_name(param2_name) \
		, var3_name(std::move(param3_name)) \
		, var4_name(param4_name) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_copy_constructible<var3_type>, std::is_move_constructible<var4_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name, const var3_type& param3_name, var4_type&& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>, std::is_nothrow_move_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(param1_name) \
		, var2_name(param2_name) \
		, var3_name(param3_name) \
		, var4_name(std::move(param4_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_copy_constructible<var3_type>, std::is_copy_constructible<var4_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name, const var3_type& param3_name, const var4_type& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>, std::is_nothrow_copy_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name(param3_name) \
		, var4_name(param4_name) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_move_constructible<var3_type>, std::is_copy_constructible<var4_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name, var3_type&& param3_name, const var4_type& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>, std::is_nothrow_copy_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(param1_name) \
		, var2_name(std::move(param2_name)) \
		, var3_name(std::move(param3_name)) \
		, var4_name(param4_name) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_move_constructible<var3_type>, std::is_move_constructible<var4_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name, var3_type&& param3_name, var4_type&& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>, std::is_nothrow_move_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(param1_name) \
		, var2_name(param2_name) \
		, var3_name(std::move(param3_name)) \
		, var4_name(std::move(param4_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_move_constructible<var3_type>, std::is_copy_constructible<var4_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, const var2_type& param2_name, var3_type&& param3_name, const var4_type& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>, std::is_nothrow_copy_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(param2_name) \
		, var3_name(std::move(param3_name)) \
		, var4_name(param4_name) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_copy_constructible<var3_type>, std::is_move_constructible<var4_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name, const var3_type& param3_name, var4_type&& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>, std::is_nothrow_move_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(param1_name) \
		, var2_name(std::move(param2_name)) \
		, var3_name(param3_name) \
		, var4_name(std::move(param4_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_move_constructible<var3_type>, std::is_move_constructible<var4_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name, var3_type&& param3_name, var4_type&& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>, std::is_nothrow_move_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(param1_name) \
		, var2_name(std::move(param2_name)) \
		, var3_name(std::move(param3_name)) \
		, var4_name(std::move(param4_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_move_constructible<var3_type>, std::is_move_constructible<var4_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, const var2_type& param2_name, var3_type&& param3_name, var4_type&& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>, std::is_nothrow_move_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(param2_name) \
		, var3_name(std::move(param3_name)) \
		, var4_name(std::move(param4_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_copy_constructible<var3_type>, std::is_move_constructible<var4_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name, const var3_type& param3_name, var4_type&& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>, std::is_nothrow_move_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name(param3_name) \
		, var4_name(std::move(param4_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_move_constructible<var3_type>, std::is_copy_constructible<var4_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name, var3_type&& param3_name, const var4_type& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>, std::is_nothrow_copy_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name(std::move(param3_name)) \
		, var4_name(param4_name) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_move_constructible<var3_type>, std::is_move_constructible<var4_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name, var3_type&& param3_name, var4_type&& param4_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>, std::is_nothrow_move_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name(std::move(param3_name)) \
		, var4_name(std::move(param4_name)) \
	{} \
 \
	MAKE_READ_METHOD_V4(var1_name, var2_name, var3_name, var4_name); \
	MAKE_WRITE_METHODS(var1_name, var2_name, var3_name, var4_name); \
	__VA_ARGS__; \
 \
	var1_type var1_name; \
	var2_type var2_name; \
	var3_type var3_name; \
	var4_type var4_name; \
}

#define MAKE_EMPTY_PACKET_4VAR(name, protocol, var1_type, var1_name, var2_type, var2_name, var3_type, var3_name, var4_type, var4_name, ...) \
struct name : public iconer::app::BasicPacket \
{ \
	using Super = iconer::app::BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type) + sizeof(var2_type) + sizeof(var3_type) + sizeof(var4_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	constexpr name() \
		noexcept(std::conjunction_v<std::is_nothrow_default_constructible<var1_type>, std::is_nothrow_default_constructible<var2_type>, std::is_nothrow_default_constructible<var3_type>, std::is_nothrow_default_constructible<var4_type>>) \
		: Super(protocol, static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name() \
		, var2_name() \
		, var3_name() \
		, var4_name() \
	{} \
 \
	MAKE_READ_METHOD_V4(var1_name, var2_name, var3_name, var4_name); \
	__VA_ARGS__; \
 \
	var1_type var1_name; \
	var2_type var2_name; \
	var3_type var3_name; \
	var4_type var4_name; \
}
#pragma endregion
#pragma endregion
