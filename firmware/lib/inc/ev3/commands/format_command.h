#ifndef __EV3_COMMANDS_FORMAT_COMMAND_H
#define __EV3_COMMANDS_FORMAT_COMMAND_H

#include <ev3/ev3_uart.h>
#include <ev3/sensor_types.h>
#include <ev3/commands/checksum.h>
#include <mpl/float.h>

namespace ev3 {
namespace commands {

    //Info message for data range descriptor
    //This command should be the last command in the mode descriptor.
    template <uint8_t mode, uint8_t sample_size, DataType data_type, uint8_t figures_, uint8_t decimals_>
    class FormatCommand {
    private:
        static const uint8_t cmd = EV3Command::info<mode, sizeof(uint8_t) * 4>::value;
        static const uint8_t info_byte = UartProtocol::InfoByte::FORMAT;


        uint8_t command;
        uint8_t infoByte;
        uint8_t sampleSize;
        uint8_t dataType;
        uint8_t figures;
        uint8_t decimals;
        uint8_t checksum;
    public:
        FormatCommand()
            : command(cmd), infoByte(info_byte),
            sampleSize(sample_size), dataType(data_type), figures(figures_), decimals(decimals_),
            checksum(check_sum<cmd, info_byte, sample_size, data_type, figures_, decimals_>::value)
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


#endif //__EV3_COMMANDS_FORMAT_COMMAND_H