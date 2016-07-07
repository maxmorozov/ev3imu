#ifndef __MPL_IS_SAME_H
#define __MPL_IS_SAME_H

namespace mpl {

    template <typename T, typename U>
    struct is_same
    {
        static const bool value = false;
    };
    
    template <typename T>
    struct is_same<T, T>
    {
        static const bool value = true;
    };

}

#endif  //__MPL_IS_SAME_H