#include <catch2/catch.hpp>

#include <type_traits>

#include <wayland-server-core.h>
#include <wayland-util.h>

#include "waysig/detail/intrusive_list.hpp"
#include "waysig/detail/link.hpp"
#include "waysig/signal.hpp"
#include "waysig/slot.hpp"

TEST_CASE("wayland")
{
    SECTION("abi")
    {
        SECTION("link")
        {
            static_assert(sizeof(wl_list) == sizeof(ws::detail::link));
            static_assert(alignof(wl_list) == alignof(ws::detail::link));

            static_assert(offsetof(wl_list, prev) ==
                          offsetof(ws::detail::link, prev));
            static_assert(offsetof(wl_list, next) ==
                          offsetof(ws::detail::link, next));
        }

        SECTION("list")
        {
            static_assert(sizeof(wl_list) ==
                          sizeof(ws::detail::intrusive_list<int>));
            static_assert(alignof(wl_list) ==
                          alignof(ws::detail::intrusive_list<int>));
            // these 2 properties are enough to ensure binary compatibility. The
            // underlying object to intrusive list has already been checked for
            // ABI.
        }

        SECTION("listener")
        {
            static_assert(sizeof(wl_listener) == sizeof(ws::slot_base_impl));
            static_assert(alignof(wl_listener) == alignof(ws::slot_base_impl));

            static_assert(
                offsetof(wl_listener, link) ==
                offsetof(ws::slot_base_impl,
                         link)); // 0 is the offset of the base in inheritance
            static_assert(offsetof(wl_listener, notify) ==
                          offsetof(ws::slot_base_impl, func));

            // since the layout of the 2 listener types are the same, the
            // function pointers will be able to interoperate.
        }

        SECTION("signal")
        {
            static_assert(sizeof(wl_signal) == sizeof(ws::signal_base));
            static_assert(alignof(wl_signal) == alignof(ws::signal_base));

            static_assert(offsetof(wl_signal, listener_list) ==
                          offsetof(ws::signal_base, slot_list));
        }
    }
}