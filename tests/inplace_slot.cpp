#include <catch2/catch.hpp>

#include "waysig/inplace_slot.hpp"
#include "waysig/signal.hpp"

TEST_CASE("inplace_slot")
{
    int i = 0;

    ws::signal<void()> sig;

    ws::inplace_slot<void()> s0{[&i](auto&) {
        REQUIRE(i == 0);
        ++i;
    }};

    SECTION("empty_signal")
    {
        sig();

        REQUIRE(i == 0);
    }

    SECTION("modify_capture")
    {
        sig.connect(s0);
        sig();
        REQUIRE(i == 1);
    }

    SECTION("move_construct")
    {
        sig.connect(s0);
        auto s1 = std::move(s0);
        sig();
        REQUIRE(i == 1);
    }
}