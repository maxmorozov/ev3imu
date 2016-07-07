#ifndef __EV3_COMMANDS_NAME_COMMAND_H
#define __EV3_COMMANDS_NAME_COMMAND_H

#include <ev3/ev3_uart.h>
#include <ev3/commands/checksum.h>
#include <mpl/integral_c.h>
#include <mpl/fold.h>

namespace ev3 {
namespace commands {

    namespace details {
        //Initial state for type list folding
        struct root {
        };

        //Storage class that puts the character value into a buffer
        template <typename Item>
        struct char_item {
            char c;

            char_item() : c(Item::value) {}
        };

        //Metafunction to generate character buffer
        template <typename State, typename Item>
        struct StringBuilder {
            struct type: State, char_item<Item> {
            };
        };

        //Specialization for the terminal item
        template <typename Item>
        struct StringBuilder<root, Item> {
            typedef char_item<Item> type;
        };

        //Metafunction to calculate checksum of the character sequence
        template <typename State, typename Item>
        struct CheckSumCalc {
            struct type {
                static const uint8_t value = State::value ^ Item::value;
            };
        };

        template <typename T, size_t padding_size>
        struct data_with_padding {
            T data;
            char padding[padding_size];
        };

        template <typename T>
        struct data_with_padding<T, 0> {
            T data;
        };
    }


    //Info message for mode name descriptor
    //This command should be the first command in the mode descriptor.
    template <uint8_t mode, typename CharSequence>
    class NameCommand {
    private:
        typedef typename mpl::fold<CharSequence, details::root, details::StringBuilder>::type data_type;

        static const uint8_t buffer_size = mpl::clp2<sizeof(data_type)>::value;
        static const uint8_t cmd = EV3Command::info<mode, buffer_size>::value;
        static const uint8_t info_byte = UartProtocol::InfoByte::NAME;

        //The string buffer size should be a power of 2, so we add zero bytes after the string value
        static const uint8_t padding_size = buffer_size - sizeof(data_type);

        //Calculate checksum only for the string content because padding zeros
        //dont not affect the checksum value, because checkum is calulates using
        //simple xor operations
        typedef typename mpl::fold<
            CharSequence,
            mpl::integral_c<uint8_t, ev3::commands::check_sum<cmd, info_byte>::value>,
            details::CheckSumCalc
        >::type checksum_type;

        uint8_t command;
        uint8_t infoByte;
        details::data_with_padding<data_type, padding_size> str_data;
        uint8_t checksum;
    public:
        NameCommand()
            : command(cmd), infoByte(info_byte), checksum(checksum_type::value)
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


#endif //__EV3_COMMANDS_NAME_COMMAND_H