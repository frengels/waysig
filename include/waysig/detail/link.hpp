#pragma once

namespace ws
{
namespace detail
{
struct link
{
public:
    link* prev{nullptr};
    link* next{nullptr};

public:
    constexpr link() noexcept = default;
    constexpr link(link* prev, link* next) noexcept : prev{prev}, next{next}
    {}
};
} // namespace detail
} // namespace ws