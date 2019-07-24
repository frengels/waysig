#pragma once

#include <type_traits>

namespace ws
{
namespace storage
{
template<std::size_t Len>
class local
{
private:
    std::aligned_storage_t<Len> storage_;

public:
    constexpr local() noexcept = default;

    template<typename T, typename... Args>
    constexpr void emplace(Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
    {
        static_assert(sizeof(T) <= Len, "T does not fit in local storage");
        new (&storage_) T(std::forward<Args>(args)...);
    }

    template<typename T>
    T& get() noexcept
    {
        static_assert(sizeof(T) <= Len,
                      "T is not in this local storage because it wouldn't fit");
        return *reinterpret_cast<T*>(&storage_);
    }

    template<typename T>
    const T& get() const noexcept
    {
        static_assert(sizeof(T) <= Len,
                      "T is not in this local storage because it wouldn't fit");
        return *reinterpret_cast<const T*>(&storage_);
    }
};
} // namespace storage
} // namespace ws