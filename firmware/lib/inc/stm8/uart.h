#ifndef __STM8_UART_H
#define __STM8_UART_H

#include <os_services.h>
#include <stm8/uart/uart_base.h>
#include <utils/ring_buffer.h>
#include <utils/blocking_queue.h>
#include <utils/inline.h>

namespace stm8 {

    struct EmptyDiagnostic {
        static void receive(uint8_t byte) {}
    };

    //UART class with interrupt handlers and FIFO buffers
    template <UartType type, size_t buffer_size, typename Diagnostic = EmptyDiagnostic>
    class Uart : public UartBase<type> {
    private:
        typedef UartBase<type> uart_base;
        using uart_base::uart_type;
        using uart_base::UARTx;
        using uart_base::error;

        typedef utils::blocking_queue<uint8_t, buffer_size> uart_rx_buffer_type;
        typedef typename uart_rx_buffer_type::size_type size_type;

        uart_rx_buffer_type uart_rx_buffer;

        const uint8_t* tx_iterator;
        const uint8_t* tx_end;

        OS::TEventFlag tx_event;

        void reset_buffers() {
            uart_rx_buffer.flush();
        }

    public:
        INLINE Uart() {
        }

        //Enables interrupts on data receive and clear buffers
        void start() {
            reset_buffers();
            uart_base::start_receive();
        }

        //Disables interrupts
        void stop() {
            uart_base::stop_receive();
        }

        // UART data receive function
        //  - checks if data exists in the receive sw buffer
        //  - if data exists, it returns the oldest element contained in the buffer
        //  - automatically handles "uart_rx_buffer_full_flag"
        //  - if no data exists, it clears the uart_rx_flag
        bool get_byte(uint8_t& byte, timeout_t timeout = 0) {
            return uart_rx_buffer.pop(byte, timeout);
        }

        // UART data transmit function
        // If UART transmitter's buffer does not have ehough space to
        // send all data, it blocks until the buffer is ready to receive new data.
        NOINLINE void send_data(const uint8_t* data, size_type size) {
            tx_iterator = data;
            tx_end = data + size;

            uart_base::enable_transmitter();
            tx_event.wait();
        }

        // UART data transmit function
        //  - checks if there's room in the transmit sw buffer
        //  - if there's room, it transfers data byte to sw buffer
        //  - automatically handles "uart_tx_buffer_full_flag"
        //  - sets the overflow flag upon software buffer overflow (doesn't overwrite existing data)
        //  - if this is the first data byte in the buffer, it enables the "hw buffer empty" interrupt
        void send_byte(uint8_t byte) {
            send_data(&byte, sizeof(byte));
            //send_data(io::buffer(&byte, sizeof(byte)));
        }

        //These methods are called from interupt handlers
        INLINE void handle_byte_receive() {
            uint8_t status = UARTx->SR;
            //This interrupt can be generated in two cases:
            // received data ready to be read.
            // overrun error detected.
            // We need to read data register in both cases.
            uint8_t data = UARTx->DR;
            UartError last_error;
            last_error.error_flags = status & UartConstants::UART_SR_ERRORS_MASK; //copy error flags

            if (status & UartConstants::UART_SR_RXNE) {
                if(uart_rx_buffer.is_full()) {                     // if the sw buffer is full
                    last_error.fifo_buffer_overflow = 1;           // set the overflow flag
                } else {                                           // if there's room in the sw buffer
                    Diagnostic::receive(data);
                    uart_rx_buffer.push_isr(data);                 // store the received data as the newest data element in the sw buffer
                }
                //Clear interrupt source.
                //It is for IAR simulator, because reading of DR register clears RXNE flag
                //UARTx->SR &= ~UartConstants::UART_SR_RXNE;
            }
            error.error_flags = last_error.error_flags;
        }

        INLINE void handle_byte_transmit() {
            if (UARTx->SR & UartConstants::UART_SR_TXE) {
                if (tx_iterator != tx_end) { // if data exists in the sw buffer
                    UARTx->DR = *tx_iterator;    // place oldest data element in the TX hardware buffer
                    ++tx_iterator;
                }
                if(tx_iterator == tx_end) {           // if no more data exists
                    uart_base::disable_transmitter();
                    tx_event.signal_isr();
                }
                //It is for IAR simulator
                //UARTx->SR &= ~UartConstants::UART_SR_TXE;
            }
        }
    };

}

#endif // __STM8_UART_H
