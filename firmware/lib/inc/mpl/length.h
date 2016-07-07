#ifndef __MPL_LENGTH_H
#define __MPL_LENGTH_H

#include <mpl/type_list.h>

namespace mpl {
    //Calculates the type list length
    template <typename TypeList> struct length;

    // the normal case : the head element of the list (it's a type), and the tail, which is itself a type list
    template <typename Head, typename Tail>
    struct length<type_list<Head, Tail> > {
        static const int value = 1 + length<Tail>::value ;
    };

    // the terminal case : our typelist is the empty list, we're at the end of the list, so we won't add 1 neither we will go on with the recursion
    template <>
    struct length<null_type> {
        static const int value = 0 ;
    };

}

#endif //__MPL_LENGTH_H
