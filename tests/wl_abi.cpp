#include <catch2/catch.hpp>

#include <type_traits>

#undef WAYSIG_ENABLE_WL
#include <ws/listener.hpp>
#include <ws/signal.hpp>

#include <wayland-server-core.h>

TEST_CASE("wayland")
{
    SECTION("abi")
    {
        SECTION("list")
        {
            static_assert(sizeof(ws::detail::list) == sizeof(wl_list));
            static_assert(offsetof(ws::detail::list, prev) ==
                          offsetof(wl_list, prev));
            static_assert(offsetof(ws::detail::list, next) ==
                          offsetof(wl_list, next));
        }

        SECTION("listener")
        {
            static_assert(sizeof(ws::detail::listener) == sizeof(wl_listener));
            static_assert(offsetof(ws::detail::listener, link) ==
                          offsetof(wl_listener, link));
            static_assert(offsetof(ws::detail::listener, notify) ==
                          offsetof(wl_listener, notify));
        }

        SECTION("signal")
        {
            static_assert(sizeof(ws::detail::signal) == sizeof(wl_signal));
            static_assert(offsetof(ws::detail::signal, listener_list) ==
                          offsetof(wl_signal, listener_list));
        }
    }
}
