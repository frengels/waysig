#pragma once

#include "waysig/detail/slot_base.hpp"

namespace ws
{
// this is a basic slot, which can take the specified args. There is only room
// for a single function pointer.
template<typename... Args>
class slot : protected ws::detail::slot_base
{
public:
    using function_type = void (*)(slot*, Args...);

public:
    constexpr slot(function_type func) noexcept : ws::detail::slot_base{func}
    {}
};
} // namespace ws