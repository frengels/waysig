#pragma once

#include <functional>
#include <tuple>

namespace ws
{
namespace detail
{
// Take a bunch of arguments and convert them to the most optimal void*
// representable object. This involves either creating a forwarded tuple, simply
// casting a pointer, taking the address of a reference or passing nullptr in
// case of no args.
template<typename... Args>
class packaged_args
{
private:
    std::tuple<Args...> tup_args_;

public:
    constexpr packaged_args(Args... args) noexcept
        : tup_args_{std::forward<Args>(args)...}
    {}

    constexpr void* void_ptr() noexcept
    {
        return static_cast<void*>(std::addressof(tup_args_));
    }
};

template<typename T>
class packaged_args<T>
{
private:
    T val_;

public:
    constexpr packaged_args(T val) noexcept : val_{std::move(val)}
    {}

    constexpr void* void_ptr() noexcept
    {
        return static_cast<void*>(std::addressof(val_));
    }
};

template<typename T>
class packaged_args<T*>
{
private:
    T* ptr_;

public:
    constexpr packaged_args(T* ptr) noexcept : ptr_{ptr}
    {}

    constexpr void* void_ptr() noexcept
    {
        return static_cast<void*>(ptr_);
    }
};

template<typename T>
class packaged_args<T&>
{
private:
    T* ptr_;

public:
    constexpr packaged_args(T& ref) noexcept : ptr_{std::addressof(ref)}
    {}
    constexpr void* void_ptr() noexcept
    {
        return static_cast<void*>(ptr_);
    }
};

template<typename T>
class packaged_args<T&&>
{
private:
    T* ptr_;

public:
    constexpr packaged_args(T&& ref) noexcept : ptr_{std::addressof(ref)}
    {}

    constexpr void* void_ptr() noexcept
    {
        return static_cast<void*>(ptr_);
    }
};

template<>
class packaged_args<>
{
public:
    constexpr packaged_args() noexcept = default;

    constexpr void* void_ptr() noexcept
    {
        return nullptr;
    }
};

template<typename... Args>
constexpr packaged_args<Args...> make_packaged_args(Args&&... args) noexcept
{
    return packaged_args<Args...>{std::forward<Args>(args)...};
}

// take the above generated void* and reverse the process to get a reference or
// pointer to the actual tuple or similar.
template<typename... Args>
class unpacked_args
{
private:
    std::tuple<Args...>* tup_args_;

public:
    constexpr unpacked_args(void* data) noexcept
        : tup_args_{static_cast<std::tuple<Args...>*>(data)}
    {}
};

template<typename T>
class unpacked_args<T>
{
private:
    T* arg_;

public:
    constexpr unpacked_args(void* data) noexcept : arg_{static_cast<T*>(data)}
    {}

    template<typename F>
    constexpr decltype(auto)
    apply(F&& f) noexcept(std::is_nothrow_invocable_v<F, T>)
    {
        static_assert(std::is_invocable_v<F, T>, "Cannot call F with T");
        return std::invoke(std::forward<F>(f), std::move(*arg_));
    }
};

template<typename T>
class unpacked_args<T&>
{
private:
    T* arg_;

public:
    constexpr unpacked_args(void* data) noexcept : arg_{static_cast<T*>(data)}
    {}

    template<typename F>
    constexpr decltype(auto)
    apply(F&& f) noexcept(std::is_nothrow_invocable_v<F, T&>)
    {
        static_assert(std::is_invocable_v<F, T&>, "Cannot call F with T&");
        return std::invoke(std::forward<F>(f), *arg_);
    }
};

template<typename T>
class unpacked_args<T&&>
{
private:
    T* arg_;

public:
    constexpr unpacked_args(void* data) noexcept : arg_{static_cast<T*>(data)}
    {}

    template<typename F>
    constexpr decltype(auto)
    apply(F&& f) noexcept(std::is_nothrow_invocable_v<F, T&&>)
    {
        static_assert(std::is_invocable_v<F, T&&>, "Cannot call F with T&&");
        return std::invoke(std::forward<F>(f), std::move(*arg_));
    }
};

template<typename T>
class unpacked_args<T*>
{
private:
    T* arg_;

public:
    constexpr unpacked_args(void* data) noexcept : arg_{static_cast<T*>(data)}
    {}

    template<typename F>
    constexpr decltype(auto)
    apply(F&& f) noexcept(std::is_nothrow_invocable_v<F, T*>)
    {
        static_assert(std::is_invocable_v<F, T*>, "Cannot call F with T*");
        return std::invoke(std::forward<F>(f), arg_);
    }
};

template<>
class unpacked_args<>
{
public:
    constexpr unpacked_args(void*) noexcept
    {}

    template<typename F>
    constexpr decltype(auto)
    apply(F&& f) noexcept(std::is_nothrow_invocable_v<F>)
    {
        static_assert(std::is_invocable_v<F>, "Cannot call F");
        return std::invoke(std::forward<F>(f));
    }
};
} // namespace detail
} // namespace ws