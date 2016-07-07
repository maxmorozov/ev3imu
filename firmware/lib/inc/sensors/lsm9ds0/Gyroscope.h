#ifndef __LSM9DS0_GYROSCOPE_H
#define __LSM9DS0_GYROSCOPE_H

#include <sensors/spi_transport.h>
#include <sensors/vector.h>
#include <utils/byte_order.h>
#include <sensors/traits/scale_count.h>

namespace sensors {
namespace lsm9ds0 {

    //Gyroscope driver for lsm9ds0
    //TODO Consider adding ByteOrder template parameter to configure platform-specific byte order
    template <typename Transport>
    class Gyroscope {
        //////////////////////////////
        // LSM9DS0 Gyro Registers   //
        //////////////////////////////
        struct Registers {
            static const uint8_t WHO_AM_I = 0x0F;
            static const uint8_t CTRL_REG1 = 0x20;
            static const uint8_t CTRL_REG2 = 0x21;
            static const uint8_t CTRL_REG3 = 0x22;
            static const uint8_t CTRL_REG4 = 0x23;
            static const uint8_t CTRL_REG5 = 0x24;
            static const uint8_t REFERENCE = 0x25;
            static const uint8_t OUT_TEMP = 0x26;
            static const uint8_t STATUS_REG = 0x27;
            static const uint8_t OUT_X_L = 0x28;
            static const uint8_t OUT_X_H = 0x29;
            static const uint8_t OUT_Y_L = 0x2A;
            static const uint8_t OUT_Y_H = 0x2B;
            static const uint8_t OUT_Z_L = 0x2C;
            static const uint8_t OUT_Z_H = 0x2D;
            static const uint8_t FIFO_CTRL_REG = 0x2E;
            static const uint8_t FIFO_SRC_REG = 0x2F;
            static const uint8_t INT1_CFG = 0x30;
            static const uint8_t INT1_SRC = 0x31;
            static const uint8_t INT1_THS_XH = 0x32;
            static const uint8_t INT1_THS_XL = 0x33;
            static const uint8_t INT1_THS_YH = 0x34;
            static const uint8_t INT1_THS_YL = 0x35;
            static const uint8_t INT1_THS_ZH = 0x36;
            static const uint8_t INT1_THS_ZL = 0x37;
            static const uint8_t INT1_DURATION = 0x38;
        };

        struct Bitmasks {
            //CTRL_REG1
            static const uint8_t ODR = 0xF0;
            static const uint8_t PowerDown = 0x08;
            static const uint8_t Axis = 0x07;

            //CTRL_REG4
            static const uint8_t Scale = 0x30;
            static const uint8_t Endiannes = 0x40;
            static const uint8_t SelfTest = 0x06;
        };

        struct Bitfields {
            //CTRL_REG1
            static const uint8_t PowerDown = 0x07; //Initial value of CTRL_REG1
            //CTRL_REG2
            static const uint8_t EdgeSensitiveTrigger = 0x80;
            static const uint8_t LevelSensitiveTrigger = 0x40;

            //CTRL_REG3
            static const uint8_t DataReadyInterrupt = 0x08;
            static const uint8_t FifoWaterMarkInterrupt = 0x04;
            static const uint8_t FifoOverrunInterrupt = 0x02;
            static const uint8_t FifoEmptyInterrupt = 0x01;
        };

    public:
        // Scale defines the possible full-scale ranges of the gyroscope:
        enum Scale
        {
            SCALE_245DPS,	// 00:  250 degrees per second
            SCALE_500DPS,	// 01:  500 dps
            SCALE_2000DPS,	// 10:  2000 dps
        };
        // ODR defines all possible data rate/bandwidth combos of the gyro:
        enum ODR
        {	    				  // ODR (Hz) --- Cutoff
            ODR_95_BW_125  = 0x0, //   95         12.5
            ODR_95_BW_25   = 0x1, //   95          25
            // 0x2 and 0x3 define the same data rate and bandwidth
            ODR_190_BW_125 = 0x4, //   190        12.5
            ODR_190_BW_25  = 0x5, //   190         25
            ODR_190_BW_50  = 0x6, //   190         50
            ODR_190_BW_70  = 0x7, //   190         70
            ODR_380_BW_20  = 0x8, //   380         20
            ODR_380_BW_25  = 0x9, //   380         25
            ODR_380_BW_50  = 0xA, //   380         50
            ODR_380_BW_100 = 0xB, //   380         100
            ODR_760_BW_30  = 0xC, //   760         30
            ODR_760_BW_35  = 0xD, //   760         35
            ODR_760_BW_50  = 0xE, //   760         50
            ODR_760_BW_100 = 0xF, //   760         100
        };

        enum Mode {
            PowerDown,
            Normal,
            Sleep      ///<! sleep mode only when all axis will be disabled
        };

        enum SyncMode {
            NoSync = 0,
            Sync = Bitfields::EdgeSensitiveTrigger
        };

        //Self test mode configuration
        enum SelfTestConfig {
            ST_NORNAL_MODE = 0,    //Normal operation mode (no self-test)
            ST_MODE0 = 1,          //Self-test 0 (X positive sign, Y and Z negative sign)
            ST_MODE1 = 3           //Self-test 1 (X negative sign, Y and Z positive sign)
        };

        enum DataReadyInterrupt {
            InterruptDisabled = 0,
            InterruptEnabled =  Bitfields::DataReadyInterrupt
        };

    private:
        Transport transport;

    public:
        static const uint8_t DEVICE_ID = 0xD4;

        //Returns the device identifier
        uint8_t getId() const {
            return transport.readByte(Registers::WHO_AM_I);   // Read the WHO_AM_I register
        }

        //Checks if the device works correctly
        bool checkDevice() const {
            return getId() == DEVICE_ID;
        }

        // Initialize the gyroscope.
        // This will set up the scale and output rate of the sensor.
        // It'll also "turn on" the sensor and every axis of the sensor.
        // Input:
        //	- scale = The scale of the gyroscope. Should be a Scale value.
        //	- odr = Output data rate of the gyroscope. ORD value.
        //  - sync = synchronization with accelerometer
        //  - dataReadyInterrupt = generate an interrupt when gyro data is ready
        void init(Scale scale, ODR odr, DataReadyInterrupt dataReadyInterrupt, SyncMode syncMode)
        {
            uint8_t data[4] = {
                    (odr << 4) | 0x0F, // Normal mode, all axis enabled
                    syncMode,
                    dataReadyInterrupt,
                    scale << 4};
            transport.writeBytes(Registers::CTRL_REG1, data, sizeof(data));
        }

        //Returns the sensor to initial state
        void reset() {
            uint8_t data[4] = {Bitfields::PowerDown, 0, 0, 0};
            transport.writeBytes(Registers::CTRL_REG1, data, sizeof(data));
        }

        //Turns off the gyroscope
        //Note: if interrupts have been enabled, disableDataReadyInterrupt should be called
        //before next initialization to make sure that interrupts will be activated
        void powerDown() {
            transport.writeByte(Registers::CTRL_REG1, Bitfields::PowerDown);
        }

        //Update the gyroscope output data rate
        void setODR(ODR odr) {
            transport.writeByteWithMask(Registers::CTRL_REG1, odr << 4, Bitmasks::ODR);
        }

        //Update the gyroscope full scale range
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

        //Set the gyroscope mode: power down, normal or sleep
        void setMode(Mode mode) {
            switch (mode){
            case PowerDown:
                transport.clearBits(Registers::CTRL_REG1, Bitmasks::PowerDown);
                break;
            case Normal:
                transport.setBits(Registers::CTRL_REG1, Bitmasks::PowerDown | Bitmasks::Axis);
                break;
            case Sleep:
                transport.writeByteWithMask(Registers::CTRL_REG1, Bitmasks::PowerDown, Bitmasks::PowerDown | Bitmasks::Axis);
                break;
            }
        }

        //Enable interrupt generation on DRDY_G/INT2_G pin when the sample is ready
        void enableDataReadyInterrupt() {
            transport.setBits(Registers::CTRL_REG3, Bitfields::DataReadyInterrupt);
        }

        //Disable interrupt generation on DRDY_G/INT2_G pin when the sample is ready
        void disableDataReadyInterrupt() {
            transport.clearBits(Registers::CTRL_REG3, Bitfields::DataReadyInterrupt);
        }

        //Enable reading gyro samples when the high level appears on DEN_G input pin
        //The exteranal ODR should be less than ODR/2
        void enableSynchronization() {
            transport.setBits(Registers::CTRL_REG2, Bitfields::EdgeSensitiveTrigger);
        }

        //Disable reading gyro samples when the high level appears on DEN_G input pin.
        //The gyros samples are reading using the internal ODR
        void disableSynchronization() {
            transport.clearBits(Registers::CTRL_REG2, Bitfields::EdgeSensitiveTrigger);
        }

        //Starts self-testing procedure
        void enableSelfTest(SelfTestConfig selfTestMode) {
            transport.writeByteWithMask(Registers::CTRL_REG4, selfTestMode << 1, Bitmasks::SelfTest);
        }

        //Stops self-testing procedure
        void disableSelfTest() {
            transport.clearBits(Registers::CTRL_REG4, Bitmasks::SelfTest);
        }


        //Returns the value of the first control register
        uint8_t getConfig() const {
            return transport.readByte(Registers::CTRL_REG1);
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
            uint8_t temp[6]; // We'll read six bytes from the gyroscope into temp
            transport.readBytes(Registers::OUT_X_L, temp, sizeof(temp));
            out.assign(temp);
        }

        //Reads sensor's memory starting from OUT_X_L address
        void readSample(uint8_t* out, size_t size) const {
            transport.readBytes(Registers::OUT_X_L, out, size);
        }

        //Reads the gyroscope temperature
        int8_t readTemperature() const {
            return transport.readByte(Registers::OUT_TEMP);
        }
    };
}

namespace traits {
	template <typename Transport>
    struct scale_count<lsm9ds0::Gyroscope<Transport> > {
		static const unsigned int value = 3;
	};
}

}

#endif //__LSM9DS0_GYROSCOPE_H