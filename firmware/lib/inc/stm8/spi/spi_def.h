#ifndef __STM8_SPI_DEF_H
#define __STM8_SPI_DEF_H

namespace stm8 {
    //SPI BaudRate Prescaler (BR bits value)
    enum SpiPrescaler
    {
        SpiPrescaler_2   = 0,    // SPI frequency = frequency(CPU)/2
        SpiPrescaler_4   = 1,    // SPI frequency = frequency(CPU)/4
        SpiPrescaler_8   = 2,    // SPI frequency = frequency(CPU)/8
        SpiPrescaler_16  = 3,    // SPI frequency = frequency(CPU)/16
        SpiPrescaler_32  = 4,    // SPI frequency = frequency(CPU)/32
        SpiPrescaler_64  = 5,    // SPI frequency = frequency(CPU)/64
        SpiPrescaler_128 = 6,    // SPI frequency = frequency(CPU)/128
        SpiPrescaler_256 = 7     // SPI frequency = frequency(CPU)/256
    };

    //SPI master/slave mode (MSTR bit value)
    enum SpiMode
    {
        SpiModeSlave  = 0,
        SpiModeMaster = 1
    };

    //SPI Frame Format: MSB or LSB transmitted first (LSBFIRST bit value)
    enum SpiFirstBit
    {
        SpiFirstBitMsb = 0,      //MSB bit will be transmitted first
        SpiFirstBitLsb = 1       //LSB bit will be transmitted first
    };


    //SPI Clock Polarity (CPOL bit value)
    enum SpiClockPolarity
    {
        SpiClockPolarityLow = 0, //Clock to 0 when idle
        SpiClockPolarityHigh = 1 //Clock to 1 when idle
    };

    //SPI Clock Phase (CPHA bit value)
    enum SpiClockPhase
    {
        SpiClockPhase1Edge = 0,  //The first clock transition is the first data capture edge
        SpiClockPhase2Edge = 1   //The second clock transition is the first data capture edge
    };

    //SPI data direction mode (BDM, BDOE, RXONLY bits values)
    //
    //Full Duplex:
    //  Master MISO <- Slave MISO
    //  Master MOSI -> Slave MOSI
    //
    //RxOnly Master:
    //  Master MISO <- Slave MISO
    //RxOnly Slave:
    //  Master MOSI -> Slave MOSI
    //
    //Simplex:
    //  Master MOSI <-> Slave MISO
    //Simplex Master: BDOE=1, data flow M=>S
    //Simplex Slave: BDOE=0, data flow M<=S
    enum SpiDataDirection
    {
        SpiDataDirectionDuplex     = 0x00,  //2-line full duplex
        SpiDataDirectionRxOnly     = 0x04,  //Receiver only: MISO <- MISO for RxOnly Master, MOSI -> MOSI for RxOnly Slave
        SpiDataDirectionSimplexRx  = 0x80,  //Receiver only in 1 line bi-directional data mode (MOSI <- MISO)
        SpiDataDirectionSimplexTx  = 0xC0   //Transmit only in 1 line bi-directional data mode (MOSI -> MISO)
    };

    //SPI Slave Select management (LSBFIRST bit value)
    enum SpiNSS
    {
        SpiNssSoft = 1,
        SpiNssHard = 0
    };

    //PI_IT possible values
    //Elements values convention: 0xYX
    //  X: Position of the corresponding Interrupt
    //  Y: ITPENDINGBIT position
    enum SpiInterrupt
    {
        SpiWakeUp      = (uint8_t)0x34,  // Wake-up interrupt
        SpiOverrun     = (uint8_t)0x65,  // Overrun interrupt
        SpiModeFault   = (uint8_t)0x55,  // Mode fault interrupt
        SpiCrcError    = (uint8_t)0x45,  // CRC error interrupt
        SpiTXE         = (uint8_t)0x17,  // Transmit buffer empty interrupt
        SpiRXNE        = (uint8_t)0x06,  // Receive buffer not empty interrupt
        SpiError       = (uint8_t)0x05   // Error interrupt
    };

    template <SpiInterrupt interrupt>
    struct spi_interrupt_trait
    {
        static const uint8_t mask = 1 << (interrupt & 0x0F);
    };

}


#endif //__STM8_SPI_DEF_H