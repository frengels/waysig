#pragma once

#include "waysig/detail/intrusive_list.hpp"
#include "waysig/detail/slot_base.hpp"

namespace ws
{
namespace detail
{
template<typename Ret = void>
struct signal_base
{
public:
    using return_type = Ret;

public:
    ws::detail::intrusive_list<ws::detail::slot_base<return_type>> slot_list;

public:
    constexpr signal_base() noexcept = default;

    constexpr void connect(ws::detail::slot_base<return_type>& slot) noexcept
    {
        slot_list.push_front(slot);
    }

    constexpr void emit(void* data) noexcept
    {
        // iteration should be identical to wl_signal_emit
        auto end = slot_list.end();
        for (auto it = slot_list.begin(); it != end; ++it)
        {
            auto& slot = *it;
            slot(data);
        }
    }

    // Usually used together with back_inserter. This will push back all the
    // results into the vector. Only participates in overload resolution when
    // the slot return type is non void.
    template<typename OutputIt>
    constexpr std::enable_if_t<!std::is_same_v<void, return_type>>
    emit_collect(OutputIt out, void* data) noexcept
    {
        auto end = slot_list.end();
        for (auto it = slot_list.begin(); it != end; ++it, ++out)
        {
            auto& slot = *it;
            *out       = slot(data);
        }
    }
};
} // namespace detail
} // namespace ws