#ifndef __MPL_MAP_H
#define __MPL_MAP_H

#include <mpl/type_list.h>

namespace mpl {
    //Applies some metafunction to each element of type list
    //Func template should export "type" member type that is a transformed type
    //Example:
    //   template <typename T>
    //   struct make_pointer {
    //       typedef T* type;
    //   };
    template <typename TypeList, template <typename> class Func> struct map;

    template <typename Head, typename Tail, template <typename> class Func>
    struct map<type_list<Head, Tail>, Func> {
        typedef type_list<typename Func<Head>::type, typename map<Tail, Func>::type> type;
    };

    template <template <typename> class Func>
    struct map<null_type, Func>
    {
        typedef null_type type;
    };

}

#endif //__MPL_TYPE_LIST_H
