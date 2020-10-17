#pragma once

#include <type_traits>
#include <utility>

#include "ws/detail/config.hpp"

namespace ws
{
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
        std::is_reference_v<T> || sizeof(T) <= sizeof(void*),
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
        void* void_ptr = [&]() noexcept->void*
        {
            if constexpr (std::is_reference_v<T>)
            {
                auto* p = std::addressof(arg);
                return static_cast<void*>(p);
            }
            else if constexpr (std::is_pointer_v<T>)
            {
                return static_cast<void*>(arg);
            }
            else
            {
                return reinterpret_cast<void*>(arg);
            }
        }
        ();

        this->notify(this, void_ptr);
    }

    constexpr void operator()(T arg) noexcept
    {
        invoke(static_cast<T&&>(arg));
    }

    template<typename F>
    constexpr void set_notify(F fn) noexcept
    {
        static_assert(std::is_empty_v<F>, "F may not carry state");
        static_assert(
            std::is_trivially_destructible_v<F>,
            "F must be trivially destructible as no destructor will be run");

        static_assert(std::is_invocable_r_v<void, F, listener<T>&, T>,
                      "F must be invocable with a listener and data");

        set_notify_raw([](ws::detail::listener* l, void* data_ptr) {
            T data = [&]() -> T {
                if constexpr (std::is_reference_v<T>)
                {
                    using ptr_type =
                        std::add_pointer_t<std::remove_reference_t<T>>;
                    auto* p = static_cast<ptr_type>(data_ptr);
                    return *p;
                }
                else if constexpr (std::is_pointer_v<T>)
                {
                    return static_cast<T>(data_ptr);
                }
                else
                {
                    return reinterpret_cast<T>(data_ptr);
                }
            }();

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
        static_assert(std::is_empty_v<F>, "F may not carry state");
        static_assert(
            std::is_trivially_destructible_v<F>,
            "F must be trivially destructible as no destructor will be run");
        static_assert(std::is_invocable_r_v<void, F, listener<void>&>,
                      "F must be invocable with a listener");

        set_notify_raw([](ws::detail::listener* l, void*) {
            listener<void>& self = *static_cast<listener<void>*>(l);

            F& fn = reinterpret_cast<F&>(self);
            fn(self);
        });
    }
};
} // namespace ws
