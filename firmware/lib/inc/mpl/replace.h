#ifndef __MPL_REPLACE_H
#define __MPL_REPLACE_H

#include <mpl/type_list.h>

namespace mpl {
	// Replaces the first occurence of a type in a typelist, with another type
	template <typename TypeList, typename Type, typename Replacement> struct replace;

	template <class Type, class Replacement>
	struct replace<null_type, Type, Replacement>
	{
		typedef null_type type;
	};

	template <class Type, class Tail, class Replacement>
	struct replace<type_list<Type, Tail>, Type, Replacement>
	{
		typedef type_list<Replacement, Tail> type;
	};

	template <class Head, class Tail, class Type, class Replacement>
	struct replace<type_list<Head, Tail>, Type, Replacement>
	{
		typedef type_list<Head, typename replace<Tail, Type, Replacement>::type> type;
	};

}

#endif //__MPL_REPLACE_H
