#ifndef __EV3_COMMANDS_DATA_COMMAND_H
#define __EV3_COMMANDS_DATA_COMMAND_H

#include <ev3/ev3_uart.h>
#include <ev3/commands/checksum.h>

namespace ev3 {
namespace commands {

    template <uint8_t data_size>
    class DataCommand {
    private:
        //size of buffer in the command
        static const uint8_t buffer_size = mpl::clp2<data_size>::value;
        //encoded buffer size to make command byte
        static const uint8_t buffer_size_log = mpl::log2<buffer_size>::value;
        
        uint8_t command;
        uint8_t data[buffer_size];
        uint8_t checksum;
    public:
        INLINE DataCommand(uint8_t mode)
        {
            command = UartProtocol::makeData(mode, buffer_size_log);
            checksum = ev3::commands::checksum(&command, sizeof(command) + sizeof(data));
        }

        //Makes the command 
        const uint8_t* get() const {
            return &command;
        }

        uint8_t size() const {
            return sizeof(*this);
        }
        
        //Provide access to the buffer to fill in
        uint8_t* buffer() { 
            return data;
        }
    };

}
}


#endif //__EV3_COMMANDS_DATA_COMMAND_H