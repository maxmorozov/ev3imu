#ifndef __LSM9DS0_SPI_ADDRESS_STRATEGY_H
#define __LSM9DS0_SPI_ADDRESS_STRATEGY_H

namespace sensors {
namespace lsm9ds0 {

    //This class sets multi read/write but of the address
    //in case of multiple byte request
	class SpiAddressStrategy {
        static const uint8_t AUTO_INCREMENT_MASK = 0x40;
    public:

        //Selects 6 bit of the address
        static uint8_t normalize(uint8_t address) {
            return address & 0x3F;
        }

        //For reading multiple bytes we need to set 6 bit to 1
        //If the target size greater than 1 we will use
        //auto-increment of address to read or write multiple bytes at
        //one transaction
        static uint8_t auto_increment(uint8_t count) {
            // If single write, bit 6 should be 0
            return count > 1 ? AUTO_INCREMENT_MASK : 0;
        }

	};

}
}


#endif //__LSM9DS0_SPI_ADDRESS_STRATEGY_H
