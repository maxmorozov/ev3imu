#ifndef __MPL_MIN_MAX_H
#define __MPL_MIN_MAX_H

#include <mpl/type_list.h>
#include <mpl/if.h>

namespace mpl {
	// Finding the smallest element of the list
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
	template <typename TypeList, template <typename, typename> class Comparator> struct min_element;

	template <typename Type, template <typename, typename> class Comparator>
	struct min_element<type_list<Type, null_type>, Comparator>
	{
		typedef Type type;
	};

	template <typename Head, typename Tail, template <typename, typename> class Comparator>
	struct min_element<type_list<Head, Tail>, Comparator> {
		typedef typename min_element<Tail, Comparator>::type Least;
		static const bool condition = Comparator<Head, Least>::value < 0;

		typedef typename if_c<condition, Head, Least>::type type;
	};

	// Finding the greatest element of the list
	template <typename TypeList, template <typename, typename> class Comparator> struct max_element;

	template <typename Type, template <typename, typename> class Comparator>
	struct max_element<type_list<Type, null_type>, Comparator>
	{
		typedef Type type;
	};

	template <typename Head, typename Tail, template <typename, typename> class Comparator>
	struct max_element<type_list<Head, Tail>, Comparator> {
		typedef typename max_element<Tail, Comparator>::type Greatest;
		static const bool condition = Comparator<Head, Greatest>::value > 0;

		typedef typename if_c<condition, Head, Greatest>::type type;
	};

}

#endif //__MPL_MIN_MAX_H
