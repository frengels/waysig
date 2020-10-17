#pragma once

#include <type_traits>
#include <utility>

#include "ws/detail/config.hpp"

namespace ws
{
namespace detail
{
template<typename T>
struct voidify_t
{
    void* operator()(T t) const noexcept
    {
        static_assert(sizeof(T) <= sizeof(void*), "Cannot convert T to void*");
        return const_cast<void*>(reinterpret_cast<const void*>(t));
    }
};

template<typename T>
struct voidify_t<T*>
{
    void* operator()(T* p) const noexcept
    {
        return const_cast<void*>(static_cast<const void*>(p));
    }
};

template<typename T>
struct voidify_t<T&>
{
    void* operator()(T& ref) const noexcept
    {
        return const_cast<void*>(static_cast<const void*>(std::addressof(ref)));
    }
};

template<typename T>
struct voidify_t<T&&>
{
    void* operator()(T&& ref) const noexcept
    {
        return const_cast<void*>(static_cast<const void*>(std::addressof(ref)));
    }
};

template<typename T>
void* voidify(T arg) noexcept
{
    return voidify_t<T>{}(static_cast<T&&>(arg));
}

template<typename T>
struct devoidify_t
{
    T operator()(void* p) const noexcept
    {
        static_assert(sizeof(T) <= sizeof(void*), "Cannot convert void* to T");
        return reinterpret_cast<T>(p);
    }
};

template<typename T>
struct devoidify_t<T*>
{
    T* operator()(void* p) const noexcept
    {
        return static_cast<T*>(p);
    }
};

template<typename T>
struct devoidify_t<T&>
{
    T& operator()(void* p) const noexcept
    {
        return *static_cast<T*>(p);
    }
};

template<typename T>
struct devoidify_t<T&&>
{
    T&& operator()(void* p) const noexcept
    {
        return static_cast<T&&>(*static_cast<T*>(p));
    }
};

template<typename T>
T devoidify(void* p) noexcept
{
    return devoidify_t<T>{}(p);
}
} // namespace detail

class listener_base : public ws::detail::listener
{
public:
    listener_base() = default;

    listener_base(listener_base&& other) noexcept
        : ws::detail::listener{std::move(other)}
    {
        this->link.prev->next = &this->link;
        this->link.next->prev = &this->link;

        other.link.prev = nullptr;
        other.link.next = nullptr;
    }

    listener_base& operator=(listener_base&& other) noexcept
    {
        this->link.prev = std::exchange(other.link.prev, nullptr);
        this->link.next = std::exchange(other.link.next, nullptr);

        this->link.prev->next = &this->link;
        this->link.next->prev = &this->link;

        return *this;
    }

    ~listener_base()
    {
        if (is_connected())
        {
            remove();
        }
    }

    bool is_connected() const noexcept
    {
        return this->link.prev != nullptr;
    }

    void remove() noexcept
    {
        this->link.prev->next = this->link.next;
        this->link.next->prev = this->link.prev;

        this->link.next = nullptr;
        this->link.prev = nullptr;
    }

    constexpr void set_notify_raw(ws::detail::notify_func_t notify) noexcept
    {
        this->notify = notify;
    }
};

template<typename T>
class listener : public listener_base
{
    static_assert(
        std::is_reference<T>::value || sizeof(T) <= sizeof(void*),
        "Argument type must be a reference or less than the size of a pointer");

public:
    using value_type = T;

public:
    using listener_base::listener_base;

    template<typename F>
    listener(F fn) noexcept
    {
        set_notify(fn);
    }

    constexpr void invoke(T arg) noexcept
    {
        void* void_ptr = detail::voidify<T>(static_cast<T&&>(arg));

        this->notify(this, void_ptr);
    }

    constexpr void operator()(T arg) noexcept
    {
        invoke(static_cast<T&&>(arg));
    }

    template<typename F>
    constexpr void set_notify(F fn) noexcept
    {
        static_assert(std::is_empty<F>::value, "F may not carry state");
        static_assert(
            std::is_trivially_destructible<F>::value,
            "F must be trivially destructible as no destructor will be run");

        /*
        static_assert(std::is_invocable_r<void, F, listener<T>&, T>::value,
                      "F must be invocable with a listener and data");
        */

        set_notify_raw([](ws::detail::listener* l, void* data_ptr) {
            T data = detail::devoidify<T>(data_ptr);

            listener<T>& self = *static_cast<listener<T>*>(l);

            // very UB but no reason for this not to work
            F& fn = reinterpret_cast<F&>(self);
            fn(self, data);
        });
    }
};

template<>
class listener<void> : public listener_base
{
public:
    using listener_base::listener_base;

    template<typename F>
    listener(F fn) noexcept
    {
        set_notify(fn);
    }

    constexpr void invoke() noexcept
    {
        this->notify(this, nullptr);
    }

    constexpr void operator()() noexcept
    {
        invoke();
    }

    template<typename F>
    constexpr void set_notify(F fn) noexcept
    {
        static_assert(std::is_empty<F>::value, "F may not carry state");
        static_assert(
            std::is_trivially_destructible<F>::value,
            "F must be trivially destructible as no destructor will be run");
        /*
        static_assert(std::is_invocable_r<void, F, listener<void>&>::value,
                      "F must be invocable with a listener");
        */

        set_notify_raw([](ws::detail::listener* l, void*) {
            listener<void>& self = *static_cast<listener<void>*>(l);

            F& fn = reinterpret_cast<F&>(self);
            fn(self);
        });
    }
};
} // namespace ws
