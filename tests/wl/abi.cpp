#include <catch2/catch.hpp>

#include <type_traits>

#include <wayland-server-core.h>
#include <wayland-util.h>

#include "waysig/ext/wl_compat.hpp"

TEST_CASE("wayland")
{
    SECTION("abi")
    {
        SECTION("link")
        {
            static_assert(ws::detail::is_link_layout_compatible_v);
        }

        SECTION("list")
        {
            static_assert(ws::detail::is_list_layout_compatible_v);
        }

        SECTION("listener")
        {
            static_assert(ws::detail::is_slot_layout_compatible_v);
        }

        SECTION("signal")
        {
            static_assert(ws::detail::is_signal_layout_compatible_v);
        }
    }
}