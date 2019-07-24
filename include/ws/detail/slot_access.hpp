#pragma once

#include "ws/slot.hpp"

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

    template<typename T, typename Res>
    static constexpr T& derived(ws::detail::slot_base<Res>& slot_b) noexcept
    {
        static_assert(
            std::is_base_of_v<ws::detail::slot_base<Res>, T>,
            "Cannot cast because T is not derived from slot_base<Res>");
        return *static_cast<T*>(&slot_b);
    }

    template<typename T, typename Res>
    static constexpr const T&
    derived(const ws::detail::slot_base<Res>& slot_b) noexcept
    {
        static_assert(
            std::is_base_of_v<ws::detail::slot_base<Res>, T>,
            "Cannot cast because T is not derived from slot_base<Res>");
        return *static_cast<const T*>(&slot_b);
    }
};
} // namespace detail
} // namespace ws