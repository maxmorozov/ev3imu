#ifndef __MPL_GROUP_BY_H
#define __MPL_GROUP_BY_H

#include <mpl/type_list.h>
#include <mpl/fold.h>
#include <mpl/if.h>


namespace mpl {

	//Generating a type list that contains sub-lists of types with the same ID
	template <typename TypeList, template <typename, typename> class Comparator>
	struct group_by
	{
		//typedef type_list<null_type, null_type> state;
		typedef null_type state;

		template <typename State, typename Item> struct group_func;

		template <typename Item>
		struct group_func<null_type, Item>
		{
			typedef type_list<type_list<Item, null_type>, null_type> type;
		};

		template <typename Head, typename Tail, typename Item>
		struct group_func<type_list<Head, Tail>, Item>
		{
			static const bool equals = Comparator<typename Head::head, Item>::value == 0;
			typedef typename if_c<
				equals, 
				type_list<type_list<Item, Head>, Tail>,
				type_list<type_list<Item, null_type>, type_list<Head, Tail> > >::type type;
		};

		typedef typename foldr<TypeList, state, group_func>::type type;
	};

}

#endif //__MPL_GROUP_BY_H
