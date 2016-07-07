#ifndef __STM8_SPI_STM8S_H
#define __STM8_SPI_STM8S_H

#include <hal/spi_interface.h>
#include <stm8/spi/spi_config.h>

namespace stm8 {

    //Polling implementation of SPI interface
    //Interrupt based implementation is meaningful if CPU clock is much higher than SPI clock.
    template <typename Config>
    class SPI: public hal::SPI
    {
	protected:
	    static Error checkError(uint8_t statusReg) {
	        Error error = NO_ERROR;

            //TODO - can these error states be combined?

	        //Master Mode fault event
	        if (statusReg & SPI_FLAG_MODF) {
	        	error = MASTER_MODE_FAULT;
	        }
	        //Overrun error
	        if (statusReg & SPI_FLAG_OVR) {
	        	error = OVERRUN_ERROR;
	        }
	        //CRC error flag
	        if (statusReg & SPI_FLAG_CRCERR) {
	        	error = CRC_ERROR;
	        }
	        return error;
	    }

        //Send and receive the data through the SPI peripheral.
        //Synchronous operation.
        //Sync mode allows to reach better performance in high-speed SPI modes.
        //See: https://github.com/loganb/SPI/tree/async_spi
        static Error transaction(uint8_t data, uint8_t& received) {
            // Wait for moving the data from DR register to the shift register to transmin
            uint8_t sr;
            Error error;
            do {
                sr = ::SPI()->SR;
                error = checkError(sr);
                if(error != NO_ERROR)
                    return error;
            } while ((sr & SPI_FLAG_TXE) == 0);

            // Send a byte
            ::SPI()->DR = data;

            // Waiting for getting incoming data into DR register
            do {
                sr = ::SPI()->SR;
                error = checkError(sr);
                if(error != NO_ERROR)
                    return error;
            } while ((sr & SPI_FLAG_RXNE) == 0);

            // Return the data in the DR register
            received = ::SPI()->DR;

            return error;
        }

    public:
        void init() {
            Config::reset();
			Config::configure();
		}


        // Enable the SPI peripheral
        void enable() {
             ::SPI()->CR1 |= SPI_CR1_SPE;
        }

        // Disable the SPI peripheral
        void disable() {
            ::SPI()->CR1 &= (uint8_t)(~SPI_CR1_SPE);
        }

        // Enable interrupt
        template <SpiInterrupt interrupt>
        void enableInterrupt(spi_interrupt_trait<interrupt> it_trait) {
            ::SPI()->ICR |= spi_interrupt_trait<interrupt>::mask;
        }

        // Enable interrupt
        template <SpiInterrupt interrupt>
        void disableInterrupt(spi_interrupt_trait<interrupt> it_trait) {
            ::SPI()->ICR &= ~spi_interrupt_trait<interrupt>::mask;
        }

        //Writes one byte
        Error write(const uint8_t data) {
            uint8_t received;
            return transaction(data, received);
        }
        //Reads one byte, write an optional byte at the same time
        Error read(uint8_t &data, const uint8_t write = 0x00) {
            return transaction(write, data);
        }

        //Writes multiple bytes
        Error writeBuffer(const uint8_t* data, const size_t length) {
            Error error = NO_ERROR;
            uint8_t received;
            for (const uint8_t* end = data + length; data != end && error == NO_ERROR; ++data) {
                error = transaction(*data, received); // Write the data
            }
            return error;
        }
        //Reads multiple bytes, write an optional byte for each byte read operation
        Error readBuffer(uint8_t* data, const size_t length, const uint8_t write = 0x00) {
            Error error = NO_ERROR;
            for (uint8_t* end = data + length; data != end && error == NO_ERROR; ++data) {
                error = transaction(write, *data); // Read into destination array
            }
            return error;
        }
    };
}

#endif //__STM8_SPI_STM8S_H
