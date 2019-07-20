#pragma once

#include "waysig/detail/intrusive_list.hpp"
#include "waysig/detail/slot_base.hpp"

namespace ws
{
namespace detail
{
struct signal_base
{
public:
    ws::detail::intrusive_list<ws::detail::slot_base> slot_list;

public:
    constexpr signal_base() noexcept = default;

    constexpr void connect(ws::detail::slot_base& slot) noexcept
    {
        slot_list.push_front(slot);
    }

    constexpr void emit(void* data) noexcept
    {
        // iteration should be identical to wl_signal_emit
        auto end = slot_list.end();
        for (auto it = slot_list.begin(); it != end; ++it)
        {
            ws::detail::slot_base& slot = *it;
            slot(data);
        }
    }

    constexpr void emit() noexcept
    {
        emit(nullptr);
    }

    constexpr void operator()(void* data) noexcept
    {
        emit(data);
    }

    constexpr void operator()() noexcept
    {
        emit(nullptr);
    }
};
} // namespace detail
} // namespace ws