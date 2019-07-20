#pragma once

namespace ws
{
namespace detail
{
class signal_base;

/// This is the class which is binary compatible with wl_listener, this is
/// verified through the provided unit tests. Any functions defined should
/// function exactly the same as the wl_listener_* ones.
struct slot_base
{
    friend class ws::detail::signal_base;
    friend class ws::detail::link_offset<slot_base>;

public:
    using function_type = void (*)(slot_base*, void* data);

public:
    ws::detail::link link{};
    function_type    func{nullptr};

public:
    constexpr slot_base() noexcept = default;
    constexpr slot_base(function_type fn) noexcept : func{fn}
    {}

    constexpr void operator()(void* data) noexcept
    {
        func(this, data);
    }
};

template<>
struct link_offset<ws::detail::slot_base>
    : std::integral_constant<std::size_t, offsetof(ws::detail::slot_base, link)>
{};
} // namespace detail
} // namespace ws