#ifndef __LSM9DS0_MAGNETOMETER_H
#define __LSM9DS0_MAGNETOMETER_H

#include <stddef.h>
#include <sensors/spi_transport.h>
#include <sensors/vector.h>
#include <utils/byte_order.h>
#include <sensors/traits/scale_count.h>

namespace sensors {
namespace lsm9ds0 {

    //Magnetometer driver for lsm9ds0
    //TODO Consider adding ByteOrder template parameter to configure platform-specific byte order
    template <typename Transport>
    class Magnetometer {
    private:
        //////////////////////////////////////////
        // LSM9DS0 Accel/Magneto (XM) Registers //
        //////////////////////////////////////////
        struct Registers {
            static const uint8_t OUT_TEMP_L = 0x05;
            static const uint8_t OUT_TEMP_H = 0x06;
            static const uint8_t STATUS_REG = 0x07;
            static const uint8_t OUT_X_L = 0x08;
            static const uint8_t OUT_X_H = 0x09;
            static const uint8_t OUT_Y_L = 0x0A;
            static const uint8_t OUT_Y_H = 0x0B;
            static const uint8_t OUT_Z_L = 0x0C;
            static const uint8_t OUT_Z_H = 0x0D;
            static const uint8_t WHO_AM_I = 0x0F;
            static const uint8_t INT_CTRL_REG = 0x12;
            static const uint8_t INT_SRC_REG = 0x13;
            static const uint8_t INT_THS_L = 0x14;
            static const uint8_t INT_THS_H = 0x15;
            static const uint8_t OFFSET_X_L = 0x16;
            static const uint8_t OFFSET_X_H = 0x17;
            static const uint8_t OFFSET_Y_L = 0x18;
            static const uint8_t OFFSET_Y_H = 0x19;
            static const uint8_t OFFSET_Z_L = 0x1A;
            static const uint8_t OFFSET_Z_H = 0x1B;
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
            //CTRL_REG5
            static const uint8_t ODR  = 0x1C;
            static const uint8_t Resolution  = 0x60;
            //CTRL_REG6
            static const uint8_t Scale = 0x60;
            //CTRL_REG7
            static const uint8_t LowPower = 0x04;
            static const uint8_t Mode = 0x03;
        };

        struct Bitfields {
            //CTRL_REG5
            static const uint8_t InitialMode = 0x18;
            //CTRL_REG6
            static const uint8_t InitialModeReg6 = 0x20;
            //CTRL_REG7
            static const uint8_t InitialModeReg7 = 0x02;

            //CTRL_REG7
            static const uint8_t LowPowerMode = 0x04;
            //CTRL_REG3
            //The first version of PCB has only two interrupt wires.
            //The seond version has three wires, so we can use INT_XM2
            //to handle magnetometer data ready events.
            //In this version we share interrupt with accelerometer, so
            //we need to check status register to identify the event source.
            static const uint8_t DataReadyInterrupt = 0x02;
        };

    public:
        // Scale defines all possible FSR's of the magnetometer:
        enum Scale
        {
            SCALE_2GS,	// 00:  2Gs
            SCALE_4GS, 	// 01:  4Gs
            SCALE_8GS,	// 10:  8Gs
            SCALE_12GS,	// 11:  12Gs
        };

        // ODR defines all possible output data rates of the magnetometer:
        enum ODR
        {
            ODR_3,      // 3.125 Hz (0x00)
            ODR_6,      // 6.25 Hz (0x01)
            ODR_12,     // 12.5 Hz (0x02)
            ODR_25,     // 25 Hz (0x03)
            ODR_50,     // 50 (0x04)
            ODR_100,    // 100 Hz (0x05)
        };

        //Magnetic sensor mode selection
        enum OperationMode {
            OM_CONTINIOUS,
            OM_SINGLE,
            OM_POWER_DOWN

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
        void init(Scale scale, ODR odr, DataReadyInterrupt dataReadyInterrupt)
        {
            uint8_t data[3] = {
                    (odr << 2) | 0xE0, //Temperature sensor is enabled, high resolution is selected
                    (scale << 5),
                    OM_CONTINIOUS      //Continuous conversion mode
            };
            transport.writeBytes(Registers::CTRL_REG5, data, sizeof(data));
            transport.writeByteWithMask(Registers::CTRL_REG3, dataReadyInterrupt, Bitfields::DataReadyInterrupt);
        }

        //Returns the sensor to initial state
        void reset() {
            uint8_t data[3] = {Bitfields::InitialMode, Bitfields::InitialModeReg6, Bitfields::InitialModeReg7};
            transport.writeBytes(Registers::CTRL_REG5, data, sizeof(data));
            transport.clearBits(Registers::CTRL_REG3, Bitfields::DataReadyInterrupt);
        }

        //Turns off the accelerometer
        //Note: if interrupts have been enabled, disableDataReadyInterrupt should be called
        //before next initialization to make sure that interrupts will be activated
        void powerDown() {
            transport.writeByteWithMask(Registers::CTRL_REG7, OM_POWER_DOWN, Bitfields::PowerDown);
        }

        //Update the accelerometer's output data rate
        void setODR(ODR odr) {
            transport.writeByteWithMask(Registers::CTRL_REG5, odr << 2, Bitmasks::ODR);
        }

        //Update the accelerometer's full scale range
        void setScale(Scale scale) {
            transport.writeByteWithMask(Registers::CTRL_REG6, scale << 5, Bitmasks::Scale);
        }

        //Turns on the low power mode
        void enableLowPowerMode() {
            transport.setBits(Registers::CTRL_REG7, Bitfields::LowPowerMode);
        }

        //Turns off the low power mode
        void disableLowPowerMode() {
            transport.clearBits(Registers::CTRL_REG7, Bitfields::LowPowerMode);
        }

        //Enable interrupt generation on INT1_A pin when the sample is ready
        void enableDataReadyInterrupt() {
            transport.setBits(Registers::CTRL_REG3, Bitfields::DataReadyInterrupt);
        }

        //Disable interrupt generation on INT1_A pin when the sample is ready
        void disableDataReadyInterrupt() {
            transport.clearBits(Registers::CTRL_REG3, Bitfields::DataReadyInterrupt);
        }

        //Returns the value of the first control register
        uint8_t getConfig() const {
            return transport.readByte(Registers::CTRL_REG5);
        }

        void getConfig(uint8_t* data, uint8_t size) const {
            transport.readBytes(Registers::CTRL_REG5, data, size);
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
    struct scale_count<lsm9ds0::Magnetometer<Transport> > {
		static const unsigned int value = 4;
	};
}

}


#endif //__LSM9DS0_MAGNETOMETER_H