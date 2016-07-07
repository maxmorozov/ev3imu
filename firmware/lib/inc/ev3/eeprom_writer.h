#ifndef __EV3_EEPROM_WRITER_H
#define __EV3_EEPROM_WRITER_H

#include <stdint.h>

namespace ev3 {

    //Eeprom buffer that keeps data sent from EV3 host
    //DeviceData type should have following interface:
    // struct DeviceData {
    //     void assign(const uint8_t* data, uint8_t length);
    //     uint8_t capacity() const; //buffer capacity in bytes
    //     operator uint8_t*();
    // };
    template <typename DeviceData>
    class EepromWriter {
    private:
        DeviceData buffer;
        uint8_t data_size;

        bool empty() const {
            return data_size == 0;
        }

        void reset() {
            data_size = 0;
        }

    public:
        //Writes the new EEPROM data into internal buffer
        void write(const uint8_t* data, uint8_t size) {
            if (empty() && size >= sizeof(buffer)) {
                data_size = buffer.capacity();
                buffer.assign(data, data_size);
            }
        }

        //Pass the internal data into the callback to
        //write the data into EEPROM
        template <typename Functor>
        void updateEeprom(Functor f) {
            f((uint8_t*)buffer, data_size);
            reset();
        }
    };
}


#endif //__EV3_EEPROM_WRITER_H