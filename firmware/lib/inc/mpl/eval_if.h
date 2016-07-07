#ifndef __MPL_EVAL_IF_H
#define __MPL_EVAL_IF_H

#include <mpl/if.h>

namespace mpl {

    //Evaluates one metafunction according the condition expression
    //Allows to not instantiate not selected type
	template<typename Condition, typename F1, typename F2>
	struct eval_if
	    : if_<Condition, F1, F2>::type
	{
	    
	};

	// (almost) copy & paste in order to save one more
	// recursively nested template instantiation to user
	template<bool Condition, typename F1, typename F2>
	struct eval_if_c
	    : if_c<Condition, F1, F2>::type
	{
	};
}

#endif //__MPL_EVAL_IF_H