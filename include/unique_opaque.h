#ifndef CZU_UNIQUE_OPAQUE_HPP
#define CZU_UNIQUE_OPAQUE_HPP
#include <memory>

/**
    Helper to define a `std::unique_ptr` that works just with a forward
    declaration

    The "regular" `std::unique_ptr<T>` requires the full definition of `T` to be
    available, as it has to emit calls to `delete` in every TU that may use it.

    A workaround to this problem is to have a `std::unique_ptr` with a custom
    deleter, which is defined in a TU that knows the full definition of `T`.

    This header standardizes and generalizes this trick. The usage is quite
    simple:

    - everywhere you would have used `std::unique_ptr<T>`, use
      `czu::unique_opaque<T>`; it will work just fine with `T` being a forward
      declaration;
    - in a TU that knows the full definition of `T`, at top level invoke the
      macro `CZU_DEFINE_OPAQUE_DELETER`; it will define the custom deleter used
      by `czu::unique_opaque<T>`
*/

namespace utils {
template<typename T>
struct opaque_deleter {
    void operator()(T *it) {
        void opaque_deleter_hook(T *);
        opaque_deleter_hook(it);
    }
};

template<typename T>
using unique_opaque = std::unique_ptr<T, opaque_deleter<T>>;
}

/// Call at top level in a C++ file to enable type %T to be used in an %unique_opaque<T>
#define CZU_DEFINE_OPAQUE_DELETER(T) namespace utils { void opaque_deleter_hook(T *it) { delete it; } }

#endif