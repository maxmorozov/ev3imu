#ifndef __MPL_SORT_H
#define __MPL_SORT_H

#include <mpl/type_list.h>
#include <mpl/remove.h>
#include <mpl/min_max.h>

namespace mpl {
	// Selection sort of type list
	// Comparator is a metafunction that have internal member "value" of integer type that 
    // has negative value if the first metafunction argumemnt is smaller than the second.
    // If the arguments are equal the metafunction should return zero value, and positive
    // value if the left argument is greater than the second.
	//
	// Example:
	// template <typename Left, typename Right>
	// struct compare {
	//     static const int value = Left::value - Right::value;
	// };

	template <typename TypeList, template <typename, typename> class Comparator> struct sort;

	//Specialization for the empty list
	template <template <typename, typename> class Comparator>
	struct sort<null_type, Comparator>
	{
		typedef null_type type;
	};

	template <typename Head, typename Tail, template <typename, typename> class Comparator>
	struct sort<type_list<Head, Tail>, Comparator> {
	private:
		typedef type_list<Head, Tail> sequence;
		typedef typename min_element<sequence, Comparator>::type minimum;
		typedef typename remove<sequence, minimum>::type remainder;
	public:
		typedef type_list<minimum, typename sort<remainder, Comparator>::type> type;
	};

}

#endif //__MPL_SORT_H
