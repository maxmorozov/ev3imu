#ifndef __MPL_LAMBDA_H
#define __MPL_LAMBDA_H

#include <mpl/let.h>

namespace mpl {
    template <typename Arg, typename F>
    struct lambda
    {
        template <typename T>
        struct apply : let<Arg, T, F>::type
        {
        };
    };
}

#endif //__MPL_LAMBDA_H