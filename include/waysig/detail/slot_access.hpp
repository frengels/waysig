#pragma once

#include "waysig/slot.hpp"

namespace ws
{
namespace detail
{
class slot_access
{
public:
    template<typename Res, typename... Args>
    static constexpr ws::detail::slot_base<Res>&
    base(ws::slot<Res(Args...)>& s) noexcept
    {
        return *static_cast<ws::detail::slot_base<Res>*>(std::addressof(s));
    }

    template<typename Res, typename... Args>
    static constexpr const ws::detail::slot_base<Res>&
    base(const ws::slot<Res(Args...)>& s) noexcept
    {
        return *static_cast<const ws::detail::slot_base<Res>*>(
            std::addressof(s));
    }
};
} // namespace detail
} // namespace ws