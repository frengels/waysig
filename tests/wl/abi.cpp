#include <catch2/catch.hpp>

#include <type_traits>

#include <wayland-server-core.h>
#include <wayland-util.h>

#include "waysig/detail/link.hpp"
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

        SECTION("listener")
        {
            static_assert(sizeof(wl_listener) == sizeof(ws::slot_base_impl));
            static_assert(alignof(wl_listener) == alignof(ws::slot_base_impl));

            static_assert(offsetof(wl_listener, link) ==
                          0); // 0 is the offset of the base in inheritance
            static_assert(offsetof(wl_listener, notify) ==
                          offsetof(ws::slot_base_impl, func));

            // since the layout of the 2 listener types are the same, the
            // function pointers will be able to interoperate.
        }
    }
}