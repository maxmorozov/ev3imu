#ifndef __LSM6DS3_GYROSCOPE_H
#define __LSM6DS3_GYROSCOPE_H

#include <stddef.h>
#include <sensors/spi_transport.h>
#include <sensors/vector.h>
#include <sensors/lsm6ds3/ImuBase.h>
#include <sensors/traits/scale_count.h>

namespace sensors {
namespace lsm6ds3 {

    //IMU driver for LSM6DS3
    //TODO Consider adding ByteOrder template parameter to configure platform-specific byte order
    template <typename Transport>
	class Gyroscope : public ImuBase {
    private:
        struct Bitmasks {
            //CTRL1_XL, CTRL2_G
            static const uint8_t ODR = 0xF0;
            //CTRL2_G
            static const uint8_t Scale = 0x0E;
            //CTRL3_C
            static const uint8_t Endiannes = 0x02;

            //CTRL10_C
            static const uint8_t Axis = 0x38;
            //INT1_CTRL, INT2_CTRL
            static const uint8_t DataReadyInterrupt = 0x02;
            //STATUS_REG
            static const uint8_t DataAvailable = 0x02;
            static const uint8_t TempDataAvailable = 0x04;
            static const uint8_t BootRunning = 0x08;
        };

        struct Bitfields {
            //CTRL1_XL, CTRL2_G
            static const uint8_t PowerDown = 0; //Initial value of CTRL1_XL, CTRL2_G
            //INT2_CTRL
            static const uint8_t DataReadyInterrupt = 0x02;
            //CTRL4_C
            static const uint8_t DataReadyMask = 0x08;
            //CTRL3_C
            static const uint8_t Reset = 0x01;
        };

    public:
        // GyroScale defines all possible FSR's of the gyroscope:
        enum Scale
        {
            SCALE_125DPS  = 0x04,   // 125 degrees per second
            SCALE_245DPS  = 0x00,   // 245 dps
            SCALE_500DPS  = 0x01,   // 500 dps
            SCALE_1000DPS = 0x02,   // 1000 dps
            SCALE_2000DPS = 0x03    // 2000 dps
        };

        // AccelODR defines all possible output data rates of the gyroscope:
        enum ODR
        {
            ODR_POWER_DOWN  = 0x00,
            ODR_13Hz 		= 0x10,
            ODR_26Hz 		= 0x20,
            ODR_52Hz 		= 0x30,
            ODR_104Hz 		= 0x40,
            ODR_208Hz 		= 0x50,
            ODR_416Hz 		= 0x60,
            ODR_833Hz 		= 0x70,
            ODR_1660Hz 	 	= 0x80
        };

        enum DataReadyInterrupt {
            InterruptDisabled,
            InterruptEnabled = Bitfields::DataReadyInterrupt
        };

    private:
        Transport transport;

        //Converts gyro scale enumeration to scale configuration in the control register
        uint8_t scaleToBitMask(Scale scale) {
            if (scale == SCALE_125DPS)
                return 2;
            else
                return scale << 2;
        }

    public:
        static const uint8_t DEVICE_ID = 0x69;

        //Returns the device identifier
        uint8_t getId() const {
            return transport.readByte(Registers::WHO_AM_I);   // Read the WHO_AM_I register
        }

        //Checks if the device works correctly
        bool checkDevice() const {
            return getId() == DEVICE_ID;
        }

        // Initialize the gyroscope
        // This will set up the scale and output rate of the sensor.
        // It'll also "turn on" the sensor and every axis of the sensor.
        // Input:
        //	- scale = The scale of the gyroscope. Should be a Scale value.
        //	- odr = Output data rate of the gyroscope. ORD value.
        //  - dataReadyInterrupt = generate an interrupt when gyro data is ready
        void init(Scale scale, ODR odr, DataReadyInterrupt dataReadyInterrupt)
        {
            //INT2_CTRL - set gyro data ready interrupt bit
            //CTRL4_C - set DRDY_MASK to 1 to skip invalid samples
            //CTRL2_G - set ODR and scale
            transport.writeByte(Registers::CTRL4_C, Bitfields::DataReadyMask);
            transport.writeByte(Registers::INT2_CTRL, dataReadyInterrupt);
            transport.writeByte(Registers::CTRL2_G, uint8_t(odr) | scaleToBitMask(scale));
        }

        //Returns the sensor to initial state
        void reset() {
            transport.writeByte(Registers::CTRL2_G, Bitfields::PowerDown);
            transport.writeByte(Registers::INT2_CTRL, 0);
        }

        void restart() {
            transport.writeByte(Registers::CTRL3_C, Bitfields::Reset);
        }

        //Turns off the gyroscope
        //Note: if interrupts have been enabled, disableDataReadyInterrupt should be called
        //before next initialization to make sure that interrupts will be activated
        void powerDown() {
            transport.writeByte(Registers::CTRL2_G, Bitfields::PowerDown);
        }

        //Updates the gyroscope's output data rate
        void setODR(ODR odr) {
            transport.writeByteWithMask(Registers::CTRL2_G, odr, Bitmasks::ODR);
        }

        //Updates the gyroscope full scale range
        void setScale(Scale scale) {
            transport.writeByteWithMask(Registers::CTRL2_G, scaleToBitMask(scale), Bitmasks::Scale);
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

        //Enable interrupt generation on INT1_A pin when the sample is ready
        void enableDataReadyInterrupt() {
            transport.setBits(Registers::INT2_CTRL, Bitfields::DataReadyInterrupt);
        }

        //Disable interrupt generation on INT1_A pin when the sample is ready
        void disableDataReadyInterrupt() {
            transport.clearBits(Registers::INT2_CTRL, Bitfields::DataReadyInterrupt);
        }

        //Returns the value of the first control register
        uint8_t getConfig() const {
            return transport.readByte(Registers::CTRL2_G);
        }

        void getConfig(uint8_t* data, uint8_t size) const {
            transport.readBytes(Registers::CTRL2_G, data, size);
        }

        //Returns the status reister value
        uint8_t getStatus() const {
            return transport.readByte(Registers::STATUS_REG);
        }

        //Returns true if  there is an unread data sample
        bool isNewDataAvailable() const {
            return (getStatus() & Bitmasks::DataAvailable) != 0;
        }

        //Reads all axis data as a 3D vector from gyroscope
        void read(Vector& out) const {
            uint8_t temp[6]; // We'll read six bytes from the gyroscope into temp
            transport.readBytes(Registers::OUTX_L_G, temp, sizeof(temp));
            out.assign(temp);
        }

        //Reads sensor's memory starting from OUTX_L_G address
        void readSample(uint8_t* out, size_t size) const {
            transport.readBytes(Registers::OUTX_L_G, out, size);
        }

        //Reads the sample from the sensor
        //The length parameter keeps the sample size in elements
        void readSample(uint16_t* out, size_t length) const {
            transport.readBytes(Registers::OUTX_L_G, (int8_t*)out, length * sizeof(*out));
        }
	};
}

namespace traits {
	template <typename Transport>
        struct scale_count<lsm6ds3::Gyroscope<Transport> > {
		static const unsigned int value = 5;
	};
}

}


#endif //__LSM6DS3_GYROSCOPE_H