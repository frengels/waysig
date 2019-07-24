#pragma once

#include <memory>

#include "ws/detail/slot_base.hpp"
#include "ws/detail/util.hpp"

/// calculates the offset of mem_name to subtract from the address of ref, which
/// can then be casted to a reference of type.
/// Yes I know this is technically undefined behavior, but I'm pretty sure
/// there's no compiler which treats this code wrong.
/// using offsetof is avoided to avoid warnings from -Winvalid-offsetof which
/// usually don't mean much anyway.
#define WS_CONTAINER_OF(ref, type, mem_name)                                   \
    *reinterpret_cast<type*>(                                                  \
        reinterpret_cast<std::byte*>(std::addressof(ref)) -                    \
        reinterpret_cast<std::byte*>(                                          \
            std::addressof(reinterpret_cast<type*>(0)->mem_name)))

namespace ws
{
namespace detail
{
class slot_access;
}

template<typename Sig>
class slot;

template<typename Res, typename... Args>
class slot<Res(Args...)> : private ws::detail::slot_base<Res>
{
    friend ws::detail::slot_access;

public:
    using result_type = Res;

public:
    template<typename F>
    constexpr slot(F&& fn) noexcept(std::is_nothrow_move_constructible_v<F>)
        : ws::detail::slot_base<result_type>{
              [](ws::detail::slot_base<result_type>* self, void* data) {
                  auto& this_ =
                      *static_cast<ws::slot<result_type(Args...)>*>(self);
                  // this prevents reinterpret_cast, don't know how sound it is.
                  // can be replaced by default constructing in c++20
                  auto* v       = static_cast<void*>(std::addressof(this_));
                  F&    functor = *static_cast<F*>(v);

                  auto args = ws::detail::unpacked_args<Args...>{data};
                  return args.apply([&](Args... args) {
                      return std::invoke(
                          functor, this_, std::forward<Args>(args)...);
                  });
              }}
    {
        static_assert(
            std::is_invocable_r_v<result_type,
                                  F&,
                                  ws::slot<result_type(Args...)>&,
                                  Args...>,
            "F's signature is required to be (ws::slot<result_type(Args...)>&, "
            "Args...) -> result_type");
        static_assert(std::is_empty_v<F>, "F must be empty for this slot type");
        // to be trivially copyable the type must not contain virtual functions
        // or bases, and the type itself and all its bases have implicitly
        // defined or defaulted copy/move construct/assign.
        // This causes me confusion as a lambda is trivially copyable but move
        // and copy assign are deleted, so maybe it shouldn't be
        // trivially_copyable?
        // Either way this is solved for stateless lambdas in c++20 where they
        // are default constructible (would remove need for trivially_copyable
        // trait). Additionally copy/move assign will be defaulted for these.
        // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0624r0.pdf
        static_assert(std::is_trivially_copyable_v<F>,
                      "F must be trivially copyable");

        // because of being an empty type I assume this is well defined.
        new (this) std::decay_t<F>(std::forward<F>(fn));
    }

    constexpr result_type operator()(Args... args) noexcept
    {
        auto pack_args =
            ws::detail::make_packaged_args(std::forward<Args>(args)...);
        auto& base = *static_cast<ws::detail::slot_base<result_type>*>(this);
        return base(pack_args.void_ptr());
    }

    constexpr bool connected() const noexcept
    {
        return static_cast<const ws::detail::slot_base<result_type>*>(this)
            ->connected();
    }

    constexpr void disconnect() noexcept
    {
        static_cast<ws::detail::slot_base<result_type>*>(this)->disconnect();
    }
};
} // namespace ws
