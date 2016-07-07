#ifndef __LSM6DS3_ACCELEROMETER_H
#define __LSM6DS3_ACCELEROMETER_H

#include <stddef.h>
#include <sensors/spi_transport.h>
#include <sensors/vector.h>
#include <sensors/lsm6ds3/ImuBase.h>
#include <sensors/traits/scale_count.h>
#include <utils/byte_order.h>

namespace sensors {
namespace lsm6ds3 {

    //IMU driver for LSM6DS3
    //TODO Consider adding ByteOrder template parameter to configure platform-specific byte order
    template <typename Transport>
	class Accelerometer : public ImuBase {
    private:
        struct Bitmasks {
            //CTRL1_XL
            static const uint8_t ODR = 0xF0;
            //CTRL1_XL
            static const uint8_t Scale = 0x0C;
            //CTRL3_C
            static const uint8_t Endiannes = 0x02;
            //CTRL9_XL
            static const uint8_t Axis = 0x38;
            //INT1_CTRL, INT2_CTRL
            static const uint8_t DataReadyInterrupt = 0x01;
            //STATUS_REG
            static const uint8_t DataAvailable = 0x01;
            static const uint8_t TempDataAvailable = 0x04;
            static const uint8_t BootRunning = 0x08;
        };

        struct Bitfields {
            //CTRL1_XL
            static const uint8_t PowerDown = 0; //Initial value of CTRL1_XL, CTRL2_G
            //INT1_CTRL
            static const uint8_t DataReadyInterrupt = 0x01;
            //CTRL4_C
            static const uint8_t DataReadyMask = 0x08;
            //CTRL3_C
            static const uint8_t Reset = 0x01;
        };

    public:
        // Scale defines all possible FSR's of the accelerometer:
        enum Scale
        {
            SCALE_2G  = 0x00,	// 2g
            SCALE_4G  = 0x01,	// 4g
            SCALE_8G  = 0x02,	// 8g
            SCALE_16G = 0x03 	// 16g
        };

        // ODR defines all possible output data rates of the accelerometer:
        enum ODR
        {
            ODR_POWER_DOWN 	 = 0x00,
            ODR_13Hz 		 = 0x10,
            ODR_26Hz 		 = 0x20,
            ODR_52Hz 		 = 0x30,
            ODR_104Hz 		 = 0x40,
            ODR_208Hz 		 = 0x50,
            ODR_416Hz 		 = 0x60,
            ODR_833Hz 		 = 0x70,
            ODR_1660Hz 		 = 0x80,
            ODR_3330Hz 		 = 0x90,
            ODR_6660Hz 		 = 0xA0
        };

        //Accelerometer analog anti-alias filter bandwidth
        enum Bandwidth {
            BW_400Hz  = 0x00,
            BW_200Hz  = 0x01,
            BW_100Hz  = 0x02,
            BW_50Hz   = 0x03
        };

        enum DataReadyInterrupt {
            InterruptDisabled,
            InterruptEnabled = Bitfields::DataReadyInterrupt
        };

    private:
        Transport transport;

#if 0
        //Converts (0, 1, 2, 3) -> (0, 2, 3, 1) according the datasheet
        //We can use mapping via an array, but it prevents the compiler to
        //calculate constain expression in init method in compile time.
        //So the code size will be bigger.
        INLINE static uint8_t convertScale(uint8_t x) {
            //Propagating bit 1 as the number sign:
            //(0, 1, 2, 3) -> (0, 1, -2, -1)
            //and multiplying the number by 2:
            //(0, 1, 2, 3) -> (0, 2, -4, -2)=(00000000b, 00000010b,  11111100b, 11111110b)
            uint8_t c = ((x ^ 2) - 2) << 1;
            //(c < 0 ? ~c : c)
            //I use c >> 4 to get high nibble of the c.
            //if c < 0, the high nibble is 0xf, otherwise it is zero, so we can use XOR
            //instead of conditional bits inversion:
            //(00000000b, 00000010b,  11111100b, 11111110b) -> (0000b, 0010b, 0011b, 0001b)
            return (c ^ (c >> 4)) & 3;
        }

        //Converts accelerometer scale enumeration to scale configuration in the control register
        INLINE static uint8_t scaleToBitMask(Scale scale) {
            return convertScale(scale) << 2;
        }
#else
        //Converts accelerometer scale enumeration to scale configuration in the control register
        INLINE static uint8_t scaleToBitMask(Scale scale) {
            if (scale == SCALE_2G)
                return 0;
            else if (scale == SCALE_16G)
                return 1 << 2;
            else
                return (scale + 1) << 2;
        }
#endif

    public:
        //Checks if the device works correctly
        bool checkDevice() const {
            return true;
        }

        // Initialize the accelerometer.
        // This will set up the scale and output rate of the sensor.
        // It'll also "turn on" the sensor and every axis of the sensor.
        // Input:
        //	- scale = The scale of the accelerometer. Should be a Scale value.
        //	- odr = Output data rate of the accelerometer. ORD value.
        //  - dataReadyInterrupt = generate an interrupt when accelerometer's data is ready
        void init(Scale scale, ODR odr, DataReadyInterrupt dataReadyInterrupt)
        {
            //INT1_CTRL - set accel data ready interrupt bit
            //CTRL4_C - set DRDY_MASK to 1 to skip invalid samples
            //CTRL1_XL - set ODR and scale. Bandwith depends on ODR
            transport.writeByte(Registers::CTRL4_C, Bitfields::DataReadyMask);
            transport.writeByte(Registers::INT1_CTRL, dataReadyInterrupt);
            transport.writeByte(Registers::CTRL1_XL, uint8_t(odr) | scaleToBitMask(scale));
        }

        //Returns the sensor to initial state
        void reset() {
            transport.writeByte(Registers::CTRL1_XL, Bitfields::PowerDown);
            transport.writeByte(Registers::INT1_CTRL, 0);
        }

        void restart() {
            transport.writeByte(Registers::CTRL3_C, Bitfields::Reset);
        }

        //Turns off the accelerometer
        //Note: if interrupts have been enabled, disableDataReadyInterrupt should be called
        //before next initialization to make sure that interrupts will be activated
        void powerDown() {
            transport.writeByte(Registers::CTRL1_XL, Bitfields::PowerDown);
        }

        //Update the accelerometer's output data rate
        void setODR(ODR odr) {
            transport.writeByteWithMask(Registers::CTRL1_XL, odr, Bitmasks::ODR);
        }

        //Update the accelerometer's full scale range
        //Note: if this method is called from several places, it may be better to remove INLINE declaration
        INLINE void setScale(Scale scale) {
            transport.writeByteWithMask(Registers::CTRL1_XL, scaleToBitMask(scale), Bitmasks::Scale);
        }

        //Sets the appropriate byte ordeder in output samples
        void setEndiannes(utils::Endianness endianness) {
            transport.writeByteWithMask(Registers::CTRL3_C, endianness << 1, Bitmasks::Endiannes);
        }

        //Sets the appropriate byte ordeder in output samples
        void setBigEndian() {
            //Usage of setBist looks more reasonable, but produces bigger code,
            //because other methods that use setBits are not used in my project
            transport.writeByteWithMask(Registers::CTRL3_C, Bitmasks::Endiannes, Bitmasks::Endiannes);
        }

        //Enable interrupt generation on INT1 pin when the sample is ready
        void enableDataReadyInterrupt() {
            transport.setBits(Registers::INT1_CTRL, Bitfields::DataReadyInterrupt);
        }

        //Disable interrupt generation on INT1_A pin when the sample is ready
        void disableDataReadyInterrupt() {
            transport.clearBits(Registers::INT1_CTRL, Bitfields::DataReadyInterrupt);
        }

        //Returns the value of the first control register
        uint8_t getConfig() const {
            return transport.readByte(Registers::CTRL1_XL);
        }

        void getConfig(uint8_t* data, uint8_t size) const {
            transport.readBytes(Registers::CTRL1_XL, data, size);
        }

        //Returns the status reister value
        uint8_t getStatus() const {
            return transport.readByte(Registers::STATUS_REG);
        }

        //Returns true if  there is an unread data sample
        bool isNewDataAvailable() const {
            return (getStatus() & Bitmasks::DataAvailable) != 0;
        }

        //Reads all axis data as a 3D vector from accelerometer
        void read(Vector& out) const {
            uint8_t temp[6]; // We'll read six bytes from the accelerometer into temp
            transport.readBytes(Registers::OUTX_L_XL, temp, sizeof(temp));
            out.assign(temp);
        }

        //Reads sensor's memory starting from OUTX_L_XL address
        void readSample(uint8_t* out, size_t size) const {
            transport.readBytes(Registers::OUTX_L_XL, out, size);
        }

        //Reads the sample from the sensor
        //The length parameter keeps the sample size in elements
        void readSample(uint16_t* out, size_t length) const {
            transport.readBytes(Registers::OUTX_L_XL, (int8_t*)out, length * sizeof(*out));
        }
	};
}

namespace traits {
	template <typename Transport>
    struct scale_count<sensors::lsm6ds3::Accelerometer<Transport> > {
		static const unsigned int value = 4;
	};
}

}


#endif //__LSM6DS3_ACCELEROMETER_H