#ifndef __MPL_VAR_H
#define __MPL_VAR_H

#include <mpl/returns.h>

namespace mpl {

	//Metafunction that wraps some type and returns itself
	template <typename Name>
	struct var : returns< var<Name> >
	{
	};
}

#endif //__MPL_VAR_H