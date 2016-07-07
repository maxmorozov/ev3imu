#ifndef __STM8_SPI_CONFI_H
#define __STM8_SPI_CONFI_H

#include <stm8/spi/spi_def.h>

namespace stm8 {

    template <
        SpiPrescaler prescaler,
        SpiMode mode,
        SpiFirstBit firstBit,
        SpiClockPolarity clockPolarity,
        SpiClockPhase clockPhase,
        SpiDataDirection dataDirection,
        SpiNSS slaveManagement,
        uint8_t CRCPolynomial>
    class spi_config {
    private:
        static const uint8_t CR1 =
            clockPhase |
            (clockPolarity << 1) |
            (prescaler << 3) |
            (firstBit << 7);

        static const uint8_t CR2 =
            (slaveManagement << 1) |
            (dataDirection << 2) |
            (mode == SpiModeMaster ? SPI_CR2_SSI : 0);

        static const uint8_t modeValue = mode << 2;

    public:
        static void reset() {
            ::SPI()->CR1    = SPI_CR1_RESET_VALUE;
            ::SPI()->CR2    = SPI_CR2_RESET_VALUE;
            ::SPI()->ICR    = SPI_ICR_RESET_VALUE;
            ::SPI()->SR     = SPI_SR_RESET_VALUE;
            ::SPI()->CRCPR  = SPI_CRCPR_RESET_VALUE;
        }

        static void configure() {
            ::SPI()->CR1 = CR1;
            ::SPI()->CR2 = CR2;

            //Master/Slave mode configuration
            ::SPI()->CR1 |= modeValue;
            //CRC configuration
            ::SPI()->CRCPR = CRCPolynomial;
        }
    };
}


#endif //__STM8_SPI_CONFI_H