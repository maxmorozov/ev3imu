#ifndef __MPL_VECTOR_C_H
#define __MPL_VECTOR_C_H

#include <limits.h>
#include <mpl/type_list.h>
#include <mpl/integral_c.h>
#include <mpl/if.h>

namespace mpl {
    namespace details {
        //Checks if the specified constant is maximal long value that means the empty value
        template <long N>
        struct is_max_long {
            static const bool value = N == LONG_MAX;
        };

        //Converts the numeric constant into inetgral_c type
        template <typename T, long N>
        struct int_ : if_<is_max_long<N>, null_type, integral_c<T, T(N)> > {
        };
    }

	template<
	      typename T, long C0 = LONG_MAX, long C1 = LONG_MAX, long C2 = LONG_MAX
	    , long C3 = LONG_MAX, long C4 = LONG_MAX, long C5 = LONG_MAX
	    , long C6 = LONG_MAX, long C7 = LONG_MAX, long C8 = LONG_MAX
	    , long C9 = LONG_MAX, long C10 = LONG_MAX, long C11 = LONG_MAX
	    , long C12 = LONG_MAX, long C13 = LONG_MAX, long C14 = LONG_MAX
	    , long C15 = LONG_MAX, long C16 = LONG_MAX, long C17 = LONG_MAX
	    , long C18 = LONG_MAX, long C19 = LONG_MAX
	    >
    struct vector_c : make_type_list<
            typename details::int_<T, C0>::type, typename details::int_<T, C1>::type,
            typename details::int_<T, C2>::type, typename details::int_<T, C3>::type,
            typename details::int_<T, C4>::type, typename details::int_<T, C5>::type,
            typename details::int_<T, C6>::type, typename details::int_<T, C7>::type,
            typename details::int_<T, C8>::type, typename details::int_<T, C9>::type,
            typename details::int_<T, C10>::type, typename details::int_<T, C11>::type,
            typename details::int_<T, C12>::type, typename details::int_<T, C13>::type,
            typename details::int_<T, C14>::type, typename details::int_<T, C15>::type,
            typename details::int_<T, C16>::type, typename details::int_<T, C17>::type,
            typename details::int_<T, C18>::type, typename details::int_<T, C19>::type
        >
    {
    };
}



#endif //__MPL_VECTOR_C_H