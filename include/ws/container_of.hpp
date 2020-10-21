#pragma once

#include <type_traits>

namespace ws
{
namespace detail
{
template<typename T>
struct is_basic_type : std::true_type
{};

template<typename T>
struct is_basic_type<T*> : std::false_type
{};

template<typename T>
struct is_basic_type<T&> : std::false_type
{};

template<typename T>
struct is_basic_type<T&&> : std::false_type
{};

template<typename T>
struct is_basic_type<const T> : std::false_type
{};

template<typename T>
struct is_basic_type<volatile T> : std::false_type
{};

template<typename T>
struct is_basic_type<const volatile T> : std::false_type
{};

template<typename T>
struct remove_cvref
{
    using type =
        typename std::remove_cv<typename std::remove_reference<T>::type>::type;
};

template<typename T>
struct apply_cvref
{
    template<typename U>
    using invoke = typename remove_cvref<U>::type;
};

template<typename T>
struct apply_cvref<const T>
{
    template<typename U>
    using invoke = const typename remove_cvref<U>::type;
};

template<typename T>
struct apply_cvref<volatile T>
{
    template<typename U>
    using invoke = volatile typename remove_cvref<U>::type;
};

template<typename T>
struct apply_cvref<const volatile T>
{
    template<typename U>
    using invoke = const volatile typename remove_cvref<U>::type;
};

template<typename T>
struct apply_cvref<T&>
{
    template<typename U>
    using invoke = typename apply_cvref<T>::template invoke<U>&;
};

template<typename T>
struct apply_cvref<T&&>
{
    template<typename U>
    using invoke = typename apply_cvref<T>::template invoke<U>&&;
};
} // namespace detail
} // namespace ws

#define WS_CONTAINER_OF(ref, ty, mem)                                          \
    [&]() -> decltype(auto) {                                                  \
        static_assert(::ws::detail::is_basic_type<ty>::value,                  \
                      "ty cannot have any qualifiers or be of pointer type");  \
        using _fwd_type = typename ::ws::detail::apply_cvref<decltype(         \
            ref)>::template invoke<ty>;                                        \
        return static_cast<_fwd_type>(                                         \
            const_cast<ty&>(*reinterpret_cast<const volatile ty*>(             \
                reinterpret_cast<const volatile char*>(                        \
                    __builtin_addressof(ref)) -                                \
                __builtin_offsetof(ty, mem))));                                \
    }()
