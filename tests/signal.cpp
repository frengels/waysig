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

        ws::slot<void(int&)> s0{[](auto& self, int& i) {
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

    SECTION("scope")
    {
        int                    i = 0;
        ws::signal<void(int&)> sig;

        ws::slot<void(int&)> s0{[](auto& self, int& i) {
            (void) self;
            ++i;
        }};
        ws::slot<void(int&)> s1{[](auto& self, int& i) {
            (void) self;
            ++i;
        }};

        sig.connect(s0);

        sig(i);
        REQUIRE(i == 1);
        i = 0;

        sig.connect(s1);

        sig(i);
        REQUIRE(i == 2);
        i = 0;

        {
            ws::slot<void(int&)> s2{[](auto& self, int& i) {
                (void) self;
                ++i;
            }};

            sig.connect(s2);

            sig(i);
            REQUIRE(i == 3);
            i = 0;
        }

        sig(i);
        REQUIRE(i == 2);
        i = 0;
    }

    SECTION("emit_order")
    {
        int                    i = 0;
        ws::signal<void(int&)> sig;

        ws::slot<void(int&)> s0{[](auto&, int& i) {
            REQUIRE(i == 0);
            ++i;
        }};

        ws::slot<void(int&)> s1{[](auto&, int& i) {
            REQUIRE(i == 1);
            ++i;
        }};

        sig.connect(s0);
        sig(i);
        REQUIRE(i == 1);

        i = 0;
        ws::connect(sig, s1);
        // sig.connect(s1);
        sig(i);
        REQUIRE(i == 2);

        SECTION("invoke_slot")
        {
            // s0 and s1 are connected
            i = 0;
            s0(i);
            REQUIRE(i == 1);
            s1(i);
            REQUIRE(i == 2);
        }

        SECTION("move")
        {
            // s0 and s1 are connected

            ws::slot<void(int&)> s2 = std::move(s1);
            i                       = 0;

            REQUIRE(!s1.connected());
            sig(i);
            REQUIRE(i == 2);
        }

        SECTION("remove_self")
        {
            // s0 and s1 are connected
            // in here we test whether removing a slot itself it safe, this is
            // the reason why the iterator in intrusive_list stores a pointer
            // for current and next.
            // In the case that we self remove current->next will be invalid.
            i = 0;

            auto s2 = ws::slot<void(int&)>{
                [](ws::slot<void(int&)>& self, auto) { self.disconnect(); }};

            auto s3 = ws::slot<void(int&)>{[](auto&, int& i) {
                REQUIRE(i == 2);
                ++i;
            }};

            sig.connect(s2);
            sig.connect(s3);

            sig(i);
            REQUIRE(i == 3);

            SECTION("disconnect")
            {
                i = 0;
                s0.disconnect();
                s1.disconnect();
                s2.disconnect(); // duplicate disconnect should be safe
                s3.disconnect();
            }
        }
    }
}