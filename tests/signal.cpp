#include <catch2/catch.hpp>

#include "ws/detail/signal_base.hpp"
#include "ws/detail/slot_base.hpp"
#include "ws/listener.hpp"
#include "ws/signal.hpp"
#include "ws/slot.hpp"

struct move_copy_counter
{
    bool moved_from{false};
    int  copy_construct{0};
    int  copy_assign{0};
    int  move_construct{0};
    int  move_assign{0};

    constexpr move_copy_counter() noexcept = default;

    constexpr move_copy_counter(const move_copy_counter& other) noexcept
        : copy_construct{other.copy_construct + 1},
          copy_assign{other.copy_assign}, move_construct{other.move_construct},
          move_assign{other.move_assign}
    {}

    constexpr move_copy_counter&
    operator=(const move_copy_counter& other) noexcept
    {
        copy_construct = other.copy_construct;
        copy_assign    = other.copy_assign + 1;
        move_construct = other.move_construct;
        move_assign    = other.move_assign;

        return *this;
    }

    constexpr move_copy_counter(move_copy_counter&& other) noexcept
        : copy_construct{other.copy_construct}, copy_assign{other.copy_assign},
          move_construct{other.move_construct + 1}, move_assign{
                                                        other.move_assign}
    {
        other.moved_from = true;
    }

    constexpr move_copy_counter& operator=(move_copy_counter&& other) noexcept
    {
        copy_construct = other.copy_construct;
        copy_assign    = other.copy_assign;
        move_construct = other.move_construct;
        move_assign    = other.move_assign + 1;

        other.moved_from = true;

        return *this;
    }
};

TEST_CASE("signal")
{
    SECTION("base")
    {
        int i = 0;

        ws::signal<int&> s;

        auto l = ws::listener<int&>{[](auto& self, int& data) {
            (void) self;
            ++data;
        }};

        REQUIRE(i == 0);
        s.emit(i);
        REQUIRE(i == 0);
        s.add(l);
        s.emit(i);
        REQUIRE(i == 1);

        ws::signal<void> s1;

        auto l1 = ws::listener<void>{[](auto& self) {}};

        s1.add(l1);
        s1.emit();
    }

    SECTION("scope")
    {
        int              i = 0;
        ws::signal<int&> sig;

        ws::listener<int&> s0{[](auto& self, int& i) {
            (void) self;
            ++i;
        }};

        ws::listener<int&> s1{[](auto& self, int& i) {
            (void) self;
            ++i;
        }};

        sig.add(s0);

        sig.emit(i);
        REQUIRE(i == 1);
        i = 0;

        sig.add(s1);

        sig.emit(i);
        REQUIRE(i == 2);
        i = 0;

        {
            ws::listener<int&> s2{[](auto& self, int& i) {
                (void) self;
                ++i;
            }};

            sig.add(s2);

            sig.emit(i);
            REQUIRE(i == 3);
            i = 0;
            // check destructor properly disconnects
        }

        sig.emit(i);
        REQUIRE(i == 2);
        i = 0;
    }

    SECTION("emit_order")
    {
        int              i = 0;
        ws::signal<int&> sig;

        ws::listener<int&> l0{[](auto&, int& i) {
            REQUIRE(i == 0);
            ++i;
        }};

        ws::listener<int&> l1{[](auto&, int& i) {
            REQUIRE(i == 1);
            ++i;
        }};

        sig.add(l0);
        sig.emit(i);
        REQUIRE(i == 1);

        i = 0;
        sig.add(l1);
        sig.emit(i);
        REQUIRE(i == 2);

        SECTION("invoke_slot")
        {
            i = 0;
            l0(i);
            REQUIRE(i == 1);
            l1(i);
            REQUIRE(i == 2);
        }

        SECTION("move")
        {
            ws::listener<int&> l2{std::move(l1)};
            i = 0;

            REQUIRE(!l1.is_connected());
            sig(i);
            REQUIRE(i == 2);
        }

        SECTION("remove_self")
        {
            i = 0;

            auto l2 = ws::listener<int&>{
                [](ws::listener<int&>& self, auto) { self.remove(); }};

            auto l3 = ws::listener<int&>{[](auto&, int& i) {
                REQUIRE(i == 2);
                ++i;
            }};

            sig.add(l2);
            sig.add(l3);

            sig(i);
            REQUIRE(i == 3);

            SECTION("disconnect")
            {
                l0.remove();
                l1.remove();
                // duplicate removes lead to UB, therefore don't remove l2
                // again
                l3.remove();
            }
        }
    }
}
