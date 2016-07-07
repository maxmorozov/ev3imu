#ifndef __EV3_UART_SENSOR_H
#define __EV3_UART_SENSOR_H

#include <mpl/math.h>
#include <utils/math.h>
#include <utils/copy.h>
#include <stm8/uart.h>
#include <ev3/ev3_uart.h>
#include <ev3/command_info.h>
#include <ev3/sensor_info.h>
#include <ev3/commands/message_command.h>

namespace ev3 {
    /**
     * Implements Lego EV3 UART protocol
     *
     * type - sensor type indentifier
     *        Sensor type ranges:
     *        0           "Don't change type" type
     *        1..49       Reserved for LEGO existing and future devices
     *        50..98      Free to 3th. party devices
     *        99          Reserved for LEGO energy meter
     *        100..127    Reserved for internal use
     *
     * Uart - UART protocol implementation. Should implement following methods (see stm8/uart.h for details):
     *        void reset() {
     *        template <typename Config> void configure();
     *        void start();
     *        void stop();
     *        bool get_byte(uint8_t& byte, timeout_t timeout);
     *        void send_data(const uint8_t* data, size_type size);
     *        void handle_byte_receive();
     *        void handle_byte_receive();
     *
     * Config - UART config contains necessary information to set up communication parameters.
     *          The sensor starts using the fixed UART speed 2400 bps, than is changed to the necessary 
     *          communication speed after getting response from the host. 
     *          The UART config contains necessary information to set up communication parameters.
     *
     * Device - Sensor's implementation. This implementation uses 'Curiously recurring template pattern' to 
     *          allow sensor's core sending samples to the EV3 host.
     */
    template<int type, typename Uart, typename Config, template <typename > class Device>
    class Ev3UartSensor : public Device<Ev3UartSensor<type, Uart, Config, Device> > {
        typedef stm8::UartConfig<F_MASTER, 2400> uart_start_config;

        typedef Device<Ev3UartSensor<type, Uart, Config, Device> > device_type;
        typedef typename device_type::commands commands;

        //Size for sample buffer
        using device_type::sample_size;

        //Commands with fixed-lenth payload.
        //They have been precalculated to simplicity
        enum Commands {
            CMD_TYPE = 0x40,
            CMD_SELECT = 0x43,
            CMD_MODES = 0x49,
            CMD_SPEED = 0x52
        };

        //Reading command payload result
        enum Result {
            Success,
            CRCError,
            Timeout,
            FormatError,  //Payload size exceeds 32 byte
            NotExpectedCommand
        };

        //Sending data from the full buffer (32 bytes) should take 106 ms
        //On the port1 ack time usually is 151 ms, on the port4 it is 171 ms
        static const timeout_t ACK_TIMEOUT = 256; // 256ms
        static const timeout_t HEARTBEAT_PERIOD = 1000; // 1000 ms

        enum State {
            Start,
            Reset,
            Init,
            WaitingForAck,
            SetSpeed,
            WaitingForCommand
        };

    private:
        Uart uart;

        //Reserve the space for data command. The Imu device driver does not
        //manages the data buffer by itself. It requested the space for the
        //data sample from this class.
        uint8_t buffer[mpl::clp2<sample_size>::value + 2];

        State currentState;

        //Sensor descriptor type
        typedef ev3::SensorInfo<type, Config::speed, typename Device<Ev3UartSensor>::mode_list> SensorInfo;

        //Sensor descriptor data buffer that will be sent to the host
        static const SensorInfo sensorInfo;

        //Returns pointer to device implementatio
        device_type* getDevice() {
            return static_cast<device_type*>(this);
        }
        
        //Reads a command from the host
        Result readCommand(uint8_t command, uint8_t* buffer, uint8_t size) {
            uint8_t checksum = 0xff ^ command;
            uint8_t byte_read;
            for (uint8_t pos = 0; pos < size; ++pos) {
                if (uart.get_byte(byte_read, HEARTBEAT_PERIOD)) {
                    checksum ^= byte_read;
                    buffer[pos] = byte_read;
                } else {
                    return Timeout;
                }
            }
            if (uart.get_byte(byte_read, HEARTBEAT_PERIOD)) {
                if (byte_read != checksum) {
                    return CRCError;
                }
            } else {
                return Timeout;
            }

            return Success;
        }

        //Startup communication sequence
        void init() {
            //Setup UART initial configuration
            uart.template configure<uart_start_config>();

            //start receiving
            uart.start();

            uart.send_data(sensorInfo, sensorInfo.size());
        }

        //Processing the command from the host
        Result handleCommand(uint8_t command) {
            Result result = Success;
            switch (UartProtocol::getMessageType(command)) {
            case UartProtocol::MESSAGE_SYS:
                if (command == UartProtocol::BYTE_NACK) {
                    //device.dataRequest();
                } else {
                    return NotExpectedCommand;
                }
                break;
            case UartProtocol::MESSAGE_CMD: {
                    ev3::commands::MessageCommand<UartProtocol::UART_DATA_LENGTH> buffer(UartProtocol::getMessageLength(command));
                    if (buffer.is_valid_size()) {
                        result = readCommand(command, buffer.buffer(), buffer.size());
                        if (result == Success) {
                            uint8_t hostCommand = buffer.hostCommand();
                            switch (UartProtocol::getCommand(command)) {
                            case UartProtocol::CMD_SELECT:
                                device_type::changeMode(hostCommand);
                                break;
                            case UartProtocol::CMD_WRITE:
                                if (hostCommand == commands::DEVICE_RESET) {
                                    device_type::reset();
                                } else {
                                    commands::handleCommand(getDevice(), buffer);
                                }
                                break;
                            default:
                                return NotExpectedCommand;
                            }
                        }
                    } else {
                        return FormatError;
                    }
                }
                break;
            case UartProtocol::MESSAGE_DATA:
                break;
            default:
                return NotExpectedCommand;
            }
            return result;
        }

        //Sends data command to the host.
        bool sendData(uint8_t command, uint8_t offset, uint8_t size) {
            if (currentState == WaitingForCommand) {
                buffer[offset] = command;
                buffer[offset + size + 1] = ev3::commands::checksum(buffer + offset, sizeof(command) + size);
                //Send the command. Increase size by 2 bytes to take into account
                //command byte and checksum byte
                uart.send_data(buffer + offset, size + 2);
                return true;
            }
            return false;
        }

    public:
        //Calculates the command byte, checks the data size
        template <uint8_t offset, uint8_t size>
        bool sendData(uint8_t mode) {
            //We use enum to delclare a constant, because usage of "static const uint_8"
            //causes creation of constants in read-only memory
            enum helper {
                buffer_size = mpl::clp2<size>::value
            };
            static_assert(buffer_size + offset + 2 <= sizeof(buffer), "Buffer overrun");

            return sendData(UartProtocol::makeData(mode, mpl::log2<buffer_size>::value), offset, buffer_size);
        }

        //returns pointer to the data buffer. Skip one byte to place command there
        uint8_t* getBuffer() { return buffer + 1; }
    public:
        INLINE Ev3UartSensor()
        {
            currentState = Start;
        }

        void process() {
            uint8_t data;
            switch (currentState) {
            case Start:
                uart.reset();
                //Waiting for 500 ms
                OS::sleep(500);
                currentState = Init;
                break;
            case Reset:
                uart.reset();
                //Waiting for >= 100 ms
                OS::sleep(110);
                currentState = Init;
                break;
            case Init:
                init();
                currentState = WaitingForAck;
                break;
            case WaitingForAck:
                if (uart.get_byte(data, ACK_TIMEOUT) && data == UartProtocol::BYTE_ACK) {
                    currentState = SetSpeed;
                } else {
                    currentState = Start;
                }
                break;
            case SetSpeed:
                uart.template set_speed<Config>();
                //Waiting for 10 ms before host switched to the new speed
                OS::sleep(10);
                currentState = WaitingForCommand;

                device_type::start();
                break;
            case WaitingForCommand:
                if (!uart.get_byte(data, HEARTBEAT_PERIOD) || handleCommand(data) != Success) {
                    currentState = Reset;
                    device_type::stop();
                }
                break;
            }
        }

        void processIMU() {
            device_type::waitForEvent();
        }

        void handleGyroDataReady() {
            device_type::handleGyroDataReady();
        }

        void handleAcelDataReady() {
            device_type::handleAcelDataReady();
        }

        void handleUartReceive() {
            uart.handle_byte_receive();
        }

        void handleUartTransmit() {
            uart.handle_byte_transmit();
        }
    };

    template<int type, typename Uart, typename Config, template <typename> class Device>
    const Ev3UartSensor<type, Uart, Config, Device>::SensorInfo Ev3UartSensor<type, Uart, Config, Device>::sensorInfo;

}

#endif //__EV3_UART_SENSOR_H