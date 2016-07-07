#ifndef __STM8_UART_BASE_H
#define __STM8_UART_BASE_H

#include <stm8/config.h>
#include <stm8/uart/uart_config.h>
#include <utils/ring_buffer.h>

namespace stm8 {

    //This structure combines Uart error flags in the one byte
    struct UartError {
        union {
            struct {
                uint8_t parity_error : 1;
                uint8_t framing_error : 1;
                uint8_t noise : 1;
                uint8_t overrun : 1;
                uint8_t fifo_buffer_overflow : 1;
            };
            uint8_t error_flags;
        };
    };

    //UART class with interrupt handlers and FIFO buffers
    template <UartType type>
    class UartBase {
    protected:
        template <typename Base> friend class isr_ring_buffer;

        typedef typename uart_traits<type>::uart_type uart_type;

        enum { UartAddress = uart_traits<type>::UartBase };

        struct UART
        {
            uart_type* operator-> () { return (uart_type*)UartAddress; }
		};

		static UART UARTx;

        static void enable_transmitter() {
            /* enable UART "TX hw buffer empty" interrupt here */
            UARTx->CR2 |= UartConstants::UART_CR2_TIEN;
        }

        static void disable_transmitter() {
            /* Disable the UART Transmit interrupt */
            UARTx->CR2 &= ~UartConstants::UART_CR2_TIEN;
        }

        //This class enables transmit data interrupts generation.
        //We need to add this ability to the buffer::push, because
        //utils::blocking_queue will swicth the control to receiver process
        //just after calling buffer::push.
        //So we need to be sure that receiver is able to process the data.
        //It can do it only if the "transmitter is ready" interrupts are enabled
        template <typename Base>
        class isr_ring_buffer : public Base {
        public:
            typedef typename Base::value_type value_type;
        public:
            bool push(value_type value) {
                bool result = Base::push(value);
                enable_transmitter();
                return result;
            }

            template <typename ConstIterator>
            ConstIterator push(ConstIterator begin, ConstIterator end) {
                ConstIterator result = Base::push(begin, end);
                enable_transmitter();
                return result;
            }
        };

        volatile UartError error;                            // these flags are not automatically cleared by the software buffer

    public:
        UartBase() {
            error.error_flags = 0;
        }

        void reset() {
            /* Clear the Idle Line Detected bit in the status register by a read
            to the UART1_SR register followed by a Read to the UART1_DR register */
            uint8_t dummy = UARTx->SR;
            dummy = UARTx->DR;

            UARTx->BRR2 = UartConstants::UART_BRR2_RESET_VALUE;
            UARTx->BRR1 = UartConstants::UART_BRR1_RESET_VALUE;

            UARTx->CR1 = UartConstants::UART_CR1_RESET_VALUE;
            UARTx->CR2 = UartConstants::UART_CR2_RESET_VALUE;
            UARTx->CR3 = UartConstants::UART_CR3_RESET_VALUE;
            UARTx->CR4 = UartConstants::UART_CR4_RESET_VALUE;
            UARTx->CR5 = UartConstants::UART_CR5_RESET_VALUE;

            UARTx->GTR = UartConstants::UART_GTR_RESET_VALUE;
            UARTx->PSCR = UartConstants::UART_PSCR_RESET_VALUE;
        }

        template <typename Config>
        void configure() {
            //Set the word length and parity bits
            UARTx->CR1 = Config::CR1_value;
            //Set the STOP bits number and the Clock Polarity, lock Phase, Last Bit Clock pulse
            UARTx->CR3 = Config::CR3_value1;

            //Set the UART baud rates in BRR1 and BRR2 registers
            UARTx->BRR2 = Config::BBR2_value;
            UARTx->BRR1 = Config::BBR1_value;
            //Enable transmitter and receiver
            UARTx->CR2 = Config::CR2_value;

            //Set the Clock Enable bit
            if (!(Config::syncMode & UART_SYNCMODE_CLOCK_DISABLE)) {
                UARTx->CR3 |= Config::syncMode & UartConstants::UART_CR3_CKEN;
            }
        }

        template <typename Config>
        void set_speed() {
            //Set the UART baud rates in BRR1 and BRR2 registers
            UARTx->BRR2 = Config::BBR2_value;
            UARTx->BRR1 = Config::BBR1_value;
        }


        //Enables interrupts on data receive
        void start_receive() {
            UARTx->CR2 |= UartConstants::UART_CR2_RIEN;
        }

        //Disables interrupts on data receive
        void stop_receive() {
            UARTx->CR2 &= ~UartConstants::UART_CR2_RIEN;
        }

        void handle_errors() {
            // check for rx overflow condition
            if (error.error_flags != 0) {
                /* handle rx overflow condition as desired */
                error.error_flags = 0;            // clear the rx overflow flag
            }
        }
    };

    template <UartType type>
    typename UartBase<type>::UART UartBase<type>::UARTx;

}

#endif // __STM8_UART_BASE_H
