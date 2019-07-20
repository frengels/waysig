#include <catch2/catch.hpp>

#include "waysig/detail/signal_base.hpp"
#include "waysig/detail/slot_base.hpp"

TEST_CASE("signal")
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