#ifndef __EV3_COMMANDS_RANGE_COMMAND_H
#define __EV3_COMMANDS_RANGE_COMMAND_H

#include <ev3/ev3_uart.h>
#include <ev3/commands/checksum.h>
#include <mpl/float.h>

namespace ev3 {
namespace commands {

    //Info message for data range descriptor
    //Optional command to describe output data range.
    //If data range is not specified, the default range [0..1023] will be used.
    template <uint8_t info_byte, uint8_t mode, int32_t min_value, int32_t max_value>
    class RangeCommand {
    private:
        static const uint8_t cmd = EV3Command::info<mode, sizeof(int32_t) * 2>::value;

        template <typename T>
        struct xor_int {
            static const uint8_t value = T::b1 ^ T::b2 ^ T::b3 ^ T::b4;
        };

        //Min/Max values should be in little-endian format, so we use uint32_buffer to place 
        //bytes in the required order.
        //Min/Max values must be in float format, so we use int_to_float metafunction to
        //compose floating point values using 32-bit integer type.
        typedef utils::uint32_buffer<mpl::int_to_float<min_value>::value> min_value_type;
        typedef utils::uint32_buffer<mpl::int_to_float<max_value>::value> max_value_type;

        uint8_t command;
        uint8_t infoByte;
        min_value_type minValue;
        max_value_type maxValue;
        uint8_t checksum;
    public:
        RangeCommand()
            : command(cmd), infoByte(info_byte),
            checksum(check_sum<cmd, info_byte,
                     xor_int<typename min_value_type::value_type>::value,
                     xor_int<typename max_value_type::value_type>::value>::value)
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


#endif //__EV3_COMMANDS_RANGE_COMMAND_H