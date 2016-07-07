#ifndef __MPL_REMOVE_H
#define __MPL_REMOVE_H

#include <mpl/type_list.h>

namespace mpl {
	//Removes the first occurence of the type from the typelist
	template <class TypeList, class Type> struct remove;

	template <class Type>                         // Specialization 1
	struct remove<null_type, Type>
	{
		typedef null_type type;
	};

	template <class Type, class Tail>             // Specialization 2
	struct remove<type_list<Type, Tail>, Type>
	{
		typedef Tail type;
	};

	template <class Head, class Tail, class Type> // Specialization 3
	struct remove<type_list<Head, Tail>, Type>
	{
		typedef type_list<Head, typename remove<Tail, Type>::type> type;
	};


}

#endif //__MPL_REMOVE_H
