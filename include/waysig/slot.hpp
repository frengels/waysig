#pragma once

#include "waysig/detail/slot_base.hpp"
#include "waysig/detail/util.hpp"

namespace ws
{
namespace detail
{
class slot_access;
}

template<typename Sig>
class slot;

template<typename Res, typename... Args>
class slot<Res(Args...)> : protected ws::detail::slot_base<Res>
{
    friend ws::detail::slot_access;

public:
    using result_type = Res;

public:
    template<typename F>
    constexpr slot(F fn) noexcept(std::is_nothrow_move_constructible_v<F>)
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
            "F's signature is required to be (ws::slot<result_type(Args...)*, "
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
        new (this) F(std::move(fn));
    }
};
} // namespace ws
