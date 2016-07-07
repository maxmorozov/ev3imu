#ifndef __MPL_TYPE_LIST_H
#define __MPL_TYPE_LIST_H

namespace mpl {
    template <typename Head, typename Tail>
    struct type_list
    {
        typedef Head head;
        typedef Tail tail;
    };

	// class null_type
	// Used as a placeholder for "no type here"
	// Useful as an end marker in typelists
    struct null_type { };

    //Helper to create a type list
    template <
        typename T1 = null_type, typename T2 = null_type, typename T3 = null_type,
        typename T4 = null_type, typename T5 = null_type, typename T6 = null_type,
        typename T7 = null_type, typename T8 = null_type, typename T9 = null_type,
        typename T10 = null_type, typename T11 = null_type, typename T12 = null_type,
        typename T13 = null_type, typename T14 = null_type, typename T15 = null_type,
        typename T16 = null_type, typename T17 = null_type, typename T18 = null_type,
        typename T19 = null_type, typename T20 = null_type, typename T21 = null_type,
        typename T22 = null_type, typename T23 = null_type, typename T24 = null_type,
        typename T25 = null_type, typename T26 = null_type, typename T27 = null_type,
        typename T28 = null_type, typename T29 = null_type, typename T30 = null_type,
        typename T31 = null_type, typename T32 = null_type, typename T33 = null_type,
        typename T34 = null_type, typename T35 = null_type, typename T36 = null_type
    >
    struct make_type_list {
    private:
        typedef typename make_type_list<
            T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, 
            T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33, T34, T35, T36
        >::type tail_type;

    public:
        typedef type_list<T1, tail_type> type;
    };

    template<>
    struct make_type_list<> {
        typedef null_type type;
    };

}

#endif //__MPL_TYPE_LIST_H
