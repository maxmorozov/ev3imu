#ifndef __MPL_ENABLE_IF_H
#define __MPL_ENABLE_IF_H

namespace mpl {
    template <bool B, typename T = void>
    struct enable_if_c {
        typedef T type;
    };

    template <typename T>
    struct enable_if_c<false, T> {
    };

    template <typename Cond, typename T = void>
    struct enable_if : public enable_if_c<Cond::value, T> {
    };
}

#endif //__MPL_ENABLE_IF_H