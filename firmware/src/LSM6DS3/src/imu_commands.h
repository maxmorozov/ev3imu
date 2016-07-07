#ifndef __EV3_LSM6DS3_IMU_COMMANDS_H
#define __EV3_LSM6DS3_IMU_COMMANDS_H

#include <ev3/command_info.h>
#include <ev3/imu/commands.h>

namespace ev3 {
namespace lsm6ds3 {

    //Command codes
    struct Commands : public ev3::imu::Commands<Commands> {
        //Commands from the host
        enum Command {
            //Return device to the state right after power on
            DEVICE_RESET  = 0x11,

            //Accelerometer sensitivity
            ACC_SCALE_2G  = 0x20,
            ACC_SCALE_4G  = 0x21,
            ACC_SCALE_8G  = 0x22,
            ACC_SCALE_16G = 0x23,

            //Gyroscope sensitivity
            GYRO_SCALE_245DPS  = 0x30,
            GYRO_SCALE_500DPS  = 0x31,
            GYRO_SCALE_1000DPS = 0x32,
            GYRO_SCALE_2000DPS = 0x33,
            GYRO_SCALE_125DPS  = 0x34,

            //Write calibration matrix into EEPROM
            CALIBRATE_ACC_2G  = 0x40,
            CALIBRATE_ACC_4G  = 0x41,
            CALIBRATE_ACC_8G  = 0x42,
            CALIBRATE_ACC_16G = 0x43,

            CALIBRATE_GYRO_245DPS  = 0x50,
            CALIBRATE_GYRO_500DPS  = 0x51,
            CALIBRATE_GYRO_1000DPS = 0x52,
            CALIBRATE_GYRO_2000DPS = 0x53,
            CALIBRATE_GYRO_125DPS  = 0x54,
        };

        //Checks if it is the command to change sensor's scale
        static bool isScaleCommand(uint8_t command) {
            return (command >= ACC_SCALE_2G && command <= GYRO_SCALE_125DPS);
        }

        static bool isUpdateEepromCommand(uint8_t command) {
            return (command >= CALIBRATE_ACC_2G && command <= CALIBRATE_GYRO_125DPS);
        }

        //Packs the device kind and the target scale into device scale info byte
        //See command_info.h file for detals
        static uint8_t getScaleInfo(uint8_t command) {
            return getInfo<0x20>(command);
        }

        //Packs the device kind and the target scale into device scale info byte
        //See command_info.h file for detals
        static uint8_t getEepromInfo(uint8_t command) {
            return getInfo<0x40>(command);
        }
    };
}
}

#endif //__EV3_LSM6DS3_IMU_COMMANDS_H