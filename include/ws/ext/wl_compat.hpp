#pragma once

#include <wayland-server-core.h>
#include <wayland-util.h>

#include "ws/detail/intrusive_list.hpp"
#include "ws/detail/link.hpp"
#include "ws/detail/signal_base.hpp"
#include "ws/detail/slot_base.hpp"

namespace ws
{
namespace detail
{
constexpr bool is_link_layout_compatible_v =
    (sizeof(wl_list) == sizeof(ws::detail::link)) &&
    (alignof(wl_list) == alignof(ws::detail::link)) &&
    (offsetof(wl_list, prev) == offsetof(ws::detail::link, prev)) &&
    (offsetof(wl_list, next) == offsetof(ws::detail::link, next));
constexpr bool is_list_layout_compatible_v =
    (sizeof(wl_list) ==
     sizeof(ws::detail::intrusive_list<ws::detail::slot_base<void>>)) &&
    (alignof(wl_list) ==
     alignof(ws::detail::intrusive_list<ws::detail::slot_base<void>>));
constexpr bool is_signal_layout_compatible_v =
    (sizeof(wl_signal) == sizeof(ws::detail::signal_base<void>)) &&
    (alignof(wl_signal) == alignof(ws::detail::signal_base<void>)) &&
    (offsetof(wl_signal, listener_list) ==
     offsetof(ws::detail::signal_base<void>, slot_list)) &&
    is_list_layout_compatible_v;
constexpr bool is_slot_layout_compatible_v =
    (sizeof(wl_listener) == sizeof(ws::detail::slot_base<void>)) &&
    (alignof(wl_listener) == alignof(ws::detail::slot_base<void>)) &&
    (offsetof(wl_listener, link) ==
     offsetof(ws::detail::slot_base<void>, link)) &&
    (offsetof(wl_listener, notify) ==
     offsetof(ws::detail::slot_base<void>, func)) &&
    is_link_layout_compatible_v;
} // namespace detail
} // namespace ws