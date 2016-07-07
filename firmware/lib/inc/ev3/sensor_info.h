#ifndef __EV3_SENSOR_INFO_H
#define __EV3_SENSOR_INFO_H

#include <stdint.h>

#include <mpl/filter.h>
#include <mpl/length.h>
#include <mpl/fold.h>

#include <ev3/sensor_types.h>

#include <ev3/commands/type_command.h>
#include <ev3/commands/modes_command.h>
#include <ev3/commands/speed_command.h>
#include <ev3/commands/range_command.h>
#include <ev3/commands/name_command.h>
#include <ev3/commands/format_command.h>

namespace ev3 {

    namespace details {
        //Initial state for type list folding
        struct root {
            //Mode index
            static const uint8_t index = 0;
        };

        //Storage class that assembles a sensor mode descriptor
        template <typename Item, uint8_t index>
        struct ModeDescriptor {
            const ev3::commands::NameCommand<index, typename Item::name> name;
            const ev3::commands::RangeCommand<ev3::UartProtocol::InfoByte::RAW, index, Item::rawMin, Item::rawMax> raw;
            const ev3::commands::RangeCommand<ev3::UartProtocol::InfoByte::SI, index, Item::rawMin, Item::rawMax> si;
            const ev3::commands::FormatCommand<index, Item::sampleSize, Item::dataType, Item::figures, Item::decimals> format;
        };

        //Metafunction to generate sensor mode descriptors list
        template <typename State, typename Item>
        struct ListBuilder {
            struct type: ModeDescriptor<Item, State::index>, State {
                //Next mode index
                static const uint8_t index = State::index + 1;
            };
        };
    }

	/**
	* Represent a specific sensor mode
	**/
    template <
        typename CharSequence,
        uint8_t sample_size,
        DataType data_type,
        uint8_t figures_,
        uint8_t decimals_,
        bool view_,
        int32_t raw_min,
        int32_t raw_max
    >
	struct SensorMode {
		typedef CharSequence name;                               // The mode name
		static const uint8_t sampleSize = sample_size;           // The number of samples
		static const DataType dataType = data_type;              // The data type 0= 8bits, 1=16 bits, 2=32 bits, 3=float
		static const uint8_t figures = figures_;                 // Number of significant digits
		static const uint8_t decimals = decimals_;               // Number of decimal places
		static const bool view = view_;                          // Indicates if this mode is a view
		static const int32_t rawMin = raw_min;                   // Raw minimum value      (e.c. 0)
		static const int32_t rawMax = raw_max;                   // Raw maximum value      (e.c. 1023)
    };


    //Creates the buffer that contains sensor descriptor
    template <uint8_t typeId, uint32_t uartSpeed, typename ModeList>
    struct SensorInfo {
        template <typename T>
        struct view_predicate {
            static const bool value = T::view;
        };

        typedef typename mpl::filter<ModeList, view_predicate>::type VisibleModeList;

        typedef typename mpl::fold<ModeList, details::root, details::ListBuilder>::type mode_list_type;

        //Type identifier
        ev3::commands::TypeCommand<typeId> type;
        //Total number of modes and number of visible modes
        ev3::commands::ModesCommand<mpl::length<ModeList>::value, mpl::length<VisibleModeList>::value> modes;
        //UART speed bits per second
        ev3::commands::SpeedCommand<uartSpeed> speed;
        //List of sensor modes
        mode_list_type mode_list;

        //Acknowledge command
        uint8_t ack;

        SensorInfo() : ack(UartProtocol::BYTE_ACK) {}

        operator const uint8_t*() const {
            return type;
        }

        uint8_t size() const {
            return sizeof(*this);
        }
    };

}


#endif //__EV3_SENSOR_INFO_H
