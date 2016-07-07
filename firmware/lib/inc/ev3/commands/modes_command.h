#ifndef __EV3_COMMANDS_MODES_COMMAND_H
#define __EV3_COMMANDS_MODES_COMMAND_H

#include <ev3/ev3_uart.h>
#include <ev3/commands/checksum.h>

namespace ev3 {
namespace commands {

    //Modes count descriptor
    // modes - number of supported modes
    // views - number of visible modes
    template <uint8_t modes, uint8_t views>
    class ModesCommand {
    private:
        static const uint8_t mode_index = modes - 1;
        static const uint8_t view_index = views - 1;

        static const uint8_t cmd = EV3Command::CMD_MODES;

        uint8_t command;
        uint8_t modeMaxIndex;
        uint8_t viewMaxIndex;
        uint8_t checksum;
    public:
        ModesCommand()
            : command(cmd), modeMaxIndex(mode_index),
            viewMaxIndex(view_index),
            checksum(check_sum<cmd, mode_index, view_index>::value)
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


#endif //__EV3_COMMANDS_MODES_COMMAND_H