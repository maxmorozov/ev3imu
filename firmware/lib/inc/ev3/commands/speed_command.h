#ifndef __EV3_COMMANDS_SPEED_COMMAND_H
#define __EV3_COMMANDS_SPEED_COMMAND_H

#include <ev3/ev3_uart.h>
#include <ev3/commands/checksum.h>
#include <utils/byte_order.h>

namespace ev3 {
namespace commands {

    //Command to report connection speed in bits per second
    template <uint32_t speed>
    class SpeedCommand {
    private:
        //Speed value must be in little-endian format, so we use int32_buffer to place 
        //bytes in the required order.
        typedef utils::int32_buffer<speed> payload_type;
        typedef typename payload_type::value_type value_type;
        static const uint8_t cmd = EV3Command::CMD_SPEED;

        uint8_t command;
        payload_type payload;
        uint8_t checksum;
    public:
        SpeedCommand()
            : command(cmd), checksum(check_sum<cmd, value_type::b1, value_type::b2, value_type::b3, value_type::b4>::value)
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


#endif //__EV3_COMMANDS_SPEED_COMMAND_H