#pragma once

#ifndef NDEBUG
#include <cassert>
#endif

#include "ws/detail/config.hpp"

#include "ws/listener.hpp"

#define WS_CONTAINER_OF(ref, ty, mem)                                          \
    const_cast<ty&>(*reinterpret_cast<const volatile ty*>(                     \
        reinterpret_cast<const volatile char*>(__builtin_addressof(ref)) -     \
        __builtin_offsetof(ty, mem)))

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

#ifndef NDEBUG
    ~signal_base()
    {
        // make sure that all connected listeners have disconnected
        assert(this->listener_list.prev == &this->listener_list);
        assert(this->listener_list.next == &this->listener_list);
    }
#endif

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

        l.link.prev       = last;
        l.link.next       = last->next;
        last->next        = &l.link;
        l.link.next->prev = &l.link;
    }

    void emit_raw(void* p) noexcept
    {
        ws::detail::list* current = this->listener_list.next;
        ws::detail::list* next    = current->next;

        ws::detail::list* end = &this->listener_list;

        while (current != end)
        {
            auto& listen =
                WS_CONTAINER_OF(*current, ws::detail::listener, link);
            listen.notify(&listen, p);
            current = next;
            next    = current->next;
        }
    }
};

template<typename T>
class signal : public signal_base
{
    static_assert(
        std::is_reference<T>::value || sizeof(T) <= sizeof(void*),
        "Argument type must be a reference or less than the size of a pointer");

public:
    using value_type = T;

public:
    void emit(T arg) noexcept
    {
        emit_raw(detail::voidify<T>(static_cast<T&&>(arg)));
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
        emit_raw(nullptr);
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
