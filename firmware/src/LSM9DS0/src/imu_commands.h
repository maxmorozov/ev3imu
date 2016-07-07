#ifndef __EV3_LSM9DS0_IMU_COMMANDS_H
#define __EV3_LSM9DS0_IMU_COMMANDS_H

#include <ev3/command_info.h>
#include <ev3/imu/commands.h>

namespace ev3 {
namespace lsm9ds0 {

    //Command codes
    struct Commands : public ev3::imu::Commands<Commands> {
        //Commands from the host
        enum Command {
            //Return device to the state right after power on
            DEVICE_RESET  = 0x11,

            //Accelerometer sensitivity
            ACC_SCALE_2G  = 0x20,
            ACC_SCALE_4G  = 0x21,
            ACC_SCALE_6G  = 0x22,
            ACC_SCALE_8G  = 0x23,
            ACC_SCALE_16G = 0x24,

            //Gyroscope sensitivity
            GYRO_SCALE_250DPS  = 0x30,
            GYRO_SCALE_500DPS  = 0x31,
            GYRO_SCALE_2000DPS = 0x32,

            //Magnetometer sensitivity
            MAG_SCALE_2GS  = 0x40,
            MAG_SCALE_4GS  = 0x41,
            MAG_SCALE_8GS  = 0x42,
            MAG_SCALE_12GS = 0x43,

            //Write calibration matrix into EEPROM
            CALIBRATE_ACC_2G  = 0x50,
            CALIBRATE_ACC_4G  = 0x51,
            CALIBRATE_ACC_6G  = 0x52,
            CALIBRATE_ACC_8G  = 0x53,
            CALIBRATE_ACC_16G = 0x54,

            CALIBRATE_GYRO_250DPS  = 0x60,
            CALIBRATE_GYRO_500DPS  = 0x61,
            CALIBRATE_GYRO_2000DPS = 0x62,

            CALIBRATE_MAG_2GS  = 0x70,
            CALIBRATE_MAG_4GS  = 0x71,
            CALIBRATE_MAG_8GS  = 0x72,
            CALIBRATE_MAG_12GS = 0x73,
        };

        //Checks if it is the command to change sensor's scale
        static bool isScaleCommand(uint8_t command) {
            return command >= ACC_SCALE_2G && command <= MAG_SCALE_12GS;
        }

        static bool isUpdateEepromCommand(uint8_t command) {
            return (command >= CALIBRATE_ACC_2G && command <= CALIBRATE_MAG_12GS);
        }

        //Packs the device kind and the target scale into device scale info byte
        //See command_info.h file for detals
        static uint8_t getScaleInfo(uint8_t command) {
            return getInfo<0x20>(command);
        }

        //Packs the device kind and the target scale into device scale info byte
        //See command_info.h file for detals
        static uint8_t getEepromInfo(uint8_t command) {
            return getInfo<0x50>(command);
        }
    };
}
}

#endif //__EV3_LSM9DS0_IMU_COMMANDS_H