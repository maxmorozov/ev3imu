#ifndef __SENSORS_VECTOR_H
#define __SENSORS_VECTOR_H

namespace sensors {
    struct Vector {
        int16_t x;
        int16_t y;
        int16_t z;

		void assign(uint8_t* bytes) {
            x = (bytes[1] << 8) | bytes[0]; // Store x-axis values into x
            y = (bytes[3] << 8) | bytes[2]; // Store y-axis values into y
            z = (bytes[5] << 8) | bytes[4]; // Store z-axis values into z
		}

        //STM8 uses big-endian architecture so we need to configure the device
        //to use big-endian encoding in order to use this method
		void assign(uint16_t* data) {
            x = data[0]; // Store x-axis values into x
            y = data[1]; // Store y-axis values into y
            z = data[2]; // Store z-axis values into z
		}

    };
}

#endif //__SENSORS_VECTOR_H
