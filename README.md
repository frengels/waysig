# Waysig
signal/slot implementation based on wl_signal/listener

# Example

A very simple example which shows the basic usage.

```cpp
#include <ws/ws.hpp>

ws::signal<void()> sig;

ws::slot<void()> slot{[](ws::slot<void()>& this_slot) {
    std::cout << "slot invoked!\n";
    this_slot.disconnect();
}};

sig.connect(slot);

sig(); // outputs: "slot invoked!"
sig(); // no output, slot disconnected

// directly invoke a slot
slot(); // outputs: "slot invoked!"
```

Now follows a more complete example which shows how it will typically be used in a class.

```cpp
#include <ws/ws.hpp>

// notifies other classes that i changed
class notifier {
    int i{};
public:
    ws::signal<void(int)> i_changed;

    void set_i(int new_i) {
        i = new_i;
        i_changed(i);
    }
};

// this class listens for changes and reflects the change in itself
class listener {
public:
    int i{};

    ws::slot<void(int)> i_change{[](auto& self, int new_i) {
        // the following is a trick to get the surrounding object
        // from the address of the slot.
        // this is technically undefined behavior so below I show
        // a no ub version.
        listener& this_ = WS_CONTAINER_OF(self, listener, i_change);
        this_.i = new_i;
    }};

    ws::inplace_slot<void(int), sizeof(listener*)> i_change_safe{
        [this](auto& self, int new_i) {
            // inplace_slot created a buffer large enough to store
            // the required state for this lambda.
            this->i = new_i;
        }};
};

notifier n;

listener l0;
listener l1;

n.i_changed.connect(l0);
n.i_changed.connect(l1);

n.set_i(42);
assert(l0.i == 42);
assert(l1.i == 42);
```

# Wayland interoperability

One of the main features of this library is its seamless interop with wayland's `wl_signal` and `wl_listener`. This is done by having the base objects `ws::detail::signal_base<void>` and `ws::detail::slot_base<void>` be ABI compatible with it's C library counterparts. On top of this base are `ws::slot` and `ws::signal` which give us C++'s nice type safety features.

To connect wayland and waysig signal/slot with each other you can include the header `<ws/ext/wl.hpp>` which provide free functions for connecting and emitting signals and slot of both types amongst each other (`ws::connect` and `ws::emit`).