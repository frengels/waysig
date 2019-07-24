#pragma once

#include "ws/detail/link.hpp"

namespace ws
{
namespace detail
{
template<typename Ret>
class signal_base;

/// This is the class which is binary compatible with wl_listener, this is
/// verified through the provided unit tests. Any functions defined should
/// function exactly the same as the wl_listener_* ones.
template<typename Ret = void>
struct slot_base
{
    friend class ws::detail::signal_base<Ret>;
    friend class ws::detail::link_offset<slot_base<Ret>>;

public:
    using result_type   = Ret;
    using function_type = result_type (*)(slot_base<result_type>*, void* data);

public:
    ws::detail::link link{};
    function_type    func{nullptr};

public:
    constexpr slot_base() noexcept = default;
    constexpr slot_base(function_type fn) noexcept : func{fn}
    {}

    constexpr result_type operator()(void* data) noexcept
    {
        return func(this, data);
    }

    constexpr bool connected() const noexcept
    {
        return link.linked();
    }

    constexpr void disconnect() noexcept
    {
        link.remove(); // automatically checks whether the link is linked before
                       // removing
    }
};

template<typename Ret>
struct link_offset<ws::detail::slot_base<Ret>>
    : std::integral_constant<std::size_t,
                             offsetof(ws::detail::slot_base<Ret>, link)>
{};
} // namespace detail
} // namespace ws