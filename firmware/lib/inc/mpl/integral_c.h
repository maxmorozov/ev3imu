#ifndef __MPL_INTEGRAL_C_H
#define __MPL_INTEGRAL_C_H

namespace mpl {
	template< typename T, T N > struct integral_c {
		static const T value = N;

	    typedef integral_c type;
	    typedef T value_type;
	};
}

#endif //__MPL_INTEGRAL_C_H
