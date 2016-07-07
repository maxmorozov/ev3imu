#ifndef __LSM6DS3_IMU_H
#define __LSM6DS3_IMU_H

#include <stddef.h>
#include <sensors/spi_transport.h>
#include <sensors/vector.h>
#include <sensors/lsm6ds3/ImuBase.h>

namespace sensors {
namespace lsm6ds3 {

    //IMU driver for LSM6DS3
    //TODO Consider adding ByteOrder template parameter to configure platform-specific byte order
    template <typename Transport>
	class Imu : public ImuBase {
    private:
        struct Bitmasks {
            //CTRL1_XL, CTRL2_G
            static const uint8_t ODR = 0xF0;
            //CTRL1_XL
            static const uint8_t AccelScale = 0x0C;
            //CTRL2_G
            static const uint8_t GyroScale = 0x0E;
            //CTRL9_XL
            static const uint8_t AccelAxis = 0x38;
            //CTRL10_C
            static const uint8_t GyroAxis = 0x38;
            //INT1_CTRL, INT2_CTRL
            static const uint8_t AccelDataReadyInt = 0x01;
            static const uint8_t GyroDataReadyInt = 0x02;
            //STATUS_REG
            static const uint8_t AccelDataAvailable = 0x01;
            static const uint8_t GyroDataAvailable = 0x02;
            static const uint8_t TempDataAvailable = 0x04;
            static const uint8_t BootRunning = 0x08;
        };

        struct Bitfields {
            //CTRL1_XL, CTRL2_G
            static const uint8_t PowerDown = 0; //Initial value of CTRL1_XL, CTRL2_G
            //INT1_CTRL
            static const uint8_t DataReadyAccelInt1 = 0x01;
            static const uint8_t DataReadyGyroInt1 = 0x02;
            //INT2_CTRL
            static const uint8_t DataReadyAccelInt2 = 0x01;
            static const uint8_t DataReadyGyroInt2 = 0x02;
            //CTRL4_C
            static const uint8_t DataReadyMask = 0x08;
            //CTRL3_C
            static const uint8_t Reset = 0x01;
        };

    public:
        // AccelScale defines all possible FSR's of the accelerometer:
        enum AccelScale
        {
            SCALE_2G  = 0x00,	// 2g
            SCALE_4G  = 0x08,	// 4g
            SCALE_8G  = 0x0C,	// 8g
            SCALE_16G = 0x04 	// 16g
        };

        // AccelODR defines all possible output data rates of the accelerometer:
        enum AccelODR
        {
            ACC_POWER_DOWN 	 = 0x00,
            ACC_13Hz 		 = 0x10,
            ACC_26Hz 		 = 0x20,
            ACC_52Hz 		 = 0x30,
            ACC_104Hz 		 = 0x40,
            ACC_208Hz 		 = 0x50,
            ACC_416Hz 		 = 0x60,
            ACC_833Hz 		 = 0x70,
            ACC_1660Hz 		 = 0x80,
            ACC_3330Hz 		 = 0x90,
            ACC_6660Hz 		 = 0xA0
        };

        //Accelerometer analog anti-alias filter bandwidth
        enum AccelBandwidth {
            ACC_BW_400Hz  = 0x00,
            ACC_BW_200Hz  = 0x01,
            ACC_BW_100Hz  = 0x02,
            ACC_BW_50Hz   = 0x03
        };

        // GyroScale defines all possible FSR's of the gyroscope:
        enum GyroScale
        {
            SCALE_125DPS  = 0x02,   // 125 degrees per second
            SCALE_245DPS  = 0x00,   // 245 dps
            SCALE_500DPS  = 0x04,   // 500 dps
            SCALE_1000DPS = 0x08,   // 1000 dps
            SCALE_2000DPS = 0x0C    // 2000 dps
        };

        // AccelODR defines all possible output data rates of the accelerometer:
        enum GyroODR
        {
            GYRO_POWER_DOWN  = 0x00,
            GYRO_13Hz 		 = 0x10,
            GYRO_26Hz 		 = 0x20,
            GYRO_52Hz 		 = 0x30,
            GYRO_104Hz 		 = 0x40,
            GYRO_208Hz 		 = 0x50,
            GYRO_416Hz 		 = 0x60,
            GYRO_833Hz 		 = 0x70,
            GYRO_1660Hz 	 = 0x80
        };

        enum DataReadyInterrupt {
            InterruptDisabled     = 0,
            InterruptAccelEnabled = 1,
            InterruptGyroEnabled  = 2,
            InterruptEnabled      = 3 //Accel and Gyro interrupts are enabled
        };

    private:
        Transport transport;

    public:
        static const uint8_t DEVICE_ID = 0x69;

        //Returns the device identifier
        uint8_t getId() {
            return transport.readByte(Registers::WHO_AM_I);   // Read the WHO_AM_I register
        }

        // Initialize the accelerometer.
        // This will set up the scale and output rate of the sensor.
        // It'll also "turn on" the sensor and every axis of the sensor.
        // Input:
        //	- scale = The scale of the accelerometer. Should be a Scale value.
        //	- odr = Output data rate of the accelerometer. ORD value.
        //  - dataReadyInterrupt = generate an interrupt when gyro data is ready
        void init(AccelScale accelScale, AccelODR accelOdr, GyroScale gyroScale, GyroODR gyroOdr, DataReadyInterrupt dataReadyInterrupt)
        {
            //INT1_CTRL - set accel data ready interrupt bit
            //INT2_CTRL - set gyro data ready interrupt bit
            //CTRL4_C - set DRDY_MASK to 1 to skip invalid samples
            //CTRL1_XL - set ODR and scale. Bandwith depends on ODR
            //CTRL2_G - set ODR and scale
            transport.writeByte(Registers::CTRL4_C, Bitfields::DataReadyMask);

            uint8_t int_data[2] = {
                dataReadyInterrupt & Bitmasks::AccelDataReadyInt,
                dataReadyInterrupt & Bitmasks::GyroDataReadyInt
            };
            transport.writeBytes(Registers::INT1_CTRL, int_data, sizeof(int_data));

            uint8_t data[2] = {
                uint8_t(accelOdr) | uint8_t(accelScale),
                uint8_t(gyroOdr) | uint8_t(gyroScale),
            };
            transport.writeBytes(Registers::CTRL1_XL, data, sizeof(data));
        }

        //Returns the sensor to initial state
        void reset() {
            uint8_t data[2] = {0};
            transport.writeBytes(Registers::CTRL1_XL, data, sizeof(data));
            transport.writeBytes(Registers::INT1_CTRL, data, sizeof(data));
        }

        void restart() {
            transport.writeByte(Registers::CTRL3_C, Bitfields::Reset);
        }

/*
        //Turns off the accelerometer
        //Note: if interrupts have been enabled, disableDataReadyInterrupt should be called
        //before next initialization to make sure that interrupts will be activated
        void powerDown() {
            transport.writeByte(Registers::CTRL_REG1, Bitfields::PowerDown);
        }

        //Update the accelerometer's output data rate
        void setODR(ODR odr) {
            transport.writeByteWithMask(Registers::CTRL_REG1, odr << 4, Bitmasks::ODR_mask);
        }

        //Update the accelerometer's full scale range
        void setScale(Scale scale) {
            transport.writeByteWithMask(Registers::CTRL_REG4, scale << 4, Bitmasks::Scale_mask);
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
        uint8_t getConfig() {
            return transport.readByte(Registers::CTRL_REG1);
        }

        void getConfig(uint8_t* data, uint8_t size) {
            transport.readBytes(Registers::CTRL_REG1, data, size);
        }
*/

        //Returns the status reister value
        uint8_t getStatus() {
            return transport.readByte(Registers::STATUS_REG);
        }

        //Returns true if any axis have a new data availavle to read
        bool isNewDataAvailable() {
            return (getStatus() & (Bitmasks::GyroDataAvailable | Bitmasks::AccelDataAvailable)) != 0;
        }

        //Reads all axis data as a 3D vector from accelerometer
        void readAccel(Vector& out) {
            uint8_t temp[6]; // We'll read six bytes from the accelerometer into temp
            transport.readBytes(Registers::OUTX_L_XL, temp, sizeof(temp));
            out.assign(temp);
        }

        //Reads all axis data as a 3D vector from accelerometer
        void readGyro(Vector& out) {
            uint8_t temp[6]; // We'll read six bytes from the accelerometer into temp
            transport.readBytes(Registers::OUTX_L_G, temp, sizeof(temp));
            out.assign(temp);
        }

        //Reads sensor's memory starting from OUTX_L_XL address
        void readAccelSample(uint8_t* out, size_t size) {
            transport.readBytes(Registers::OUTX_L_XL, out, size);
        }

        //Reads sensor's memory starting from OUTX_L_G address
        void readGyroSample(uint8_t* out, size_t size) {
            transport.readBytes(Registers::OUTX_L_G, out, size);
        }
	};
}
}


#endif //__LSM6DS3_IMU_H