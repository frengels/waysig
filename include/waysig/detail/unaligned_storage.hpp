#pragma once

#include <utility>

namespace ws
{
namespace detail
{
// simple implementation
template<std::size_t Len>
class unaligned_storage
{
private:
    std::byte storage_[Len];

public:
    constexpr unaligned_storage() noexcept = default;

    template<typename T, typename... Args>
    T& emplace(Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
    {
        static_assert(sizeof(storage_) >= sizeof(T),
                      "T does not fit within this unaligned_storage");
        new (static_cast<void*>(storage_)) T(std::forward<Args>(args)...);
        return get<T>();
    }

    template<typename T>
    T& get() noexcept
    {
        static_assert(sizeof(storage_) >= sizeof(T),
                      "T does not fit within this unaligned_storage");
        return *reinterpret_cast<T*>(storage_);
    }

    template<typename T>
    const T& get() const noexcept
    {
        static_assert(sizeof(storage_) >= sizeof(T),
                      "T does not fit within this unaligned_storage");
        return *reinterpret_cast<const T*>(storage_);
    }
};

template<>
class unaligned_storage<0>
{
public:
    constexpr unaligned_storage() noexcept = default;

    template<typename T, typename... Args>
    T& emplace(Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
    {
        static_assert(std::is_empty_v<T>,
                      "unaligned_storage<0> cannot hold a non empty object");
        // we still have to actually invoke the constructor for potential side
        // effects
        new (static_cast<void*>(this)) T(std::forward<Args>(args)...);
        return get<T>();
    }

    template<typename T>
    T& get() noexcept
    {
        static_assert(std::is_empty_v<T>,
                      "unaligned_storage<0> cannot hold a non empty object");
        return *reinterpret_cast<T*>(this);
    }

    template<typename T>
    const T& get() const noexcept
    {
        static_assert(std::is_empty_v<T>,
                      "unaligned_storage<0> cannot hold a non empty object");
        return *reinterpret_cast<const T*>(this);
    }
};
} // namespace detail
} // namespace ws