#pragma once

#define WAYSIG_ENABLE_WL

#ifdef WAYSIG_ENABLE_WL
#include <wayland-server-core.h>
#endif

namespace ws
{
namespace detail
{
#if !defined(WAYSIG_ENABLE_WL)
struct listener;

using notify_func_t = void (*)(listener*, void*);

struct list
{
    list* prev;
    list* next;
};

struct listener
{
    list          link;
    notify_func_t notify;
};

struct signal
{
    list listener_list;
};
#else
using notify_func_t = wl_notify_func_t;

using list = wl_list;

using listener = wl_listener;

using signal = wl_signal;
#endif
} // namespace detail
} // namespace ws
