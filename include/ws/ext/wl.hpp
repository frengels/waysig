#pragma once

// free function overloads to allow seamless interop between wl_signal/listener
// and ws::signal/slot

#include <memory>
#include <type_traits>

#include <wayland-server-core.h>

#include "ws/detail/util.hpp"
#include "ws/ext/wl_compat.hpp"
#include "ws/signal.hpp"
#include "ws/slot.hpp"

namespace ws
{
static inline void connect(wl_signal& sig, wl_listener& listener) noexcept
{
    wl_signal_add(&sig, &listener);
}

template<typename Res, typename... Args>
void connect(wl_signal& sig, ws::slot<Res(Args...)>& slot) noexcept
{
    static_assert(sizeof...(Args) <= 1,
                  "wl_signal will never send more than one argument.");
    if constexpr (sizeof...(Args) == 1)
    {
        using arg_type = std::tuple_element_t<0, std::tuple<Args...>>;

        static_assert(!std::is_const_v<arg_type>,
                      "wl_signal will never send a const qualified value");
        static_assert(std::is_lvalue_reference_v<arg_type> ||
                          std::is_pointer_v<arg_type>,
                      "We can only convert wl_signal argument to pointer or "
                      "lvalue reference");
    }
    static_assert(std::is_same_v<void, Res>,
                  "Cannot connect slot<Res(Args...)> with non void return type "
                  "to wl_signal");
    static_assert(
        ws::detail::is_slot_layout_compatible_v,
        "Slot binary layouts are not compatible, interop is not allowed");
    auto&        slot_base = ws::detail::slot_access::base(slot);
    wl_listener* slot_cast = reinterpret_cast<wl_listener*>(&slot_base);
    wl_signal_add(&sig, slot_cast);
}

template<typename Res, typename... Args>
void connect(ws::signal<Res(Args...)>& sig, wl_listener& listener) noexcept
{
    static_assert(sizeof...(Args) <= 1,
                  "wl_listener cannot take more than 1 argument.");

    if constexpr (sizeof...(Args) == 1)
    {
        using arg_type = std::tuple_element_t<0, std::tuple<Args...>>;

        static_assert(!std::is_const_v<arg_type>,
                      "wl_listener can't handle const arguments");

        static_assert(std::is_lvalue_reference_v<arg_type> ||
                          std::is_pointer_v<arg_type>,
                      "We can only convert to wl_listener's argument from "
                      "pointer or lvalue reference types.");
    }

    static_assert(std::is_same_v<void, Res>,
                  "Cannot connect wl_listener to signal expecting non void "
                  "return type, wl_listener always returns void");
    static_assert(
        ws::detail::is_signal_layout_compatible_v,
        "Signal binary layouts are not compatible, interop is not allowed");
    static_assert(
        ws::detail::is_slot_layout_compatible_v,
        "Slot binary layouts are not compatible, interop is not allowed");

    ws::detail::slot_base<Res>* slot_b =
        reinterpret_cast<ws::detail::slot_base<Res>*>(&listener);
    // this next step is a big assumption and the responsibility lies with the
    // user
    auto& slot =
        ws::detail::slot_access::derived<ws::slot<Res(Args...)>>(*slot_b);
    // ws::slot<Res(Args...)>& slot =
    //*static_cast<ws::slot<Res(Args...)>*>(slot_b);
    sig.connect(slot);
}

template<typename... Args>
void emit(wl_signal& sig, Args&&... args) noexcept
{
    auto pack = ws::detail::make_packaged_args(std::forward<Args>(args)...);
    wl_signal_emit(&sig, pack.void_ptr());
}
} // namespace ws
