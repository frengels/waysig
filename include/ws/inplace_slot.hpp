#pragma once

#include "ws/slot.hpp"
#include "ws/storage/local.hpp"

namespace ws
{
template<typename Signature, std::size_t Len = sizeof(void*)>
class inplace_slot;

template<typename Res, typename... Args, std::size_t Len>
class inplace_slot<Res(Args...), Len> : public ws::slot<Res(Args...)>,
                                        protected ws::storage::local<Len>
{
public:
    template<typename F>
    constexpr inplace_slot(F&& fn) noexcept(
        std::is_nothrow_constructible_v<std::decay_t<F>, F>)
        : ws::slot<Res(Args...)>{[](ws::slot<Res(Args...)>& slot,
                                    Args... args) {
              auto& this_ = *static_cast<ws::inplace_slot<Res(Args...), Len>*>(
                  std::addressof(slot));

              auto& func = this_.template get<std::decay_t<F>>();
              return std::invoke(func, this_, std::forward<Args>(args)...);
          }}
    {
        static_assert(std::is_trivially_copyable_v<std::decay_t<F>>,
                      "F must be trivially copyable for inplace_slot");
        this->template emplace<std::decay_t<F>>(std::forward<F>(fn));
    }
};
} // namespace ws