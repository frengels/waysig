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
    // the default for the wayland link's empty state is to have both
    // uninitialized or null, in this case they're default to null.
    constexpr link() noexcept = default;

    // this constructor should be used for making a wayland compatible list. In
    // lists the list link is made to reference itself upon initialization,
    // creating a circular doubly linked list.
    constexpr link(link* prev, link* next) noexcept : prev{prev}, next{next}
    {}

    constexpr void remove() noexcept
    {
        // identical to wayland implementation
        prev->next = next;
        next->prev = prev;
        next       = nullptr;
        prev       = nullptr;
    }
};
} // namespace detail
} // namespace ws