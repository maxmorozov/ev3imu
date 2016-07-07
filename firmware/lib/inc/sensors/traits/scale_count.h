#ifndef __SENSORS_TRAITS_SCALE_COUNT_H
#define __SENSORS_TRAITS_SCALE_COUNT_H

namespace sensors {
namespace traits {

    //Number of different full scale ranges,
    //supported by sensor
	template <typename Sensor>
	struct scale_count {
		static const unsigned int value = 1;
	};

}}


#endif //__SENSORS_TRAITS_SCALE_COUNT_H