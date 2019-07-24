#include <catch2/catch.hpp>

#include "ws/inplace_slot.hpp"
#include "ws/signal.hpp"

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

// check how reliable the WS_CONTAINER_OF trick is with rather complex
// hierarchies
struct base_a
{
private:
    int i_a{0};

public:
    virtual ~base_a() = default;
    virtual void do_something()
    {
        i_a += 10;
    }
};

struct base_b
{
private:
    float f_b{0.0f};

public:
    virtual ~base_b() = default;

    virtual void add_5()
    {
        f_b += 5.0f;
    }
};

// 2 polymorphic bases
struct complex : base_a, base_b
{
private:
    int              j{0}; // should be hard to get to with 2 polymorphic bases
    ws::slot<void()> s0{[](auto& s) {
        auto& this_ = WS_CONTAINER_OF(s, complex, s0);
        this_.do_something();
        this_.add_5();
    }};

public:
    virtual ~complex() = default;

    void add_5() override
    {
        j += 5;
    }

    void do_something() override
    {
        j += 10;
    }

    auto& get_slot()
    {
        return s0;
    }

    bool is(int i) const
    {
        return j == i;
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

    SECTION("complex")
    {
        complex c;

        c.get_slot()();

        REQUIRE(c.is(15));
    }
}