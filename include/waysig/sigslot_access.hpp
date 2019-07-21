#pragma once

#include "waysig/detail/signal_base.hpp"
#include "waysig/detail/slot_base.hpp"

namespace ws
{
class sigslot_access
{
public:
    template<typename Ret, typename T>
    static constexpr std::enable_if_t<
        std::is_base_of_v<ws::detail::signal_base<Ret>, T>,
        ws::detail::signal_base<Ret>&>
    read_signal_base(T& t) noexcept
    {
        return *static_cast<ws::detail::signal_base<Ret>*>(std::addressof(t));
    }

    template<typename Ret, typename T>
    static constexpr std::enable_if_t<
        std::is_base_of_v<ws::detail::slot_base<Ret>, T>,
        ws::detail::slot_base<Ret>&>
    read_slot_base(T& t) noexcept
    {
        return *static_cast<ws::detail::slot_base<Ret>*>(std::addressof(t));
    }
};
} // namespace ws