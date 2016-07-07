#ifndef __LSM9DS0_ACCELEROMETER_H
#define __LSM9DS0_ACCELEROMETER_H

#include <stddef.h>
#include <sensors/spi_transport.h>
#include <sensors/vector.h>
#include <utils/byte_order.h>
#include <sensors/traits/scale_count.h>

namespace sensors {
namespace lsm9ds0 {

    //Accelerometer driver for lsm9ds0
    //TODO Consider adding ByteOrder template parameter to configure platform-specific byte order
    template <typename Transport>
    class Accelerometer {
    private:
        //////////////////////////////////////////
        // LSM9DS0 Accelerometer Registers      //
        //////////////////////////////////////////
        struct Registers {
            static const uint8_t OUT_TEMP_L = 0x05;
            static const uint8_t OUT_TEMP_H = 0x06;
            static const uint8_t WHO_AM_I = 0x0F;
            static const uint8_t REFERENCE_X = 0x1C;
            static const uint8_t REFERENCE_Y = 0x1D;
            static const uint8_t REFERENCE_Z = 0x1E;
            static const uint8_t CTRL_REG0 = 0x1F;
            static const uint8_t CTRL_REG1 = 0x20;
            static const uint8_t CTRL_REG2 = 0x21;
            static const uint8_t CTRL_REG3 = 0x22;
            static const uint8_t CTRL_REG4 = 0x23;
            static const uint8_t CTRL_REG5 = 0x24;
            static const uint8_t CTRL_REG6 = 0x25;
            static const uint8_t CTRL_REG7 = 0x26;
            static const uint8_t STATUS_REG = 0x27;
            static const uint8_t OUT_X_L = 0x28;
            static const uint8_t OUT_X_H = 0x29;
            static const uint8_t OUT_Y_L = 0x2A;
            static const uint8_t OUT_Y_H = 0x2B;
            static const uint8_t OUT_Z_L = 0x2C;
            static const uint8_t OUT_Z_H = 0x2D;
            static const uint8_t FIFO_CTRL_REG = 0x2E;
            static const uint8_t FIFO_SRC_REG = 0x2F;
            static const uint8_t INT_GEN_1_REG = 0x30;
            static const uint8_t INT_GEN_1_SRC = 0x31;
            static const uint8_t INT_GEN_1_THS = 0x32;
            static const uint8_t INT_GEN_1_DURATION = 0x33;
            static const uint8_t INT_GEN_2_REG = 0x34;
            static const uint8_t INT_GEN_2_SRC = 0x35;
            static const uint8_t INT_GEN_2_THS = 0x36;
            static const uint8_t INT_GEN_2_DURATION = 0x37;
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
            //CTRL_REG2
            static const uint8_t Scale = 0x38;
            static const uint8_t BandWidth = 0xC0;
            static const uint8_t SelfTest = 0x06;

            //static const uint8_t Endiannes  = 0x40;
        };

        struct Bitfields {
            //CTRL_REG1
            static const uint8_t PowerDown = 0x07; //Initial value of CTRL_REG1
            //CTRL_REG3
            static const uint8_t DataReadyInterrupt = 0x04;
            //static const uint8_t FifoWaterMarkInterrupt = 0x04;
            //static const uint8_t FifoOverrunInterrupt = 0x02;
        };

    public:
        // Scale defines all possible FSR's of the accelerometer:
        enum Scale
        {
            SCALE_2G,	// 000:  2g
            SCALE_4G,	// 001:  4g
            SCALE_6G,	// 010:  4g
            SCALE_8G,	// 011:  8g
            SCALE_16G	// 100:  16g
        };

        // ODR defines all possible output data rates of the accelerometer:
        enum ODR
        {
            POWER_DOWN,     // Power-down mode (0x0)
            ODR_3125,       // 3.125 Hz	(0x1)
            ODR_625,        // 6.25 Hz (0x2)
            ODR_125,        // 12.5 Hz (0x3)
            ODR_25,         // 25 Hz (0x4)
            ODR_50,         // 50 Hz (0x5)
            ODR_100,        // 100 Hz (0x6)
            ODR_200,        // 200 Hz (0x7)
            ODR_400,        // 400 Hz (0x8)
            ODR_800,        // 800 Hz (9)
            ODR_1600        // 1600 Hz (0xA)
        };


        // LPFBandWidth defines all possible anti-aliasing filter rates of the accelerometer:
        enum BandWidth
        {
            BW_773,     // 773 Hz (0x0)
            BW_194,     // 194 Hz (0x1)
            BW_362,     // 362 Hz (0x2)
            BW_50,      //  50 Hz (0x3)
        };

        //Self test mode configuration
        enum SelfTestConfig {
            ST_NORNAL_MODE,      //Normal operation mode (no self-test)
            ST_POSITIVE,
            ST_NEGATIVE,
            ST_NOT_ALLOWED
        };

        enum DataReadyInterrupt {
            InterruptDisabled = 0,
            InterruptEnabled =  Bitfields::DataReadyInterrupt
        };

    private:
        Transport transport;

    public:
        static const uint8_t DEVICE_ID = 0x49;

        //Returns the device identifier
        uint8_t getId() const {
            return transport.readByte(Registers::WHO_AM_I);   // Read the WHO_AM_I register
        }

        //Checks if the device works correctly
        bool checkDevice() const {
            return getId() == DEVICE_ID;
        }

        // Initialize the accelerometer.
        // This will set up the scale and output rate of the sensor.
        // It'll also "turn on" the sensor and every axis of the sensor.
        // Input:
        //	- scale = The scale of the accelerometer. Should be a Scale value.
        //	- odr = Output data rate of the accelerometer. ORD value.
        //  - dataReadyInterrupt = generate an interrupt when gyro data is ready
        //  - bandWidth = input low-pass anti-alias filter bandwidth
        void init(Scale scale, ODR odr, DataReadyInterrupt dataReadyInterrupt, BandWidth bandWidth)
        {
            uint8_t data[3] = {
                    (odr << 4) | 0x7, //Block data update is false
                    (bandWidth << 6) | (scale << 3),
                    dataReadyInterrupt};
            transport.writeBytes(Registers::CTRL_REG1, data, sizeof(data));
        }

        //Returns the sensor to initial state
        void reset() {
            uint8_t data[3] = {Bitfields::PowerDown, 0, 0};
            transport.writeBytes(Registers::CTRL_REG1, data, sizeof(data));
        }

        //Turns off the accelerometer
        //Note: if interrupts have been enabled, disableDataReadyInterrupt should be called
        //before next initialization to make sure that interrupts will be activated
        void powerDown() {
            setODR(POWER_DOWN);
        }

        //Update the accelerometer's output data rate
        void setODR(ODR odr) {
            transport.writeByteWithMask(Registers::CTRL_REG1, odr << 4, Bitmasks::ODR);
        }

        //Update the accelerometer's anti-alias low-pass filter bandwidth
        void setBandWidth(BandWidth bandWidth) {
            transport.writeByteWithMask(Registers::CTRL_REG2, bandWidth << 6, Bitmasks::BandWidth);
        }

        //Update the accelerometer's full scale range
        void setScale(Scale scale) {
            transport.writeByteWithMask(Registers::CTRL_REG2, scale << 3, Bitmasks::Scale);
        }

        //Enable interrupt generation on INT1_A pin when the sample is ready
        void enableDataReadyInterrupt() {
            transport.setBits(Registers::CTRL_REG3, Bitfields::DataReadyInterrupt);
        }

        //Disable interrupt generation on INT1_A pin when the sample is ready
        void disableDataReadyInterrupt() {
            transport.clearBits(Registers::CTRL_REG3, Bitfields::DataReadyInterrupt);
        }

        //Starts self-testing procedure
        void enableSelfTest(SelfTestConfig selfTestMode) {
            transport.writeByteWithMask(Registers::CTRL_REG2, selfTestMode << 1, Bitmasks::SelfTest);
        }

        //Stops self-testing procedure
        void disableSelfTest() {
            transport.clearBits(Registers::CTRL_REG2, Bitmasks::SelfTest);
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
    struct scale_count<lsm9ds0::Accelerometer<Transport> > {
		static const unsigned int value = 5;
	};
}

}


#endif //__LSM9DS0_ACCELEROMETER_H