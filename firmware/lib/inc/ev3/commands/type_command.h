#ifndef __EV3_COMMANDS_TYPE_COMMAND_H
#define __EV3_COMMANDS_TYPE_COMMAND_H

#include <ev3/ev3_uart.h>
#include <ev3/commands/checksum.h>

namespace ev3 {
namespace commands {

    //Device type descriptor
    //Type ranges:
    //0           "Don't change type" type
    //1..49       Reserved for LEGO existing and future devices
    //50..98      Free to 3th. party devices
    //99          Reserved for LEGO energy meter
    //100..127    Reserved for internal use
    template <uint8_t type_>
    class TypeCommand {
    private:
        static const uint8_t cmd = EV3Command::CMD_TYPE;

        uint8_t command;
        uint8_t type;
        uint8_t checksum;
    public:
        TypeCommand()
            : command(cmd), type(type_), checksum(check_sum<cmd, type_>::value)
        {
        }

        operator const uint8_t*() const {
            return &command;
        }

        uint8_t size() const {
            return sizeof(*this);
        }
    };

}
}


#endif //__EV3_COMMANDS_TYPE_COMMAND_H