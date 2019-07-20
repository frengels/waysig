#pragma once

#include <tuple>

#include "detail/intrusive_list.hpp"
#include "detail/link.hpp"

namespace ws
{
struct slot_base_impl
{
public:
    using function_type = void (*)(slot_base_impl*, void* data);

public:
    ws::detail::link link{};
    function_type    func{nullptr};

public:
    constexpr slot_base_impl() noexcept = default;
    constexpr slot_base_impl(function_type fn) noexcept : func{fn}
    {}

    constexpr void operator()(void* data)
    {
        func(this, data);
    }

    template<typename... Args>
    constexpr void operator()(Args&&... args)
    {
        auto fwd_args = std::forward_as_tuple(std::forward<Args>(args)...);
        func(this, &fwd_args);
    }
};

namespace detail
{
template<>
struct link_offset<ws::slot_base_impl>
    : std::integral_constant<std::size_t, offsetof(ws::slot_base_impl, link)>
{};
} // namespace detail
} // namespace ws