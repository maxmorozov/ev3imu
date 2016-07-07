#ifndef __EV3_COMMANDS_MESSAGE_COMMAND_H
#define __EV3_COMMANDS_MESSAGE_COMMAND_H

#include <stdint.h>
#include <utils/inline.h>

namespace ev3 {
namespace commands {

    //Helper object to read messages from the EV3 host
    template <uint8_t data_size>
    class MessageCommand {
    private:
        uint8_t buffer_size;
        uint8_t data[data_size];
    public:
        INLINE MessageCommand(uint8_t len)
            : buffer_size(len)
        {
        }

        //Access the message buffer
        uint8_t* buffer() { return data; }
        const uint8_t* buffer() const { return data; }
        
        //Returns the message size
        uint8_t size() const { return buffer_size; }

        //Checks if the message fit into the buffer
        bool is_valid_size() const { 
            return buffer_size <= data_size;
        }
        
        //EV3 message protocol: the first byte is a command: CMD_SELECT, CMD_WRITE
        uint8_t hostCommand() const { return data[0]; }
        
        //Peturns pointer to the message payload
        uint8_t* payload() { return data + 1; }
        const uint8_t* payload() const { return data + 1; }
        
        //Return payload size
        uint8_t payload_size() const { return buffer_size - 1; }
    };

}
}


#endif //__EV3_COMMANDS_MESSAGE_COMMAND_H