#include <catch2/catch.hpp>

#include "waysig/detail/signal_base.hpp"
#include "waysig/detail/slot_base.hpp"
#include "waysig/signal.hpp"
#include "waysig/slot.hpp"

TEST_CASE("signal")
{
    SECTION("base")
    {
        int i = 0;

        ws::detail::signal_base<void> sig;

        ws::detail::slot_base<void> s0{[](auto* self, void* data) {
            (void) self;
            auto& i = *static_cast<int*>(data);
            ++i;
        }};

        REQUIRE(i == 0);
        sig.emit(&i);
        REQUIRE(i == 0);
        sig.connect(s0);
        sig.emit(&i);
        REQUIRE(i == 1);
    }

    // empty base optimization success
    static_assert(sizeof(ws::detail::slot_base<void>) ==
                  sizeof(ws::slot<void(int&)>));

    SECTION("derived")
    {
        int                    i = 0;
        ws::signal<void(int&)> sig;

        ws::slot<void(int&)> s0{[](auto* self, int& i) {
            (void) self;
            ++i;
        }};

        REQUIRE(i == 0);
        sig.emit(i);
        REQUIRE(i == 0);
        sig.connect(s0);
        sig.emit(i);
        REQUIRE(i == 1);
    }
}