#pragma once

#include <memory>
#include <type_traits>

#include "link.hpp"

namespace ws
{
namespace detail
{
template<typename T>
struct link_offset : std::integral_constant<std::size_t, T::link_offset>
{};

template<typename T>
class intrusive_list
{
public:
    using value_type      = T;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;
    using size_type       = int;
    using difference_type = int;

private:
    static constexpr std::size_t link_offset_ =
        ws::detail::link_offset<T>::value;

private:
    ws::detail::link link_;

public:
    constexpr intrusive_list() noexcept : link_{&link_, &link_}
    {}

    constexpr bool empty() const noexcept
    {
        // identical to wayland implementation
        return link_.next == &link_;
    }

    constexpr size_type size() const noexcept
    {
        // identical to wayland implementation
        size_type count{0};
        auto*     e = link_.next;

        while (e != &link_)
        {
            e = e->next;
            ++count;
        }

        return count;
    }

    constexpr void push_front(reference ref) noexcept
    {
        // obtain the location of the link within this structure
        auto* byte_ptr = reinterpret_cast<unsigned char*>(std::addressof(ref));
        byte_ptr += link_offset_; // this is it, back to link now
        ws::detail::link* l = reinterpret_cast<ws::detail::link*>(byte_ptr);

        // from here on identical to wayland (wl_list_insert)
        l->prev       = &link_;
        l->next       = link_.next;
        link_.next    = l;
        l->next->prev = l;
    }

    constexpr void insert_list(intrusive_list& other) noexcept
    {
        if (other.empty())
        {
            return;
        }

        // TODO figure out how the lists are actually merged here and whether or
        // not the signature should require an rvalue reference (probably
        // should)
        other.link_.next->prev = &link_;
        other.link_.prev->next = link_.next;
        link_.next->prev       = other.link_.prev;
        link_.next             = other.link_.next;
    }
};
} // namespace detail
} // namespace ws