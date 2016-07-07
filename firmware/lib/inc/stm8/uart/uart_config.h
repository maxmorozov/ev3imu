#ifndef __STM8_UART_CONFIG_H
#define __STM8_UART_CONFIG_H

#include <stm8_target.h>

namespace stm8 {

    enum UartType {
        Uart1,
        Uart2,
        Uart3,
        Uart4
    };

    /**
      * @brief  UART Word length possible values
      */
    enum UartWordSize {
        UART_WORD_8 = (uint8_t)0x00, // 8 bits Data
        UART_WORD_9 = (uint8_t)0x10  // 9 bits Data
    };

    /**
      * @brief  UART stop bits possible values
      */
    enum UartStopBits {
        UART_STOPBITS_1   = (uint8_t)0x00,    // One stop bit is  transmitted at the end of frame
        UART_STOPBITS_0_5 = (uint8_t)0x10,    // Half stop bits is transmitted at the end of frame
        UART_STOPBITS_2   = (uint8_t)0x20,    // Two stop bits are  transmitted at the end of frame
        UART_STOPBITS_1_5 = (uint8_t)0x30     // One and half stop bits
    };

    /**
      * @brief  UART parity possible values
      */
    enum UartParity {
        UART_PARITY_NO     = (uint8_t)0x00,      // No Parity
        UART_PARITY_EVEN   = (uint8_t)0x04,      // Even Parity
        UART_PARITY_ODD    = (uint8_t)0x06       // Odd Parity
    };

    /**
      * @brief  UART Mode possible values
      */
    enum UartMode {
        UART_MODE_RX_ENABLE     = (uint8_t)0x08,  // Receive Enable
        UART_MODE_TX_ENABLE     = (uint8_t)0x04,  // Transmit Enable
        UART_MODE_TX_DISABLE    = (uint8_t)0x80,  // Transmit Disable
        UART_MODE_RX_DISABLE    = (uint8_t)0x40,  // Single-wire Half-duplex mode
        UART_MODE_TXRX_ENABLE   = (uint8_t)0x0C   // Transmit Enable and Receive Enable
    };

    /**
      * @brief  UART1 Synchrone modes
      */
    enum UartSyncMode {
        UART_SYNCMODE_CLOCK_DISABLE    = (uint8_t)0x80, // Sync mode Disable, SLK pin Disable
        UART_SYNCMODE_CLOCK_ENABLE     = (uint8_t)0x08, // Sync mode Enable, SLK pin Enable
        UART_SYNCMODE_CPOL_LOW         = (uint8_t)0x40, // Steady low value on SCLK pin outside transmission window
        UART_SYNCMODE_CPOL_HIGH        = (uint8_t)0x04, // Steady high value on SCLK pin outside transmission window
        UART_SYNCMODE_CPHA_MIDDLE      = (uint8_t)0x20, // SCLK clock line activated in middle of data bit
        UART_SYNCMODE_CPHA_BEGINING    = (uint8_t)0x02, // SCLK clock line activated at beginning of data bit
        UART_SYNCMODE_LASTBIT_DISABLE  = (uint8_t)0x10, // The clock pulse of the last data bit is not output to the SCLK pin
        UART_SYNCMODE_LASTBIT_ENABLE   = (uint8_t)0x01  // The clock pulse of the last data bit is output to the SCLK pin
    };

    template<UartType uartType> struct uart_traits;

    template<> struct uart_traits<Uart1>
    {
        typedef UART1_TypeDef uart_type;

        enum { UartBase = UART1_BaseAddress };
    };

    template<> struct uart_traits<Uart2>
    {
        typedef UART2_TypeDef uart_type;

        enum { UartBase = UART2_BaseAddress };
    };

    template<> struct uart_traits<Uart3>
    {
        typedef UART3_TypeDef uart_type;

        enum { UartBase = UART3_BaseAddress };
    };

#if defined(STM8AF622x)
    template<> struct uart_traits<Uart4>
    {
        typedef UART4_TypeDef uart_type;

        enum { UartBase = UART4_BaseAddress };
    };
#endif

    //I put all UART constant here to be able to work with them despite of the available UARTs
    struct UartConstants {
        static const uint8_t UART_SR_TXE = 0x80;     // Transmit Data Register Empty mask
        static const uint8_t UART_SR_TC = 0x40;      // Transmission Complete mask
        static const uint8_t UART_SR_RXNE = 0x20;    // Read Data Register Not Empty mask
        static const uint8_t UART_SR_IDLE = 0x10;    // IDLE line detected mask
        static const uint8_t UART_SR_OR = 0x08;      // OverRun error mask
        static const uint8_t UART_SR_NF = 0x04;      // Noise Flag mask
        static const uint8_t UART_SR_FE = 0x02;      // Framing Error mask
        static const uint8_t UART_SR_PE = 0x01;      // Parity Error mask

        static const uint8_t UART_SR_ERRORS_MASK = 0x0F; //Allows to copy all errors from the status register at once

        static const uint8_t UART_BRR1_DIVM = 0xFF;  // LSB mantissa of UARTDIV [7:0] mask

        static const uint8_t UART_BRR2_DIVM = 0xF0;  // MSB mantissa of UARTDIV [11:8] mask
        static const uint8_t UART_BRR2_DIVF = 0x0F;  // Fraction bits of UARTDIV [3:0] mask

        static const uint8_t UART_CR1_R8 = 0x80;     // Receive Data bit 8
        static const uint8_t UART_CR1_T8 = 0x40;     // Transmit data bit 8
        static const uint8_t UART_CR1_UARTD = 0x20;  // UART Disable (for low power consumption)
        static const uint8_t UART_CR1_M = 0x10;      // Word length mask
        static const uint8_t UART_CR1_WAKE = 0x08;   // Wake-up method mask
        static const uint8_t UART_CR1_PCEN = 0x04;   // Parity Control Enable mask
        static const uint8_t UART_CR1_PS = 0x02;     // UART Parity Selection
        static const uint8_t UART_CR1_PIEN = 0x01;   // UART Parity Interrupt Enable mask

        static const uint8_t UART_CR2_TIEN = 0x80;   // Transmitter Interrupt Enable mask
        static const uint8_t UART_CR2_TCIEN = 0x40;  // Transmission Complete Interrupt Enable mask
        static const uint8_t UART_CR2_RIEN = 0x20;   // Receiver Interrupt Enable mask
        static const uint8_t UART_CR2_ILIEN = 0x10;  // IDLE Line Interrupt Enable mask
        static const uint8_t UART_CR2_TEN = 0x08;    // Transmitter Enable mask
        static const uint8_t UART_CR2_REN = 0x04;    // Receiver Enable mask
        static const uint8_t UART_CR2_RWU = 0x02;    // Receiver Wake-Up mask
        static const uint8_t UART_CR2_SBK = 0x01;    // Send Break mask

        static const uint8_t UART_CR3_LINEN = 0x40;  // Alternate Function output mask
        static const uint8_t UART_CR3_STOP = 0x30;   // STOP bits [1:0] mask
        static const uint8_t UART_CR3_CKEN = 0x08;   // Clock Enable mask
        static const uint8_t UART_CR3_CPOL = 0x04;   // Clock Polarity mask
        static const uint8_t UART_CR3_CPHA = 0x02;   // Clock Phase mask
        static const uint8_t UART_CR3_LBCL = 0x01;   // Last Bit Clock pulse mask

        static const uint8_t UART_CR4_LBDIEN = 0x40; // LIN Break Detection Interrupt Enable mask
        static const uint8_t UART_CR4_LBDL = 0x20;   // LIN Break Detection Length mask
        static const uint8_t UART_CR4_LBDF = 0x10;   // LIN Break Detection Flag mask
        static const uint8_t UART_CR4_ADD = 0x0F;    // Address of the UART node mask

        static const uint8_t UART_CR5_SCEN = 0x20;   // Smart Card Enable mask
        static const uint8_t UART_CR5_NACK = 0x10;   // Smart Card Nack Enable mask
        static const uint8_t UART_CR5_HDSEL = 0x08;  // Half-Duplex Selection mask
        static const uint8_t UART_CR5_IRLP = 0x04;   // Irda Low Power Selection mask
        static const uint8_t UART_CR5_IREN = 0x02;   // Irda Enable mask

        static const uint8_t UART_CR6_LDUM = 0x80;   // LIN Divider Update Method
        static const uint8_t UART_CR6_LSLV = 0x20;   // LIN Slave Enable
        static const uint8_t UART_CR6_LASE = 0x10;   // LIN Auto synchronization Enable
        static const uint8_t UART_CR6_LHDIEN = 0x04; // LIN Header Detection Interrupt Enable
        static const uint8_t UART_CR6_LHDF = 0x02;   // LIN Header Detection Flag
        static const uint8_t UART_CR6_LSF = 0x01;    // LIN Synch Field

        //Reset values
        static const uint8_t UART_SR_RESET_VALUE = 0xC0;
        static const uint8_t UART_BRR1_RESET_VALUE = 0x00;
        static const uint8_t UART_BRR2_RESET_VALUE = 0x00;
        static const uint8_t UART_CR1_RESET_VALUE = 0x00;
        static const uint8_t UART_CR2_RESET_VALUE = 0x00;
        static const uint8_t UART_CR3_RESET_VALUE = 0x00;
        static const uint8_t UART_CR4_RESET_VALUE = 0x00;
        static const uint8_t UART_CR5_RESET_VALUE = 0x00;
        static const uint8_t UART_GTR_RESET_VALUE = 0x00;
        static const uint8_t UART_PSCR_RESET_VALUE = 0x00;
    };

    //UART configuration
    template <
        uint32_t cpuClock,
        uint32_t baudRate,
        UartWordSize word = UART_WORD_8,
        UartStopBits stopBits = UART_STOPBITS_1,
        UartParity parity = UART_PARITY_NO,
        UartMode mode = UART_MODE_TXRX_ENABLE,
        UartSyncMode sync_mode = UART_SYNCMODE_CLOCK_DISABLE
    >
    struct UartConfig {
    private:
        //Rounding to the nearest integer number
        static const uint32_t uart_div = (cpuClock + baudRate / 2) / baudRate;

    public:
        static const uint32_t speed = baudRate;

        static const UartSyncMode syncMode = sync_mode;

        // Set the LSB mantissa of UART DIV
        static const uint8_t BBR1_value = (uint8_t)(uart_div >> 4);

        // Set the fraction of UART DIV
        static const uint8_t BBR2_value = (uart_div & 0x0f) | ((uart_div >> 8) & 0xF0);

        //Clock Polarity, lock Phase, Last Bit Clock pulse
        static const uint8_t CR3_mask1 = UartConstants::UART_CR3_CPOL | UartConstants::UART_CR3_CPHA | UartConstants::UART_CR3_LBCL;
        //Clock Enable bit
        static const uint8_t CR3_mask2 = UartConstants::UART_CR3_CKEN;

        static const uint8_t tx_enable = mode & UART_MODE_TX_ENABLE ? UartConstants::UART_CR2_TEN : 0;
        static const uint8_t rx_enable = mode & UART_MODE_RX_ENABLE ? UartConstants::UART_CR2_REN : 0;

        static const uint8_t CR1_value = (uint8_t)word | (uint8_t)parity;
        static const uint8_t CR2_value = tx_enable | rx_enable;
        static const uint8_t CR3_value1 = stopBits | (syncMode & CR3_mask1);
        static const uint8_t CR3_value2 = syncMode & UartConstants::UART_CR3_CKEN;
    };


}

#endif // __STM8_UART_CONFIG_H
