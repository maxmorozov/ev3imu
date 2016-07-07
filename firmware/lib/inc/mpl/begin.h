#ifndef __MPL_LENGTH_H
#define __MPL_LENGTH_H

#include <mpl/type_list.h>

namespace mpl {
    //Extracts the first element of the type list
    template <typename TypeList> struct begin;

    // the normal case : the head element of the list (it's a type)
    template <typename Head, typename Tail>
    struct begin<type_list<Head, Tail> > {
		typedef Head type;
    };

    // the terminal case : our typelist is the empty list
    template <>
    struct begin<null_type> {
        typedef null_type type;
    };

}

#endif //__MPL_LENGTH_H
