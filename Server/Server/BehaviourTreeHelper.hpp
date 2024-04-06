#define MAKE_STATE(name) \
struct name\
{\
constexpr name() noexcept = default; constexpr ~name() noexcept = default;\
template<typename OtherNode>\
static constexpr bool Transient = false;\
}

#define LINK_STATE(name, target)\
template<>\
constexpr bool name::template Transient<target> = true;

#define DOUBLELINK_STATES(Lhs, Rhs)\
LINK_STATE(Lhs, Rhs);\
LINK_STATE(Rhs, Lhs);
