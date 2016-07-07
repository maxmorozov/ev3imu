#ifndef __LSM330DLC_ACCELEROMETER_H
#define __LSM330DLC_ACCELEROMETER_H

#include <stddef.h>
#include <sensors/spi_transport.h>
#include <sensors/vector.h>
#include <utils/byte_order.h>
#include <sensors/traits/scale_count.h>

namespace sensors {
namespace lsm330 {

    //Accelerometer driver for lsm330dlc
    //TODO Consider adding ByteOrder template parameter to configure platform-specific byte order
    template <typename Transport>
    class Accelerometer {
    private:
        ///////////////////////////////////
        // LSM330DLC Accel Registers     //
        ///////////////////////////////////
        struct Registers {
            static const uint8_t CTRL_REG1 = 0x20;
            static const uint8_t CTRL_REG2 = 0x21;
            static const uint8_t CTRL_REG3 = 0x22;
            static const uint8_t CTRL_REG4 = 0x23;
            static const uint8_t CTRL_REG5 = 0x24;
            static const uint8_t CTRL_REG6 = 0x25;
            static const uint8_t REFERENCE = 0x26;
            static const uint8_t STATUS_REG = 0x27;
            static const uint8_t OUT_X_L = 0x28;
            static const uint8_t OUT_X_H = 0x29;
            static const uint8_t OUT_Y_L = 0x2A;
            static const uint8_t OUT_Y_H = 0x2B;
            static const uint8_t OUT_Z_L = 0x2C;
            static const uint8_t OUT_Z_H = 0x2D;
            static const uint8_t FIFO_CTRL_REG = 0x2E;
            static const uint8_t FIFO_SRC_REG = 0x2F ;
            static const uint8_t INT1_CFG = 0x30;
            static const uint8_t INT1_SOURCE = 0x31;
            static const uint8_t INT1_THS = 0x32;
            static const uint8_t INT1_DURATION = 0x33;
            static const uint8_t INT2_CFG = 0x34;
            static const uint8_t INT2_SOURCE = 0x35;
            static const uint8_t INT2_THS = 0x36;
            static const uint8_t INT2_DURATION = 0x37;
            static const uint8_t CLICK_CFG = 0x38;
            static const uint8_t CLICK_SRC = 0x39;
            static const uint8_t CLICK_THS = 0x3A;
            static const uint8_t TIME_LIMIT = 0x3B;
            static const uint8_t TIME_LATENCY = 0x3C;
            static const uint8_t TIME_WINDOW = 0x3D;
            static const uint8_t ACT_THS = 0x3E;
            static const uint8_t ACT_DUR = 0x3F;
        };

        struct Bitmasks {
            //CTRL_REG1
            static const uint8_t ODR  = 0xF0;
            static const uint8_t Axis = 0x07;
            //CTRL_REG4
            static const uint8_t Scale = 0x30;
            static const uint8_t Endiannes  = 0x40;
        };

        struct Bitfields {
            //CTRL_REG1
            static const uint8_t PowerDown = 0x07; //Initial value of CTRL_REG1
            //CTRL_REG4
            static const uint8_t LowPowerMode = 0x08;
            //CTRL_REG3
            static const uint8_t DataReadyInterrupt1 = 0x10;
            static const uint8_t DataReadyInterrupt2 = 0x08;
            static const uint8_t FifoWaterMarkInterrupt = 0x04;
            static const uint8_t FifoOverrunInterrupt = 0x02;
        };

    public:
        // Scale defines all possible FSR's of the accelerometer:
        enum Scale
        {
            SCALE_2G,	// 000:  2g
            SCALE_4G,	// 001:  4g
            SCALE_8G,	// 010:  8g
            SCALE_16G	// 011:  16g
        };

        // ODR defines all possible output data rates of the accelerometer:
        enum ODR
        {
            POWER_DOWN, 	// Power-down mode (0x0)
            ODR_1,		    // 1 Hz	(0x1)
            ODR_10,		    // 10 Hz (0x2)
            ODR_25,		    // 25 Hz (0x3)
            ODR_50,		    // 50 Hz (0x4)
            ODR_100,		// 100 Hz (0x5)
            ODR_200,		// 200 Hz (0x6)
            ODR_400,		// 400 Hz (0x7)
            ODR_LP_1620,		      // 1620 Hz Low Power Mode(0x8). Set only when low power mode is enabled
            ODR_NORMAL_1344_LP_5376 // 1344 Hz Normal Mode/ 5376 Low Power Mode(9)
        };

        enum DataReadyInterrupt {
            InterruptDisabled = 0,
            InterruptEnabled =  Bitfields::DataReadyInterrupt1
        };

    private:
        Transport transport;

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
        //  - dataReadyInterrupt = generate an interrupt when gyro data is ready
        //  - lowPowerMode = Accelerometer low power mode.
        void init(Scale scale, ODR odr, DataReadyInterrupt dataReadyInterrupt, bool lowPowerMode = false)
        {
            uint8_t powerMode = uint8_t(lowPowerMode) << 3;
            uint8_t data[4] = {
                    (odr << 4) | powerMode | 0x7,
                    0,
                    dataReadyInterrupt,
                    scale << 4};
            transport.writeBytes(Registers::CTRL_REG1, data, sizeof(data));
        }

        //Returns the sensor to initial state
        void reset() {
            uint8_t data[4] = {Bitfields::PowerDown, 0, 0, 0};
            transport.writeBytes(Registers::CTRL_REG1, data, sizeof(data));
        }

        //Turns off the accelerometer
        //Note: if interrupts have been enabled, disableDataReadyInterrupt should be called
        //before next initialization to make sure that interrupts will be activated
        void powerDown() {
            transport.writeByte(Registers::CTRL_REG1, Bitfields::PowerDown);
        }

        //Update the accelerometer's output data rate
        void setODR(ODR odr) {
            transport.writeByteWithMask(Registers::CTRL_REG1, odr << 4, Bitmasks::ODR);
        }

        //Update the accelerometer's full scale range
        void setScale(Scale scale) {
            transport.writeByteWithMask(Registers::CTRL_REG4, scale << 4, Bitmasks::Scale);
        }

        //Sets the appropriate byte ordeder in output samples
        void setEndiannes(utils::Endianness endianness) {
            transport.writeByteWithMask(Registers::CTRL_REG4, endianness << 6, Bitmasks::Endiannes);
        }

        //Sets the appropriate byte ordeder in output samples
        void setBigEndian() {
            //Usage of setBist looks more reasonable, but produces bigger code,
            //because other methods that use setBits are not used in my project
            transport.writeByteWithMask(Registers::CTRL_REG4, Bitmasks::Endiannes, Bitmasks::Endiannes);
        }

        //Turns on the low power mode
        void enableLowPowerMode() {
            transport.setBits(Registers::CTRL_REG1, Bitfields::LowPowerMode);
        }

        //Turns off the low power mode
        void disableLowPowerMode() {
            transport.clearBits(Registers::CTRL_REG1, Bitfields::LowPowerMode);
        }

        //Enable interrupt generation on INT1_A pin when the sample is ready
        void enableDataReadyInterrupt() {
            transport.setBits(Registers::CTRL_REG3, Bitfields::DataReadyInterrupt1);
        }

        //Disable interrupt generation on INT1_A pin when the sample is ready
        void disableDataReadyInterrupt() {
            transport.clearBits(Registers::CTRL_REG3, Bitfields::DataReadyInterrupt1);
        }

        //Returns the value of the first control register
        uint8_t getConfig() const {
            return transport.readByte(Registers::CTRL_REG1);
        }

        void getConfig(uint8_t* data, uint8_t size) const {
            transport.readBytes(Registers::CTRL_REG1, data, size);
        }

        //Returns the status reister value
        uint8_t getStatus() const {
            return transport.readByte(Registers::STATUS_REG);
        }

        //Returns true if any axis have a new data availavle to read
        bool isNewDataAvailable() const {
            return (getStatus() & 0xF) != 0;
        }

        //Reads all axis data as a 3D vector
        void read(Vector& out) const {
            uint8_t temp[6]; // We'll read six bytes from the accelerometer into temp
            transport.readBytes(Registers::OUT_X_L, temp, sizeof(temp));
            out.assign(temp);
        }

        //Reads sensor's memory starting from OUT_X_L address
        void readSample(uint8_t* out, size_t size) const {
            transport.readBytes(Registers::OUT_X_L, out, size);
        }

	};

}
namespace traits {
	template <typename Transport>
    struct scale_count<lsm330::Accelerometer<Transport> > {
		static const unsigned int value = 4;
	};
}

}


#endif //__LSM330DLC_ACCELEROMETER_H