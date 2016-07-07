#ifndef __MPL_RETURNS_H
#define __MPL_RETURNS_H

namespace mpl {

	//This metafunciton returns its argument
	template <typename T>
	struct returns
	{
		typedef T type;
	};
}

#endif //__MPL_RETURNS_H