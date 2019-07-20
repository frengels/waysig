#pragma once

#include <tuple>

#include "detail/link.hpp"

namespace ws
{
struct slot_base_impl
{
public:
    using function_type = void (*)(slot_base_impl*, void* data);

public:
    ws::detail::link link;
    function_type    func;

public:
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
} // namespace ws