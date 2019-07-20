#pragma once

#include <cstddef>
#include <memory>
#include <type_traits>

#include "link.hpp"

namespace ws
{
namespace detail
{
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

public:
    class sentinel;

    class iterator
    {
        friend class sentinel;

    public:
        using value_type      = T;
        using reference       = T&;
        using pointer         = T*;
        using difference_type = std::ptrdiff_t;
        // only implement forward to ensure safe removal of elements whilst
        // iterating
        using iterator_category = std::forward_iterator_tag;

    private:
        ws::detail::link* current_{nullptr};
        ws::detail::link* next_{nullptr};

    public:
        constexpr iterator() noexcept = default;
        explicit constexpr iterator(ws::detail::link* curr) noexcept
            : current_{curr}, next_{current_->next}
        {}

        constexpr bool operator==(const iterator& other) const noexcept
        {
            return current_ == other.current_;
        }

        constexpr bool operator!=(const iterator& other) const noexcept
        {
            return current_ != other.current_;
        }

        constexpr iterator& operator++() noexcept
        {
            // this first assignment is essential. Because if the element at
            // current removed itself then next for this element will be nullptr
            // and we'd segfault.
            current_ = next_;
            next_    = next_->next;
            return *this;
        }

        constexpr iterator operator++(int) noexcept
        {
            auto res = *this;
            ++(*this);
            return res;
        }

        constexpr reference operator*() const noexcept
        {
            auto& res = current_->template get<T>();
            return res;
        }

        constexpr pointer operator->() const noexcept
        {
            auto* res = current_->template ptr<T>();
            return res;
        }
    };

    class sentinel
    {
    private:
        const ws::detail::link* end_{nullptr};

    public:
        constexpr sentinel() noexcept = default;

        explicit constexpr sentinel(const ws::detail::link* end) noexcept
            : end_{end}
        {}

        constexpr bool operator==(const sentinel& other) const noexcept
        {
            return end_ == other.end_;
        }

        constexpr bool operator!=(const sentinel& other) const noexcept
        {
            return end_ != other.end_;
        }

        constexpr bool operator==(const iterator& it) const noexcept
        {
            return end_ == it.current_;
        }

        constexpr bool operator!=(const iterator& it) const noexcept
        {
            return end_ != it.current_;
        }

        friend constexpr bool operator==(const iterator& it,
                                         const sentinel& sent) noexcept
        {
            return sent == it;
        }

        friend constexpr bool operator!=(const iterator& it,
                                         const sentinel& sent) noexcept
        {
            return sent != it;
        }
    };

private:
    ws::detail::link link_;

public:
    constexpr intrusive_list() noexcept : link_{&link_, &link_}
    {}

    constexpr iterator begin() noexcept
    {
        return iterator{link_.next};
    }

    constexpr sentinel end() const noexcept
    {
        return sentinel{&link_};
    }

    constexpr bool empty() const noexcept
    {
        // identical to wayland implementation wl_list_empty
        return link_.next == &link_;
    }

    constexpr size_type size() const noexcept
    {
        // identical to wayland implementation wl_list_len
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
        ws::detail::link* l = ws::detail::link::from(ref);

        // from here on identical to wayland (wl_list_insert)
        l->prev       = &link_;
        l->next       = link_.next;
        link_.next    = l;
        l->next->prev = l;
    }

    constexpr void insert_list(intrusive_list& other) noexcept
    {
        // identical to wl_list_insert_list

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