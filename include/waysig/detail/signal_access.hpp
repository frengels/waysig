#pragma once

#include "waysig/signal.hpp"

namespace ws
{
namespace detail
{
// this class is permitted access to the privately inherited signal_base and
// slot_base of signal/slot respectively. This is used to hide the ugly void*
// and link details of the base. Anything deriving from signal/slot should
// derive publicly and this will be able to access the base through the public
// inheritance.
class signal_access
{
public:
    template<typename Res, typename... Args>
    static constexpr ws::detail::signal_base<Res>&
    base(ws::signal<Res(Args...)>& s) noexcept
    {
        return *static_cast<ws::detail::signal_base<Res>*>(std::addressof(s));
    }

    template<typename Res, typename... Args>
    static constexpr const ws::detail::signal_base<Res>&
    base(const ws::signal<Res(Args...)>& s) noexcept
    {
        return *static_cast<const ws::detail::signal_base<Res>*>(
            std::addressof(s));
    }
};
} // namespace detail
} // namespace ws