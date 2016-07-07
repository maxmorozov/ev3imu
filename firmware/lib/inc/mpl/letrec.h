#ifndef __MPL_LETREC_H
#define __MPL_LETREC_H

#include <mpl/let.h>

namespace mpl {

    //Forward declaration
    //Recursive version of let
    template <typename A, typename E1, typename E2> struct letrec;

    template <typename A, typename E1, typename E2>
    struct letrec : let<A, letrec<A, E1, E1>, E2> 
    {
    };

}

#endif //__MPL_LETREC_H