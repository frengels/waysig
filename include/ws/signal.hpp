#pragma once

#include "ws/detail/config.hpp"

#include "ws/listener.hpp"

namespace ws
{
class signal_base : public ws::detail::signal
{
public:
    constexpr signal_base() : ws::detail::signal{}
    {
        this->listener_list.prev = &this->listener_list;
        this->listener_list.next = &this->listener_list;
    }

    signal_base(const signal_base&) = delete;
    signal_base& operator=(const signal_base&) = delete;

    constexpr signal_base(signal_base&& other) noexcept
        : ws::detail::signal{.listener_list = other.listener_list}
    {
        other.listener_list.prev->next = &this->listener_list;
        other.listener_list.next->prev = &this->listener_list;
    }

    constexpr signal_base& operator=(signal_base&& other) noexcept
    {
        this->listener_list.prev = other.listener_list.prev;
        other.listener_list.prev = nullptr;

        this->listener_list.next = other.listener_list.next;
        other.listener_list.next = nullptr;

        this->listener_list.prev->next = &this->listener_list;
        this->listener_list.next->prev = &this->listener_list;

        return *this;
    }

#ifdef WAYSIG_ENABLE_WL
    constexpr void add_wl(wl_listener& l)
    {
        add_raw(l);
    }
#endif

protected:
    constexpr void add_raw(ws::detail::listener& l)
    {
        auto* last = this->listener_list.prev;

        l.link.prev      = last;
        l.link.next      = last->next;
        last->next       = &l.link;
        last->next->prev = &l.link;
    }

    constexpr void emit_raw(void* p) noexcept
    {}
};

template<typename T>
class signal : public signal_base
{
    static_assert(
        std::is_void_v<T> || std::is_reference_v<T> ||
            sizeof(T) <= sizeof(void*),
        "Argument type must be a reference or less than the size of a pointer");

public:
    using value_type = T;

public:
    void emit(T arg) noexcept
    {
        static_assert(!std::is_same_v<T, void>,
                      "void means no argument is passed, use emit() instead");
        if constexpr (std::is_reference_v<T>)
        {
            wl_signal_emit(this, static_cast<void*>(std::addressof(arg)));
        }
        else
        {
            wl_signal_emit(this, reinterpret_cast<void*>(arg));
        }
    }

    void operator()(T arg) noexcept
    {
        emit(static_cast<T&&>(arg));
    }

    void add(listener<T>& l)
    {
        add_raw(l);
    }
};

template<>
class signal<void> : public signal_base
{

public:
    void emit() noexcept
    {
        wl_signal_emit(this, nullptr);
    }

    void operator()() noexcept
    {
        emit();
    }

    void add(listener<void>& l)
    {
        add_raw(l);
    }
};
} // namespace ws
