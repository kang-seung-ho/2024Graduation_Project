#define MAKE_EMPTY_PACKET(name, protocol) \
struct [[nodiscard]] name : public BasicPacket \
{ \
	using Super = BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept { return Super::MinSize(); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept { return Super::SignedMinSize(); } \
 \
	constexpr name() noexcept \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
	{ \
	} \
 \
	[[nodiscard]] constexpr auto Serialize() const { return iconer::util::Serializes(myProtocol, mySize); } \
}

#define MAKE_SERIALIZE_METHOD(...) \
[[nodiscard]] constexpr auto Serialize() const \
{ \
	return iconer::util::Serializes(myProtocol, mySize, __VA_ARGS__); \
}

#define ICONER_CONDITIONAL_false(...) 
#define ICONER_CONDITIONAL_true(...) __VA_ARGS__
#define ICONER_CONDITIONAL(flag, ...) ICONER_CONDITIONAL_##flag(__VA_ARGS__)

#pragma region 1 Field
#define MAKE_EMPTY_PACKET_1VAR_WITH_DEFAULT_EX(name, protocol, var1_type, var1_name, param1_name, var1_default_value, serializer_method) \
struct [[nodiscard]] name : public BasicPacket \
{ \
	using Super = BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	template<std::enable_if_t<std::is_constructible_v<var1_type, decltype(var1_default_value)>, int> = 0> \
	constexpr name() \
		noexcept(std::is_nothrow_constructible_v<var1_type, decltype(var1_default_value)>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((var1_default_value)) \
	{} \
 \
	template<std::enable_if_t<std::is_copy_constructible_v<var1_type>, int> = 0>\
	constexpr name(const var1_type& param1_name) \
		noexcept(std::is_nothrow_copy_constructible_v<var1_type>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
	{} \
 \
	template<std::enable_if_t<std::is_move_constructible_v<var1_type>, int> = 0>\
	constexpr name(var1_type&& param1_name) \
		noexcept(std::is_nothrow_move_constructible_v<var1_type>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
	{} \
 \
	constexpr std::byte* Write(std::byte* buffer) const \
	{ \
		return iconer::util::Serialize(Super::Write(buffer), var1_name); \
	} \
 \
	constexpr const std::byte* Read(const std::byte* buffer) \
	{ \
		return iconer::util::Deserialize(Super::Read(buffer), var1_name); \
	} \
 \
	serializer_method; \
 \
	var1_type var1_name; \
}

#define MAKE_EMPTY_PACKET_1VAR_WITH_DEFAULT(name, protocol, var1_type, var1_name, param1_name, var1_default_value) \
MAKE_EMPTY_PACKET_1VAR_WITH_DEFAULT_EX(name, protocol, var1_type, var1_name, param1_name, (var1_default_value), MAKE_SERIALIZE_METHOD(var1_name))

#define MAKE_EMPTY_PACKET_1VAR_EX(name, protocol, var1_type, var1_name, param1_name, serializer_method, create_default_ctor) \
struct [[nodiscard]] name : public BasicPacket \
{ \
	using Super = BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	ICONER_CONDITIONAL(create_default_ctor, \
	template<std::enable_if_t<std::is_default_constructible_v<var1_type>, int> = 0> \
	constexpr name() \
		noexcept(std::is_nothrow_default_constructible_v<var1_type>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name() \
	{}) \
 \
	template<std::enable_if_t<std::is_copy_constructible_v<var1_type>, int> = 0>\
	constexpr name(const var1_type& param1_name) \
		noexcept(std::is_nothrow_copy_constructible_v<var1_type>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
	{} \
 \
	template<std::enable_if_t<std::is_move_constructible_v<var1_type>, int> = 0>\
	constexpr name(var1_type&& param1_name) \
		noexcept(std::is_nothrow_move_constructible_v<var1_type>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
	{} \
 \
	constexpr std::byte* Write(std::byte* buffer) const \
	{ \
		return iconer::util::Serialize(Super::Write(buffer), var1_name); \
	} \
 \
	constexpr const std::byte* Read(const std::byte* buffer) \
	{ \
		return iconer::util::Deserialize(Super::Read(buffer), var1_name); \
	} \
 \
	serializer_method; \
 \
	var1_type var1_name; \
}

#define MAKE_EMPTY_PACKET_1VAR(name, protocol, var1_type, var1_name, param1_name, create_default_ctor) \
MAKE_EMPTY_PACKET_1VAR_EX(name, protocol, var1_type, var1_name, param1_name, MAKE_SERIALIZE_METHOD(var1_name), create_default_ctor)
#pragma endregion

#pragma region 2 Fields
#define MAKE_EMPTY_PACKET_2VAR_WITH_DEFAULT_EX(name, protocol, var1_type, var1_name, param1_name, var1_default_value, var2_type, var2_name, param2_name, var2_default_value, serializer_method) \
struct [[nodiscard]] name : public BasicPacket \
{ \
	using Super = BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type) + sizeof(var2_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_constructible<var1_type, decltype(var1_default_value)>, std::is_constructible<var2_type, decltype(var2_default_value)>>, int> = 0>\
	constexpr name() \
		noexcept(std::conjunction_v<std::is_nothrow_constructible<var1_type, decltype(var1_default_value)>, std::is_nothrow_constructible<var2_type, decltype(var2_default_value)>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((var1_default_value)) \
		, var2_name((var2_default_value)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name((param2_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, const var2_type& param2_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name((param2_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name(std::move(param2_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
	{} \
 \
	constexpr std::byte* Write(std::byte* buffer) const \
	{ \
		return iconer::util::Serialize(iconer::util::Serialize(Super::Write(buffer), var1_name), var2_name); \
	} \
 \
	constexpr const std::byte* Read(const std::byte* buffer) \
	{ \
		return iconer::util::Deserialize(iconer::util::Deserialize(Super::Read(buffer), var1_name), var2_name); \
	} \
 \
	serializer_method; \
 \
	var1_type var1_name; \
	var2_type var2_name; \
}
#define MAKE_EMPTY_PACKET_2VAR_WITH_DEFAULT(name, protocol, var1_type, var1_name, param1_name, var1_default_value, var2_type, var2_name, param2_name, var2_default_value) \
MAKE_EMPTY_PACKET_2VAR_WITH_DEFAULT_EX(name, protocol, var1_type, var1_name, param1_name, (var1_default_value), var2_type, var2_name, param2_name, (var2_default_value), MAKE_SERIALIZE_METHOD(var1_name, var2_name))

#define MAKE_EMPTY_PACKET_2VAR_EX(name, protocol, var1_type, var1_name, param1_name, var2_type, var2_name, param2_name, serializer_method) \
struct [[nodiscard]] name : public BasicPacket \
{ \
	using Super = BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type) + sizeof(var2_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_nothrow_default_constructible<var1_type>, std::is_nothrow_default_constructible<var2_type>>, int> = 0>\
	constexpr name() \
		noexcept(std::conjunction_v<std::is_nothrow_default_constructible<var1_type>, std::is_nothrow_default_constructible<var2_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name() \
		, var2_name() \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name((param2_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_copy_constructible<var2_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, const var2_type& param2_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name((param2_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name(std::move(param2_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
	{} \
 \
	constexpr std::byte* Write(std::byte* buffer) const \
	{ \
		return iconer::util::Serialize(iconer::util::Serialize(Super::Write(buffer), var1_name), var2_name); \
	} \
 \
	constexpr const std::byte* Read(const std::byte* buffer) \
	{ \
		return iconer::util::Deserialize(iconer::util::Deserialize(Super::Read(buffer), var1_name), var2_name); \
	} \
 \
	serializer_method; \
 \
	var1_type var1_name; \
	var2_type var2_name; \
}

#define MAKE_EMPTY_PACKET_2VAR(name, protocol, var1_type, var1_name, param1_name, var2_type, var2_name, param2_name) \
MAKE_EMPTY_PACKET_2VAR_EX(name, protocol, var1_type, var1_name, param1_name, var2_type, var2_name, param2_name, MAKE_SERIALIZE_METHOD(var1_name, var2_name))
#pragma endregion

#pragma region 3 Fields
#define MAKE_EMPTY_PACKET_3VAR_WITH_DEFAULT_EX(name, protocol, var1_type, var1_name, param1_name, var1_default_value, var2_type, var2_name, param2_name, var2_default_value, var3_type, var3_name, param3_name, var3_default_value, serializer_method) \
struct [[nodiscard]] name : public BasicPacket \
{ \
	using Super = BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type) + sizeof(var2_type) + sizeof(var3_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_constructible<var1_type, decltype(var1_default_value)>, std::is_constructible<var2_type, decltype(var2_default_value)>, std::is_constructible<var3_type, decltype(var3_default_value)>>, int> = 0>\
	constexpr name() \
		noexcept(std::conjunction_v<std::is_nothrow_constructible<var1_type, decltype(var1_default_value)>, std::is_nothrow_constructible<var2_type, decltype(var2_default_value)>, std::is_nothrow_constructible<var2_type, decltype(var2_default_value)>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((var1_default_value)) \
		, var2_name((var2_default_value)) \
		, var3_name((var3_default_value)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_copy_constructible<var3_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name, const var3_type& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name((param2_name)) \
		, var3_name((param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_copy_constructible<var3_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, const var2_type& param2_name, const var3_type& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name((param2_name)) \
		, var3_name((param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_copy_constructible<var3_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name, const var3_type& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name((param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_move_constructible<var3_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name, var3_type&& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name((param2_name)) \
		, var3_name(std::move(param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_copy_constructible<var3_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name, const var3_type& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name((param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_move_constructible<var3_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name, var3_type&& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name(std::move(param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_move_constructible<var3_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, const var2_type& param2_name, var3_type&& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name((param2_name)) \
		, var3_name(std::move(param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_move_constructible<var3_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name, var3_type&& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name(std::move(param3_name)) \
	{} \
 \
	constexpr std::byte* Write(std::byte* buffer) const \
	{ \
		return iconer::util::Serialize(iconer::util::Serialize(iconer::util::Serialize(Super::Write(buffer), var1_name), var2_name), var3_name); \
	} \
 \
	constexpr const std::byte* Read(const std::byte* buffer) \
	{ \
		return iconer::util::Deserialize(iconer::util::Deserialize(iconer::util::Deserialize(Super::Read(buffer), var1_name), var2_name), var3_name); \
	} \
 \
	serializer_method; \
 \
	var1_type var1_name; \
	var2_type var2_name; \
	var3_type var3_name; \
}
#define MAKE_EMPTY_PACKET_3VAR_WITH_DEFAULT(name, protocol, var1_type, var1_name, param1_name, var1_default_value, var2_type, var2_name, param2_name, var2_default_value) \
MAKE_EMPTY_PACKET_3VAR_WITH_DEFAULT_EX(name, protocol, var1_type, var1_name, param1_name, (var1_default_value), var2_type, var2_name, param2_name, (var2_default_value), var3_type, var3_name, param3_name, (var3_default_value), MAKE_SERIALIZE_METHOD(var1_name, var2_name, var3_name))

#define MAKE_EMPTY_PACKET_3VAR_EX(name, protocol, var1_type, var1_name, param1_name, var2_type, var2_name, param2_name, var3_type, var3_name, param3_name, serializer_method, create_default_ctor) \
struct [[nodiscard]] name : public BasicPacket \
{ \
	using Super = BasicPacket; \
 \
	[[nodiscard]] static consteval size_t WannabeSize() noexcept \
	{ return Super::MinSize() + sizeof(var1_type) + sizeof(var2_type) + sizeof(var3_type); } \
	[[nodiscard]] static consteval ptrdiff_t SignedWannabeSize() noexcept \
	{ return static_cast<ptrdiff_t>(WannabeSize()); } \
 \
	ICONER_CONDITIONAL(create_default_ctor, \
	template<std::enable_if_t<DEFER_BOOLEAN(create_default_ctor) and std::conjunction_v<std::is_nothrow_default_constructible<var1_type>, std::is_nothrow_default_constructible<var2_type>, std::is_nothrow_default_constructible<var3_type>>, int> = 0>\
	constexpr name() \
		noexcept(std::conjunction_v<std::is_nothrow_default_constructible<var1_type>, std::is_nothrow_default_constructible<var2_type>, std::is_nothrow_default_constructible<var3_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name() \
		, var2_name() \
		, var3_name() \
	{}) \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_copy_constructible<var3_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name, const var3_type& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name((param2_name)) \
		, var3_name((param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_copy_constructible<var3_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, const var2_type& param2_name, const var3_type& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name((param2_name)) \
		, var3_name((param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_copy_constructible<var3_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name, const var3_type& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name((param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_move_constructible<var3_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, const var2_type& param2_name, var3_type&& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name((param2_name)) \
		, var3_name(std::move(param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_copy_constructible<var3_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name, const var3_type& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_copy_constructible<var3_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name((param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_copy_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_move_constructible<var3_type>>, int> = 0>\
	constexpr name(const var1_type& param1_name, var2_type&& param2_name, var3_type&& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name((param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name(std::move(param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_copy_constructible<var2_type>, std::is_move_constructible<var3_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, const var2_type& param2_name, var3_type&& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_copy_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name((param2_name)) \
		, var3_name(std::move(param3_name)) \
	{} \
 \
	template<std::enable_if_t<std::conjunction_v<std::is_move_constructible<var1_type>, std::is_move_constructible<var2_type>, std::is_move_constructible<var3_type>>, int> = 0>\
	constexpr name(var1_type&& param1_name, var2_type&& param2_name, var3_type&& param3_name) \
		noexcept(std::conjunction_v<std::is_nothrow_move_constructible<var1_type>, std::is_nothrow_move_constructible<var2_type>, std::is_nothrow_move_constructible<var3_type>>) \
		: Super((protocol), static_cast<std::int16_t>(SignedWannabeSize())) \
		, var1_name(std::move(param1_name)) \
		, var2_name(std::move(param2_name)) \
		, var3_name(std::move(param3_name)) \
	{} \
 \
	constexpr std::byte* Write(std::byte* buffer) const \
	{ \
		return iconer::util::Serialize(iconer::util::Serialize(iconer::util::Serialize(Super::Write(buffer), var1_name), var2_name), var3_name); \
	} \
 \
	constexpr const std::byte* Read(const std::byte* buffer) \
	{ \
		return iconer::util::Deserialize(iconer::util::Deserialize(iconer::util::Deserialize(Super::Read(buffer), var1_name), var2_name), var3_name); \
	} \
 \
	serializer_method; \
 \
	var1_type var1_name; \
	var2_type var2_name; \
	var3_type var3_name; \
}

#define MAKE_EMPTY_PACKET_3VAR(name, protocol, var1_type, var1_name, param1_name, var2_type, var2_name, param2_name, var3_type, var3_name, param3_name) \
MAKE_EMPTY_PACKET_3VAR_EX(name, protocol, var1_type, var1_name, param1_name, var2_type, var2_name, param2_name, var3_type, var3_name, param3_name, MAKE_SERIALIZE_METHOD(var1_name, var2_name, var3_name))
#pragma endregion
