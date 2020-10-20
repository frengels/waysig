#include <catch2/catch.hpp>

#include <type_traits>

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

	SECTION("derived")
	{
#ifdef WAYSIG_ENABLE_WL
	   static_assert(std::is_base_of<wl_list, ws::detail::list>::value);
	   static_assert(std::is_base_of<wl_signal, ws::detail::signal>::value);
	   static_assert(std::is_base_of<wl_listener, ws::detail::listener>::value);
#else
	   static_assert(!std::is_base_of<wl_list, ws::detail::list>::value);
	   static_assert(!std::is_base_of<wl_signal, ws::detail::signal>::value);
	   static_assert(!std::is_base_of<wl_listener, ws::detail::listener>::value);
#endif
	}
    }
}
