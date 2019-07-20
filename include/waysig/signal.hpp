#pragma once

#include "detail/intrusive_list.hpp"
#include "slot.hpp"

namespace ws
{
struct signal_base
{
public:
    ws::detail::intrusive_list<ws::slot_base_impl> slot_list;

public:
    constexpr signal_base() noexcept = default;

    constexpr void connect(ws::slot_base_impl& slot) noexcept
    {
        slot_list.push_front(slot);
    }

    constexpr void emit(void* data) noexcept
    {
        // iteration should be identical to wl_signal_emit
        auto end = slot_list.end();
        for (auto it = slot_list.begin(); it != end; ++it)
        {
            ws::slot_base_impl& slot = *it;
            slot(data);
        }
    }

    constexpr void operator()(void* data) noexcept
    {
        emit(data);
    }
};
} // namespace ws