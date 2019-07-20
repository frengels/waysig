#pragma once

// free function overloads to allow seamless interop between wl_signal/listener
// and ws::signal/slot

#include <memory>
#include <type_traits>

#include <wayland-server-core.h>

#include "waysig/detail/signal_base.hpp"
#include "waysig/detail/slot_base.hpp"
#include "waysig/ext/wl_compat.hpp"

namespace ws
{
void connect(wl_signal& sig, wl_listener& listener) noexcept
{
    wl_signal_add(&sig, &listener);
}

void connect(wl_signal& sig, ws::detail::slot_base& slot) noexcept
{
    static_assert(
        ws::detail::is_slot_layout_compatible_v,
        "Slot binary layouts are not compatible, interop is not allowed");
    wl_signal* slot_cast = reinterpret_cast<wl_signal*>(&slot);
    wl_signal_add(&sig, slot_cast);
}

void connect(ws::detail::signal_base& sig, wl_listener& listener) noexcept
{
    static_assert(
        ws::detail::is_signal_layout_compatible_v,
        "Signal binary layouts are not compatible, interop is not allowed");
    static_assert(
        ws::detail::is_slot_layout_compatible_v,
        "Slot binary layouts are not compatible, interop is not allowed");

    ws::detail::slot_base& slot =
        *reinterpret_cast<ws::detail::slot_base*>(&listener);
    sig.connect(slot);
}

template<typename T>
void emit(wl_signal& sig, T* data) noexcept
{
    wl_signal_emit(&sig, static_cast<void*>(data));
}

template<typename T>
void emit(wl_signal& sig, T& data) noexcept
{
    wl_signal_emit(&sig, static_cast<void*>(std::addressof(data)));
}

template<typename... Args>
std::enable_if_t<(sizeof...(Args) >= 2)> emit(wl_signal& sig,
                                              Args&&... args) noexcept
{
    auto tup = std::forward_as_tuple(std::forward<Args>(args)...);
    wl_signal_emit(&sig, static_cast<void*>(&tup));
}
} // namespace ws