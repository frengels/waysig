#include <catch2/catch.hpp>

#include "waysig/inplace_slot.hpp"
#include "waysig/signal.hpp"

// a more realistic scenario where the slot is part of a struct, see if we can
// derive the container from the slot information.

#define wl_container_of(ptr, sample, member)                                   \
    (__typeof__(sample))((char*) (ptr) -offsetof(__typeof__(*sample), member))

struct inplace_container
{
private:
    int i{0};

public:
    ws::inplace_slot<void()> slot{[this](auto&) { ++this->i; }};

    constexpr bool is(int j) const noexcept
    {
        return i == j;
    }
};

struct derive_container
{
private:
    int i{0};

public:
    ws::slot<void()> slot{[](auto& self) {
        auto& this_ = WS_CONTAINER_OF(self, derive_container, slot);
        ++this_.i;
    }};

    constexpr bool is(int j) const noexcept
    {
        return i == j;
    }
};

TEST_CASE("container")
{
    SECTION("inplace")
    {
        ws::signal<void()> sig;

        derive_container c0;

        sig.connect(c0.slot);
        sig();
        REQUIRE(c0.is(1));

        derive_container c1;

        sig.connect(c1.slot);
        sig();
        REQUIRE(c0.is(2));
        REQUIRE(c1.is(1));
    }

    SECTION("this")
    {
        ws::signal<void()> sig;

        derive_container c0;

        sig.connect(c0.slot);
        sig();
        REQUIRE(c0.is(1));

        derive_container c1;
        sig.connect(c1.slot);
        sig();
        REQUIRE(c0.is(2));
        REQUIRE(c1.is(1));
    }
}