#include <catch2/catch.hpp>

#include "ws/detail/signal_base.hpp"
#include "ws/detail/slot_base.hpp"
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

    SECTION("output_iterator")
    {
        ws::signal<int(int)> sig;

        ws::slot<int(int)> doubler{[](auto&, int i) { return i * 2; }};
        ws::slot<int(int)> tripler{[](auto&, int i) { return i * 3; }};

        sig.connect(doubler);
        sig.connect(tripler);

        std::vector<int> res;

        sig(std::back_inserter(res), 5);

        REQUIRE(res[0] == 10);
        REQUIRE(res[1] == 15);
    }

    SECTION("value")
    {
        auto str = std::string{"Hello"};

        ws::signal<std::string(std::string)> sig;

        ws::slot<std::string(std::string)> move_out0{
            [](auto&, std::string str) { return str; }};
        ws::slot<std::string(std::string)> move_out1{
            [](auto&, std::string str) { return str; }};

        sig.connect(move_out0);
        sig.connect(move_out1);

        std::vector<std::string> res;

        sig(std::back_inserter(res), str);

        REQUIRE(str.compare("Hello") == 0);
        REQUIRE(res[0].compare("Hello") == 0);
        REQUIRE(res[1].compare("Hello") == 0);
    }

    SECTION("count_moves")
    {
        auto mcc = move_copy_counter{};

        ws::signal<move_copy_counter(move_copy_counter)> sig;

        ws::slot<move_copy_counter(move_copy_counter)> move_out0{
            [](auto&, move_copy_counter mcc) { return mcc; }};
        ws::slot<move_copy_counter(move_copy_counter)> move_out1{
            [](auto&, move_copy_counter mcc) { return mcc; }};

        sig.connect(move_out0);
        sig.connect(move_out1);

        std::vector<move_copy_counter> res;
        res.reserve(100); // reserve enough space to prevent further moves

        // process should be:
        // - copy into signal method
        // - move into packed_args
        // - copy into slot
        // - move into vector
        // - 2 move, 2 copy
        sig(std::back_inserter(res), mcc);

        REQUIRE(!mcc.moved_from);

        REQUIRE(!res[0].moved_from);
        REQUIRE(!res[1].moved_from);

        REQUIRE(res[0].copy_construct == res[1].copy_construct);
        REQUIRE(res[0].move_construct == res[1].move_construct);

        // this clearly shows, never pass by value unless very cheap
        REQUIRE(res[0].copy_construct == 2);
        REQUIRE(res[0].move_construct == 6); // not sure why 6?
    }

    SECTION("const_ref")
    {
        // same thing as above but with const ref, we expect less copies and
        // moves.
        auto mcc = move_copy_counter{};

        ws::signal<move_copy_counter(const move_copy_counter&)> sig;

        ws::slot<move_copy_counter(const move_copy_counter&)> move_out0{
            [](auto&, move_copy_counter mcc) { return mcc; }};
        ws::slot<move_copy_counter(const move_copy_counter&)> move_out1{
            [](auto&, move_copy_counter mcc) { return mcc; }};

        sig.connect(move_out0);
        sig.connect(move_out1);

        std::vector<move_copy_counter> res;
        res.reserve(100); // reserve enough space to prevent further moves

        // pass reference to slots
        // slot makes copy on return.
        // gets moved into vector
        sig(std::back_inserter(res), mcc);

        REQUIRE(!mcc.moved_from);

        REQUIRE(!res[0].moved_from);
        REQUIRE(!res[1].moved_from);

        REQUIRE(res[0].copy_construct == res[1].copy_construct);
        REQUIRE(res[0].move_construct == res[1].move_construct);

        REQUIRE(res[0].copy_construct == 1);
        REQUIRE(res[0].move_construct == 2);
    }
}