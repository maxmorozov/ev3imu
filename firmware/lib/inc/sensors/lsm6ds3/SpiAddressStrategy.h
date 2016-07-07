#ifndef __LSM6DS3_SPI_ADDRESS_STRATEGY_H
#define __LSM6DS3_SPI_ADDRESS_STRATEGY_H

namespace sensors {
namespace lsm6ds3 {

    //This class sets multi read/write but of the address 
    //in case of multiple byte request
	class SpiAddressStrategy {
    public:

        //Selects 7 bit of the address
        static uint8_t normalize(uint8_t address) {
            return address & 0x7F;
        }

        //Auto increment bit is not needed 
        static uint8_t auto_increment(uint8_t count) {
			return 0;
        }

	};

}
}


#endif //__LSM6DS3_SPI_ADDRESS_STRATEGY_H
