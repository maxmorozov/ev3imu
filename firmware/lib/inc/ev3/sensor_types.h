#ifndef __EV3_SENSOR_TYPES_H
#define __EV3_SENSOR_TYPES_H

namespace ev3 {

	//The data type 0= 8bits, 1=16 bits, 2=32 bits, 3=float
    enum DataType {
    	Int8 = 0,
    	Int16,
		Int32,
		Float
	};

}


#endif //__EV3_SENSOR_TYPES_H
