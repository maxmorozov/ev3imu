#ifndef __SENSORS_SPI_TRANSPORT_H
#define __SENSORS_SPI_TRANSPORT_H

namespace sensors {


    //Base class contains core SPI read/write methods.
    //It was extracted from SpiTransport to reduce code blowing
    template <typename Spi, typename AddressStrategy>
    class SpiTransportBase {
        static const uint8_t READ_MASK = 0x80;
        static const uint8_t AUTO_INCREMENT_MASK = 0x40;
    private:
        //Selects significant part of the address
        static uint8_t normalize(uint8_t address) {
            return AddressStrategy::normalize(address);
        }

        //Sets auto increment bit of address if it is necessary
        static uint8_t auto_increment(uint8_t count) {
            return AddressStrategy::auto_increment(count);
        }
    protected:
        //reads one byte by the specified address
        uint8_t readByte(uint8_t address) const {
            // To indicate a read, set bit 7 (msb) to 1
            // If we're reading multiple bytes, set bit 6 to 1 to auto increment the address
            // The remaining six bits are the address to be read
            Spi::transaction(READ_MASK | normalize(address));
            return Spi::transaction(0);
        }

        //reads the specified count of bytes starting from the specified address
        uint8_t readBytes(uint8_t address, uint8_t* dest, uint8_t count) const {
            // To indicate a read, set bit 7 (msb) to 1
            // If we're reading multiple bytes, set bit 6 to 1 to auto increment the address
            // The remaining six bits are the address to be read
            Spi::transaction(READ_MASK | auto_increment(count) | normalize(address));

            for (uint8_t i = 0; i < count; ++i) {
                dest[i] = Spi::transaction(0); // Read into the destination array
            }

            return count;
        }

        //Writes one byte to the sensor by the address
        void writeByte(uint8_t address, uint8_t value) {
            // If write, bit 7 (MSB) should be 0
            // If single write, bit 6 should be 0
            Spi::transaction(normalize(address));
            Spi::transaction(value);
        }

        //Writes multiple bytes to the sensor starting from the address
        void writeBytes(uint8_t address, const uint8_t* data, uint8_t count) {
            // If write, bit 7 (MSB) should be 0
            // If single write, bit 6 should be 0
            Spi::transaction(auto_increment(count) | normalize(address));

            for (uint8_t i = 0; i < count; ++i) {
                Spi::transaction(data[i]); // Write the data
            }
        }
    };



    template <typename Spi, typename SelectPin, typename AddressStrategy>
    class SpiTransport : public SpiTransportBase<Spi, AddressStrategy> {
        typedef SpiTransportBase<Spi, AddressStrategy> base_type;
    private:
        //Automatically enables and disables SPI communication with the slave chip
        struct ChipSelector {
            SelectPin selector;

            ChipSelector() {
                selector.on();
            }

            ~ChipSelector() {
                selector.off();
            }
        };

    public:
        //reads one byte by the specified address
        uint8_t readByte(uint8_t address) const {
            ChipSelector cs;

            return base_type::readByte(address);
        }

        //reads the specified count of bytes starting from the specified address
        uint8_t readBytes(uint8_t address, uint8_t* dest, uint8_t count) const {
            ChipSelector cs;

            return base_type::readBytes(address, dest, count);
        }

        //Writes one byte to the sensor by the address
        void writeByte(uint8_t address, uint8_t value) {
            ChipSelector cs;

            base_type::writeByte(address, value);
        }

        //Writes multiple bytes to the sensor starting from the address
        void writeBytes(uint8_t address, const uint8_t* data, uint8_t count) {
            ChipSelector cs;

            base_type::writeBytes(address, data, count);
        }

        // register bit modification functions
        void setBits(uint8_t address, uint8_t mask) {
            //We need to select/deselect the slave device for each read/write operation
            //Otherwise the write operation will not be performed
            uint8_t value = readByte(address);
            writeByte(address, value | mask);
        }

        INLINE void clearBits(uint8_t address, uint8_t mask) {
            //We need to select/deselect the slave device for each read/write operation
            //Otherwise the write operation will not be performed
            uint8_t value = readByte(address);
            writeByte(address, value & ~mask);
        }

        void writeByteWithMask(uint8_t address, uint8_t value, uint8_t mask) {
            //We need to select/deselect the slave device for each read/write operation
            //Otherwise the write operation will not be performed
            uint8_t readValue = readByte(address);
            writeByte(address, (readValue & ~mask) | (value & mask));
        }
    };
}


#endif //__SENSORS_SPI_TRANSPORT_H