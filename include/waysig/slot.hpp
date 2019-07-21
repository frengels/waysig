#pragma once

#include "waysig/detail/slot_base.hpp"
#include "waysig/detail/unaligned_storage.hpp"
#include "waysig/detail/util.hpp"
#include "waysig/sigslot_access.hpp"

namespace ws
{
template<typename Sig, std::size_t Buff = 0>
class slot;

// this is a basic slot, which can take the specified args. There is only room
// for a single function pointer.
template<typename Ret, typename... Args, std::size_t Buff>
class slot<Ret(Args...), Buff> : protected ws::detail::slot_base<Ret>,
                                 private ws::detail::unaligned_storage<Buff>
{
    friend ws::sigslot_access;

public:
    using result_type = Ret;

public:
    template<typename F>
    constexpr slot(F fn) noexcept(std::is_nothrow_move_constructible_v<F>)
        : ws::detail::slot_base<result_type>{
              [](ws::detail::slot_base<result_type>* self, void* data) {
                  auto* this_   = static_cast<slot<Ret(Args...), Buff>*>(self);
                  F&    functor = this_->template get<F>();

                  // unpacked args automatically handles conversion from void*
                  auto args = ws::detail::unpacked_args<Args...>{data};
                  return args.apply([&](Args... args) {
                      return std::invoke(
                          functor, this_, std::forward<Args>(args)...);
                  });
              }}
    {
        static_assert(std::is_invocable_r_v<result_type,
                                            F,
                                            slot<result_type(Args...), Buff>*,
                                            Args...>,
                      "Cannot call F with the arguments "
                      "(slot<result_type(Args...), Buff>*, Args...)");
        static_assert(std::is_trivially_destructible_v<F>,
                      "F is not trivially destructible");
        this->template emplace<F>(std::move(fn));
    }
};
} // namespace ws