#ifndef __UTILS_SELECT_SIZE_H
#define __UTILS_SELECT_SIZE_H

#include <mpl/eval_if.h>
#include <mpl/identity.h>
#include <stdint.h>

namespace utils {

	template<uint32_t size>
	struct select_size
	{
		static const bool less_or_eq8 = size <= 0xff;
		static const bool less_or_eq16 = size <= 0xffff;

		typedef typename mpl::eval_if_c<
				less_or_eq8,
                mpl::identity<uint8_t>,
				mpl::if_c<less_or_eq16, uint16_t, uint32_t>
		>::type type;
	};

}

#endif //SELECT_SIZE