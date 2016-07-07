#ifndef __MPL_LET_H
#define __MPL_LET_H

#include <mpl/returns.h>

namespace mpl {

    //Forward declaration
    //let replaces all ocurrences of A expression in E2 with E1
    template <typename A, typename E1, typename E2> struct let;

    namespace details {

        template <typename A, typename E1, typename E2>
        struct let_impl : returns<E2>
        {
        };

        //This specialization prevents substitutions in the inner let expressions
        template <typename A, typename E1, typename E1a, typename E2>
        struct let_impl<A, E1, let<A, E1a, E2> >: returns<let<A, E1a, E2> >
        {
        };

        template <
            typename A,
            typename E1,
            template <typename> class F,
            class T1
        >
        struct let_impl<A, E1, F<T1> > : returns< F<typename let<A, E1, T1>::type> >
        {
        };

        template <
            typename A,
            typename E1,
            template <typename, typename> class F,
            typename T1,
            typename T2
        >
        struct let_impl<A, E1, F<T1, T2> > : returns<
            F<
                typename let<A, E1, T1>::type,
                typename let<A, E1, T2>::type
            >
         >
        {
        };

        template <
            typename A,
            typename E1,
            template <typename, typename, typename> class F,
            typename T1,
            typename T2,
            typename T3
        >
        struct let_impl<A, E1, F<T1, T2, T3> > : returns<
            F<
                typename let<A, E1, T1>::type,
                typename let<A, E1, T2>::type,
                typename let<A, E1, T3>::type
            >
         >
        {
        };

        template <
            typename A,
            typename E1,
            template <typename, typename, typename, typename> class F,
            typename T1,
            typename T2,
            typename T3,
            typename T4
        >
        struct let_impl<A, E1, F<T1, T2, T3, T4> > : returns<
            F<
                typename let<A, E1, T1>::type,
                typename let<A, E1, T2>::type,
                typename let<A, E1, T3>::type,
                typename let<A, E1, T4>::type
            >
         >
        {
        };

        template <
            typename A,
            typename E1,
            template <typename, typename, typename, typename, typename> class F,
            typename T1,
            typename T2,
            typename T3,
            typename T4,
            typename T5
        >
        struct let_impl<A, E1, F<T1, T2, T3, T4, T5> > : returns<
            F<
                typename let<A, E1, T1>::type,
                typename let<A, E1, T2>::type,
                typename let<A, E1, T3>::type,
                typename let<A, E1, T4>::type,
                typename let<A, E1, T5>::type
            >
         >
        {
        };

        template <
            typename A,
            typename E1,
            template <typename, typename, typename, typename, typename, typename> class F,
            typename T1,
            typename T2,
            typename T3,
            typename T4,
            typename T5,
            typename T6
        >
        struct let_impl<A, E1, F<T1, T2, T3, T4, T5, T6> > : returns<
            F<
                typename let<A, E1, T1>::type,
                typename let<A, E1, T2>::type,
                typename let<A, E1, T3>::type,
                typename let<A, E1, T4>::type,
                typename let<A, E1, T5>::type,
                typename let<A, E1, T6>::type
            >
         >
        {
        };

        template <
            typename A,
            typename E1,
            template <typename, typename, typename, typename, typename, typename, typename> class F,
            typename T1,
            typename T2,
            typename T3,
            typename T4,
            typename T5,
            typename T6,
            typename T7
        >
        struct let_impl<A, E1, F<T1, T2, T3, T4, T5, T6, T7> > : returns<
            F<
                typename let<A, E1, T1>::type,
                typename let<A, E1, T2>::type,
                typename let<A, E1, T3>::type,
                typename let<A, E1, T4>::type,
                typename let<A, E1, T5>::type,
                typename let<A, E1, T6>::type,
                typename let<A, E1, T7>::type
            >
         >
        {
        };

        template <
            typename A,
            typename E1,
            template <typename, typename, typename, typename, typename, typename, typename, typename> class F,
            typename T1,
            typename T2,
            typename T3,
            typename T4,
            typename T5,
            typename T6,
            typename T7,
            typename T8
        >
        struct let_impl<A, E1, F<T1, T2, T3, T4, T5, T6, T7, T8> > : returns<
            F<
                typename let<A, E1, T1>::type,
                typename let<A, E1, T2>::type,
                typename let<A, E1, T3>::type,
                typename let<A, E1, T4>::type,
                typename let<A, E1, T5>::type,
                typename let<A, E1, T6>::type,
                typename let<A, E1, T7>::type,
                typename let<A, E1, T8>::type
            >
         >
        {
        };

        template <
            typename A,
            typename E1,
            template <typename, typename, typename, typename, typename, typename, typename, typename, typename> class F,
            typename T1,
            typename T2,
            typename T3,
            typename T4,
            typename T5,
            typename T6,
            typename T7,
            typename T8,
            typename T9
        >
        struct let_impl<A, E1, F<T1, T2, T3, T4, T5, T6, T7, T8, T9> > : returns<
            F<
                typename let<A, E1, T1>::type,
                typename let<A, E1, T2>::type,
                typename let<A, E1, T3>::type,
                typename let<A, E1, T4>::type,
                typename let<A, E1, T5>::type,
                typename let<A, E1, T6>::type,
                typename let<A, E1, T7>::type,
                typename let<A, E1, T8>::type,
                typename let<A, E1, T9>::type
            >
         >
        {
        };

        template <
            typename A,
            typename E1,
            template <typename, typename, typename, typename, typename, typename, typename, typename, typename, typename> class F,
            typename T1,
            typename T2,
            typename T3,
            typename T4,
            typename T5,
            typename T6,
            typename T7,
            typename T8,
            typename T9,
            typename T10
        >
        struct let_impl<A, E1, F<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> > : returns<
            F<
                typename let<A, E1, T1>::type,
                typename let<A, E1, T2>::type,
                typename let<A, E1, T3>::type,
                typename let<A, E1, T4>::type,
                typename let<A, E1, T5>::type,
                typename let<A, E1, T6>::type,
                typename let<A, E1, T7>::type,
                typename let<A, E1, T8>::type,
                typename let<A, E1, T9>::type,
                typename let<A, E1, T10>::type
            >
         >
        {
        };

    }

    template <typename A, typename E1, typename E2>
    struct let : details::let_impl<A, E1, E2>
    {
    };

    template <typename A, typename E1>
    struct let<A, E1, A> : returns<E1>
    {
    };

}

#endif //__MPL_LET_H