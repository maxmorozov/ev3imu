#ifndef __EV3_IMU_COMMANDS_H
#define __EV3_IMU_COMMANDS_H

#include <ev3/command_info.h>

namespace ev3 {
namespace imu {    

    //Common code to command processing and encoding
    template <typename CommandImpl>
    struct Commands {
        //Packs the device kind and the target scale into device scale info byte
        //See command_info.h file for detals
        template <uint8_t offset>
        static uint8_t getInfo(uint8_t command) {
            //implements formula (x-offset)*8 where x is high nibble of the command: x = (command 0xF0) >> 4
            //I use shift operator instead of / 2 to avoid usage of 16-bit arithmetic
            return ((command - offset) >> 1) & ScaleInfoMask::Device | (command & ScaleInfoMask::Scale);
        }

        //Implements processing of the device specific commands
        #pragma inline=forced
        template <typename Device, typename MessageCommand>
        static void handleCommand(Device* device, const MessageCommand& command) {
            if (CommandImpl::isScaleCommand(command.hostCommand())) {
                device->setScale(CommandImpl::getScaleInfo(command.hostCommand()));
            } else if (CommandImpl::isUpdateEepromCommand(command.hostCommand())) {
                //The first byte is the host command.
                device->writeEeprom(CommandImpl::getEepromInfo(command.hostCommand()), command.payload(), command.payload_size());
            }
        }
    };
}}

#endif //__EV3_IMU_COMMANDS_H