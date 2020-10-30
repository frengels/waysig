#pragma once

#include <type_traits>

namespace ws
{
namespace detail
{
template<typename T, typename U>
std::size_t offset_of(T U::*mem) noexcept
{
    return reinterpret_cast<std::size_t>(
        __builtin_addressof(static_cast<U*>(nullptr)->*mem));
}

} // namespace detail

template<typename T, typename U>
U& container_of(T& ref, T U::*mem) noexcept
{
    std::size_t offset = ws::detail::offset_of(mem);

    const volatile char* p = &reinterpret_cast<const volatile char&>(ref);
    p -= offset;
    return const_cast<U&>(*reinterpret_cast<const volatile U*>(p));
}
} // namespace ws

#define WS_CONTAINER_OF(ref, ty, member)                                       \
    const_cast<ty&>(*reinterpret_cast<const volatile ty*>(                     \
        reinterpret_cast<const volatile char*>(__builtin_addressof(ref)) -     \
        offsetof(ty, member)))
