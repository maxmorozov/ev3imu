#ifndef __STM8_SPI_H
#define __STM8_SPI_H

#include <stm8/spi/spi_config.h>

namespace stm8 {


    //Polling implementation of SPI interface
    //Interrupt based implementation is meaningful if CPU clock is much higher than SPI clock.
    template <typename Config>
    class SPI
    {
    public:
        INLINE static void init() {
            Config::reset();
			Config::configure();
		}

        // Enable the SPI peripheral
        static void enable() {
            ::SPI()->CR1 |= SPI_CR1_SPE;
        }

        // Disable the SPI peripheral
        static void disable() {
            ::SPI()->CR1 &= (uint8_t)(~SPI_CR1_SPE);
        }

        // Enable interrupt
        template <SpiInterrupt interrupt>
        static void enableInterrupt(spi_interrupt_trait<interrupt> it_trait) {
            ::SPI()->ICR |= spi_interrupt_trait<interrupt>::mask;
        }

        // Enable interrupt
        template <SpiInterrupt interrupt>
        static void disableInterrupt(spi_interrupt_trait<interrupt> it_trait) {
            ::SPI()->ICR &= ~spi_interrupt_trait<interrupt>::mask;
        }

        //Send and receive the data through the SPI peripheral.
        //Synchronous operation.
        //Sync mode allows to reach better performance in high-speed SPI modes.
        //See: https://github.com/loganb/SPI/tree/async_spi
        static uint8_t transaction(uint8_t data) {
            // Wait for moving the data from DR register to the shift register to transmin
            while ((::SPI()->SR & SPI_FLAG_TXE) == RESET) { ; }

            // Send a byte
            ::SPI()->DR = data;

            // Waiting for putting incoming data into DR register
            while ((::SPI()->SR & SPI_FLAG_RXNE) == RESET) { ; }

            // Return the data in the DR register
            return ::SPI()->DR;

        }


    };
}

#endif //__STM8_SPI_H
