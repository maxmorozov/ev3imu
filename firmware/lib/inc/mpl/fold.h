#ifndef __MPL_FOLD_H
#define __MPL_FOLD_H

#include <mpl/type_list.h>

namespace mpl {
    //Folding the list starting from the left side
    template <typename TypeList, typename State, template <typename State_, typename Item> class Func> struct fold;

    template <typename Head, typename Tail, typename State, template <typename, typename> class Func>
    struct fold<type_list<Head, Tail>, State, Func> {
        typedef typename fold<Tail, typename Func<State, Head>::type, Func>::type type;
    };

    template <typename State, template <typename, typename> class Func>
    struct fold<null_type, State, Func>
    {
        typedef State type;
    };

	//Folding the list starting from the right side
	template <typename TypeList, typename State, template <typename State_, typename Item> class Func> struct foldr;

	template <typename Head, typename Tail, typename State, template <typename, typename> class Func>
	struct foldr<type_list<Head, Tail>, State, Func> {
		typedef typename Func<typename foldr<Tail, State, Func>::type, Head>::type type;
	};

	template <typename State, template <typename, typename> class Func>
	struct foldr<null_type, State, Func>
	{
		typedef State type;
	};

}

#endif //__MPL_FOLD_H
