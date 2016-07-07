#ifndef _MPL_FILTER_H
#define _MPL_FILTER_H

#include <mpl/type_list.h>

namespace mpl {

    //Filtering the type list by some predicate
    //The predicate should contain boolean "value" static member.
    //Example:
    //    template <typename T>
    //    struct predicate {
    //        static const bool value = T::value > 1;
    //    };
    template <typename TypeList, template <typename> class Pred> struct filter;

    namespace details {
        template <typename Head, typename Tail, template <typename> class Pred, bool result>
        struct filter_helper {
            typedef type_list<Head, typename filter<Tail, Pred>::type> type;
        };

        template <typename Head, typename Tail, template <typename> class Pred>
        struct filter_helper<Head, Tail, Pred, false> {
            typedef typename filter<Tail, Pred>::type type;
        };
    }

    template <typename Head, typename Tail, template <typename> class Pred>
    struct filter<type_list<Head, Tail>, Pred> {
      typedef typename details::filter_helper<Head, Tail, Pred, Pred<Head>::value>::type type;
    };

    template <template <typename> class Pred>
    struct filter<null_type, Pred> {
      typedef null_type type;
    };
}

#endif //_MPL_FILTER_H
