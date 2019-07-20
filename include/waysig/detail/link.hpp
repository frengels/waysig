#pragma once

#include <cstddef>
#include <type_traits>

namespace ws
{
namespace detail
{
template<typename T>
struct link_offset : std::integral_constant<std::size_t, T::link_offset>
{};

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

    ~link()
    {
        remove();
    }

    constexpr void remove() noexcept
    {
        // identical to wayland implementation
        prev->next = next;
        next->prev = prev;
        next       = nullptr;
        prev       = nullptr;
    }

    template<typename T>
    T* ptr() noexcept
    {
        auto* bytes_repr = reinterpret_cast<std::byte*>(this);
        bytes_repr -= ws::detail::link_offset<T>::value;
        T* ptr = reinterpret_cast<T*>(bytes_repr);
        return ptr;
    }

    template<typename T>
    T& get() noexcept
    {
        return *ptr<T>();
    }

    template<typename T>
    static ws::detail::link* from(T& ref) noexcept
    {
        auto* bytes_repr = reinterpret_cast<std::byte*>(std::addressof(ref));
        bytes_repr +=
            ws::detail::link_offset<T>::value; // move to the link position
        ws::detail::link* l = reinterpret_cast<ws::detail::link*>(bytes_repr);
        return l;
    }
};
} // namespace detail
} // namespace ws