#pragma once

#include "waysig/detail/signal_base.hpp"
#include "waysig/detail/util.hpp"
#include "waysig/slot.hpp"

namespace ws
{
template<typename Signature>
class signal;

template<typename Ret, typename... Args>
class signal<Ret(Args...)> : private ws::detail::signal_base<Ret>
{
public:
    using result_type = Ret;

public:
    constexpr void emit(Args... args) noexcept
    {
        // this creates an instance of packaged_args.
        // packaged_args automatically handles required conversions
        auto package =
            ws::detail::make_packaged_args(std::forward<Args>(args)...);

        auto& sig_base =
            *static_cast<ws::detail::signal_base<result_type>*>(this);
        sig_base.emit(package.void_ptr());
    }

    template<std::size_t Buff>
    void connect(ws::slot<result_type(Args...), Buff>& s) noexcept
    {
        auto& base_sig =
            *static_cast<ws::detail::signal_base<result_type>*>(this);
        base_sig.connect(s);
    }

    constexpr void operator()(Args... args) noexcept
    {
        emit(std::forward<Args>(args)...);
    }
};
} // namespace ws