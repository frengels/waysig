#pragma once

#include <wayland-server-core.h>

#include "ws/detail/signal_base.hpp"
#include "ws/detail/slot_access.hpp"
#include "ws/detail/util.hpp"
#include "ws/slot.hpp"

#include "ws/listener.hpp"

namespace ws
{
namespace detail
{
class signal_access;
}

class signal_base : public wl_signal
{
public:
    signal_base() : wl_signal{}
    {
        this->listener_list.prev = &this->listener_list;
        this->listener_list.next = &this->listener_list;
    }

    signal_base(const signal_base&) = delete;
    signal_base& operator=(const signal_base&) = delete;

    signal_base(signal_base&& other) noexcept
        : wl_signal{.listener_list = other.listener_list}
    {
        other.listener_list.prev->next = &this->listener_list;
        other.listener_list.next->prev = &this->listener_list;
    }

    signal_base& operator=(signal_base&& other) noexcept
    {
        this->listener_list.prev =
            std::exchange(other.listener_list.prev, nullptr);
        this->listener_list.next =
            std::exchange(other.listener_list.next, nullptr);

        this->listener_list.prev->next = &this->listener_list;
        this->listener_list.next->prev = &this->listener_list;

        return *this;
    }

    void add_raw(wl_listener& l)
    {
        wl_signal_add(this, &l);
    }
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

template<typename Signature>
class signal;

template<typename Ret, typename... Args>
class signal<Ret(Args...)> : private ws::detail::signal_base<Ret>
{
    friend ws::detail::signal_access;

public:
    using result_type = Ret;

public:
    constexpr void emit(Args... args) noexcept
    {
        // this creates an instance of packaged_args.
        // packaged_args automatically handles required conversions
        auto package =
            ws::detail::make_packaged_args(std::forward<Args>(args)...);

        auto& sig_base =
            *static_cast<ws::detail::signal_base<result_type>*>(this);
        sig_base.emit(package.void_ptr());
    }

    template<typename OutputIt>
    constexpr void emit(OutputIt it, Args... args) noexcept
    {
        auto package =
            ws::detail::make_packaged_args(std::forward<Args>(args)...);

        auto& sig_base =
            *static_cast<ws::detail::signal_base<result_type>*>(this);

        sig_base.emit(std::move(it), package.void_ptr());
    }

    void connect(ws::slot<result_type(Args...)>& s) noexcept
    {
        auto& base_slot = ws::detail::slot_access::base(s);
        auto& base_sig =
            *static_cast<ws::detail::signal_base<result_type>*>(this);
        base_sig.connect(base_slot);
    }

    constexpr void operator()(Args... args) noexcept
    {
        emit(std::forward<Args>(args)...);
    }

    template<typename OutputIt>
    constexpr void operator()(OutputIt it, Args... args) noexcept
    {
        emit(std::move(it), std::forward<Args>(args)...);
    }
};

template<typename Res, typename... Args>
constexpr void emit(ws::signal<Res(Args...)>& sig, Args&&... args) noexcept
{
    sig(std::forward<Args>(args)...);
}

template<typename Res, typename... Args>
void connect(ws::signal<Res(Args...)>& sig,
             ws::slot<Res(Args...)>&   slot) noexcept
{
    sig.connect(slot);
}
} // namespace ws
