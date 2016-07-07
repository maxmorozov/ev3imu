#ifndef __EV3_LSM330DLC_IMU_COMMANDS_H
#define __EV3_LSM330DLC_IMU_COMMANDS_H

#include <ev3/imu/commands.h>

namespace ev3 {
namespace lsm330dlc {

    //Command codes
    struct Commands : public ev3::imu::Commands<Commands> {
        //Commands from the host
        enum Command {
            DEVICE_RESET  = 0x11,

            ACC_SCALE_2G  = 0x20,
            ACC_SCALE_4G  = 0x21,
            ACC_SCALE_8G  = 0x22,
            ACC_SCALE_16G = 0x23,

            GYRO_SCALE_250DPS  = 0x30,
            GYRO_SCALE_500DPS  = 0x31,
            GYRO_SCALE_2000DPS = 0x32,
        };

        //Checks if it is the command to change sensor's scale
        static bool isScaleCommand(uint8_t command) {
            return command >= ACC_SCALE_2G && command <= GYRO_SCALE_2000DPS;
        }

        static bool isUpdateEepromCommand(uint8_t command) {
            return false;
        }

        //Packs the device kind and the target scale into device scale info byte
        //See command_info.h file for detals
        static uint8_t getScaleInfo(uint8_t command) {
            return getInfo<0x20>(command);
        }

        //Packs the device kind and the target scale into device scale info byte
        //See command_info.h file for detals
        static uint8_t getEepromInfo(uint8_t command) {
            return 0;
        }
    };
}
}

#endif //__EV3_LSM330DLC_IMU_COMMANDS_H