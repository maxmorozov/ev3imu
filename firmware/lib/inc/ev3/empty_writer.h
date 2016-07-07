#ifndef __EV3_EMPTY_WRITER_H
#define __EV3_EMPTY_WRITER_H

namespace ev3 {
    //Stub EEPROM writer that does nothing
    struct EmptyEepromWriter {
        //Writes the new EEPROM data into internal buffer
        void write(const uint8_t* data, uint8_t size) {}

        template <typename Functor>
        void updateEeprom(Functor f) const {
        }
    };
}


#endif //__EV3_EMPTY_WRITER_H