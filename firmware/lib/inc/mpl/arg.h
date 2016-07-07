#ifndef __MPL_ARG_H
#define __MPL_ARG_H

#include <mpl/returns.h>

namespace mpl {
    template <int i>
    struct arg : returns< arg<i> > {
		static const int value = i;
	};

	typedef arg<1> _1;
	typedef arg<2> _2;
	typedef arg<3> _3;
	typedef arg<4> _4;
	typedef arg<5> _5;

}

#endif //__MPL_ARG_H
