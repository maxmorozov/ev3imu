#ifndef __MPL_IDENTITY_H
#define __MPL_IDENTITY_H

namespace mpl
{
    template <typename T>
    struct identity
    {
        typedef T type;
    };

    template <typename T>
    struct make_identity
    {
        typedef identity<T> type;
    };
}

#endif//__MPL_IDENTITY_H