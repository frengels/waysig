#include <catch2/catch.hpp>

#include "waysig/signal.hpp"

TEST_CASE("signal")
{
    int i = 0;

    ws::signal_base sig;

    ws::slot_base_impl s0{[](auto* self, void* data) {
        (void) self;
        auto& tup = *static_cast<std::tuple<int&>*>(data);
        ++std::get<0>(tup);
    }};

    REQUIRE(i == 0);
    sig(i);
    REQUIRE(i == 0);
    sig.connect(s0);
    sig(i);
    REQUIRE(i == 1);
}