#ifndef __STM8_UART_NONBLOCKING_H
#define __STM8_UART_NONBLOCKING_H

#include <stm8/uart/uart_base.h>
#include <utils/ring_buffer.h>

namespace stm8 {

    //UART class with interrupt handlers and FIFO buffers
    template <UartType type, size_t buffer_size>
    class UartNonBlocking : public UartBase<type> {
    private:
        typedef UartBase<type> uart_base;
        using UartBase<type>::UARTx;
        using UartBase<type>::error;

        typedef utils::ring_buffer<uint8_t, buffer_size> uart_buffer_type;
        typedef typename uart_buffer_type::size_type size_type;

        uart_buffer_type uart_rx_buffer;
        uart_buffer_type uart_tx_buffer;

        volatile bool uart_rx_fifo_not_empty_flag;         // data has been received flag
        volatile bool uart_rx_fifo_ovf_flag;               // this flag is not automatically cleared by the software buffer

        volatile bool uart_tx_fifo_full_flag;              // this flag is automatically set and cleared by the software buffer
        volatile bool uart_tx_fifo_ovf_flag;               // this flag is not automatically cleared by the software buffer

        void reset_buffers() {
            uart_rx_buffer.flush();
            uart_tx_buffer.flush();
        }

    public:
        UartNonBlocking() {
            uart_rx_fifo_not_empty_flag = false;
            uart_rx_fifo_ovf_flag = false;
            uart_tx_fifo_full_flag = false;
            uart_tx_fifo_ovf_flag = false;
        }

        //Enables interrupts on data receive and clear buffers
        void start() {
            uart_base::start_receive();
            reset_buffers();
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
        bool get_byte(uint8_t& byte) {
            if(!uart_rx_buffer.pop(byte)) {                 // if data exists in the sw buffer - grab the oldest element in the buffer
                uart_rx_fifo_not_empty_flag = 0;           // RX sw buffer is empty. Clear the rx flag
                return false;
            }
            return true;
        }

        // UART data transmit function
        //  - checks if there's room in the transmit sw buffer
        //  - if there's room, it transfers data byte to sw buffer
        //  - automatically handles "uart_tx_fifo_full_flag"
        //  - sets the overflow flag upon software buffer overflow (doesn't overwrite existing data)
        //  - it enables the "hw buffer empty" interrupt
        bool send_byte(uint8_t byte) {
            bool result = uart_tx_buffer.push(byte);         // transfer data byte to sw buffer
            if(!result) {                                    // no room in the sw buffer
                uart_tx_fifo_ovf_flag = true;                // set the overflow flag
            }
            if(uart_tx_buffer.is_full()) {                   // if sw buffer is full
                uart_tx_fifo_full_flag = true;               // set the TX FIFO full flag
            }

            //enable interrupts
            uart_base::enable_transmitter();

            return result;
        }

        // Writes data into the transmitter's buffer.
        // If there is not enough space in the buffer to write all data,
        // writes parts of them and return the actual written size.
        size_type send_data(const uint8_t* data, size_type size) {
            size_type result = uart_tx_buffer.push(data, size);            // transfer data bytes to sw buffer
            if(result != size) {                             // no room in the sw buffer
                uart_tx_fifo_ovf_flag = true;                // set the overflow flag
            }
            if(uart_tx_buffer.is_full()) {                   // if sw buffer is full
                uart_tx_fifo_full_flag = true;               // set the TX FIFO full flag
            }

            //enable interrupts
			uart_base::enable_transmitter();

            return result;
        }


        //These methods are called from interupt handlers
        void handle_byte_receive() {
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
                    uart_rx_fifo_ovf_flag = true;                  // set the overflow flag
                } else {                                           // if there's room in the sw buffer
                    uart_rx_buffer.push(data);                     // store the received data as the newest data element in the sw buffer
                }
                //Clear interrupt source.
                //It is for IAR simulator, because reading of DR register clears RXNE flag
                //UARTx->SR &= ~UartConstants::UART_SR_RXNE;
            }
            error.error_flags = last_error.error_flags;
        }

        void handle_byte_transmit() {
            if (UARTx->SR & UartConstants::UART_SR_TXE) {
                if(uart_tx_fifo_full_flag) {              // if the sw buffer is full
                    uart_tx_fifo_full_flag = false;       // clear the buffer full flag because we are about to make room
                }
                uint8_t byte;
                if(uart_tx_buffer.pop(byte)) {        // if data exists in the sw buffer
                    UARTx->DR = byte;                     // place oldest data element in the TX hardware buffer
                }
                if(uart_tx_buffer.empty()) {           // if no more data exists
                    uart_base::disable_transmitter();
                }
                //It is for IAR simulator
                //UARTx->SR &= ~UartConstants::UART_SR_TXE;
            }
        }

        void handle_rx_overflow() {
            // check for rx overflow condition
            if(uart_rx_fifo_ovf_flag) {
                /* handle rx overflow condition as desired */
                uart_rx_fifo_ovf_flag = false;  // clear the rx overflow flag
            }
        }

        void handle_tx_overflow() {
            // check for tx overflow condition
            if(uart_tx_fifo_ovf_flag) {
                /* handle tx overflow condition as desired */
                uart_tx_fifo_ovf_flag = 0;  // clear the tx overflow flag
            }
        }

    };

}

#endif // __STM8_UART_NONBLOCKING_H
