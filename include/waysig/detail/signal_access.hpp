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

    template<typename T, typename Res>
    static constexpr T& derived(ws::detail::signal_base<Res>& sig_b) noexcept
    {
        static_assert(
            std::is_base_of_v<ws::detail::signal_base<Res>, T>,
            "Cannot cast because T is not derived from signal_base<Res>");
        return *static_cast<T*>(&sig_b);
    }

    template<typename T, typename Res>
    static constexpr const T&
    derived(const ws::detail::signal_base<Res>& sig_b) noexcept
    {
        static_assert(
            std::is_base_of_v<ws::detail::signal_base<Res>, T>,
            "Cannot cast because T is not derived from signal_base<Res>");
        return *static_cast<const T*>(&sig_b);
    }
};
} // namespace detail
} // namespace ws