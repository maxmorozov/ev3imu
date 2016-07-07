#ifndef __EV3_UART_H
#define __EV3_UART_H

#include <mpl/math.h>

namespace ev3 {

    /* UartProtocol UART Device Communication Protocol
     *
     *  Definition of protocol used when an UART communicating device is detected at an input port
     *
     *  The communication used messages that consists of one or more bytes:
     *
      FIRST BYTE IN MESSAGE MEANS
      ===========================

      bit 76543210
          ||
          00LLLCC0  SYS     - System message
          ||||||||
          ||000000  SYNC    - Synchronisation byte
          ||000010  NACK    - Not acknowledge byte
          ||000100  ACK     - Acknowledge byte
          ||LLL110  ESC     - Reserved for future use
          ||
          ||
          01LLLCCC  CMD     - Command message
          ||   |||
          ||   000  TYPE
          ||   001  MODES
          ||   010  SPEED
          ||   011  SELECT
          ||   100  WRITE
          ||   101
          ||   110
          ||   111
          ||
          ||
          10LLLMMM  INFO    - Info message (next byte is command)
          ||   |||
          ||   000  Mode 0 default   (must be last mode in INFO stream to select as default)
          ||   001  Mode 1
          ||   010  Mode 2
          ||   011  Mode 3
          ||   100  Mode 4
          ||   101  Mode 5
          ||   110  Mode 6
          ||   111  Mode 7
          ||
          ||
          11LLLMMM  DATA    - Data message
            |||
            000     Message pay load is 1 byte not including command byte and check byte
            001     Message pay load is 2 bytes not including command byte and check byte
            010     Message pay load is 4 bytes not including command byte and check byte
            011     Message pay load is 8 bytes not including command byte and check byte
            100     Message pay load is 16 bytes not including command byte and check byte
            101     Message pay load is 32 bytes not including command byte and check byte



      Messages From Device
      ====================

        Command messages:

          TYPE      01000000  tttttttt  cccccccc                                                                                    Device type

          MODES     01001001  00000iii  00000jjj  cccccccc                                                                          Number of modes

          SPEED     01010010  ssssssss  ssssssss  ssssssss  ssssssss  cccccccc                                                      Max communication speed

        Info messages:

          NAME      10LLLMMM  00000000  aaaaaaaa  ........  cccccccc                                                                Name of Device in mode MMM

          RAW       10011MMM  00000001  llllllll  llllllll  llllllll  llllllll  hhhhhhhh  hhhhhhhh  hhhhhhhh  hhhhhhhh  cccccccc    Raw value span in mode MMM

          PCT       10011MMM  00000010  llllllll  llllllll  llllllll  llllllll  hhhhhhhh  hhhhhhhh  hhhhhhhh  hhhhhhhh  cccccccc    Percentage span in mode MMM

          SI        10011MMM  00000011  llllllll  llllllll  llllllll  llllllll  hhhhhhhh  hhhhhhhh  hhhhhhhh  hhhhhhhh  cccccccc    SI unit value span in mode MMM

          SYMBOL    10011MMM  00000100  aaaaaaaa  aaaaaaaa  aaaaaaaa  aaaaaaaa  aaaaaaaa  aaaaaaaa  aaaaaaaa  aaaaaaaa  cccccccc    SI symbol

          FORMAT    10010MMM  10000000  00nnnnnn  000000ff  0000FFFF  0000DDDD  cccccccc                                            Format of data in mode MMM

        Data messages:

          DATA      11LLLMMM  dddddddd  ........  cccccccc                                                                          Data in format described under INFO MMM


        Messages from the device must follow the above sequence
        Devices with more modes can repeat "Info messages" once for every mode
        Highest "mode number" must be first
        NAME is the first in info sequence and is necessary to initiate a mode info
        FORMAT is last in info sequence and is necessary to complete the modes info
        Other info messages is optional and has a default value that will be used if not provided
        Delay 10 mS between modes (from FORMATx to NAMEy) to allow the informations to be saved in the brick


        # After ACK only DATA is allowed at the moment

        (Simplest device only needs to send: TYPE, NAME, FORMAT - if SPEED, RAW, PCT and SI are defaults)



      Messages To Device
      ==================

        Command messages:

          SPEED     01010010  ssssssss  ssssssss  ssssssss  ssssssss  cccccccc                                                      Max communication speed

          SELECT    01000011  00000mmm  cccccccc                                                                                    Select new mode

          WRITE     01LLL100  dddddddd  ........  cccccccc                                                                          Write 1-23 bytes to device

        # After ACK only SELECT and WRITE is allowed at the moment



      BIT Explanations
      ================

          LLL       = Message pay load bytes not including command byte and check byte
                      000   = 1
                      001   = 2
                      010   = 4
                      011   = 8
                      100   = 16
                      101   = 32

          CCC       = Command
                      000   = TYPE
                      001   = MODES
                      010   = SPEED
                      011   = SELECT

          MMM       = Mode
                      000   = Mode 0 default   (must be last mode in INFO stream to select as default)
                      001   = Mode 1
                      010   = Mode 2
                      011   = Mode 3
                      100   = Mode 4
                      101   = Mode 5
                      110   = Mode 6
                      111   = Mode 7

          iii       = Number of modes
                      000   = Only mode 0      (default if message not received)
                      001   = Mode 0,1
                      010   = Mode 0,1,2
                      011   = Mode 0,1,2,3
                      100   = Mode 0,1,2,3,4
                      101   = Mode 0,1,2,3,4,5
                      110   = Mode 0,1,2,3,4,5,6
                      111   = Mode 0,1,2,3,4,5,6,7

          jjj       = Number of modes in view and data log (default is iii if not received)
                      000   = Only mode 0
                      001   = Mode 0,1
                      010   = Mode 0,1,2
                      011   = Mode 0,1,2,3
                      100   = Mode 0,1,2,3,4
                      101   = Mode 0,1,2,3,4,5
                      110   = Mode 0,1,2,3,4,5,6
                      111   = Mode 0,1,2,3,4,5,6,7

          cccccccc  = Check byte (result of 0xFF exclusively or'ed with all preceding bytes)

          ssssssss  = Speed 4 bytes (ULONG with LSB first)

          tttttttt  = Device type (used together with mode to form VM device type)

          llllllll  = Floating point value for low (RAW/PCT/SI) value (used to scale values)
                      (Default if not received - RAW = 0.0, PCT = 0.0, SI = 0.0)

          hhhhhhhh  = Floating point value for high (RAW/PCT/SI) value (used to scale values)
                      (Default if not received - RAW = 1023.0, PCT = 100.0, SI = 1.0)

          nnnnn     = Number of data sets [1..32 DATA8] [1..16 DATA16] [1..8 DATA32] [1..8 DATAF]

          ff        = Data set format
                      00    = DATA8
                      01    = DATA16
                      10    = DATA32
                      11    = DATAF

          aaaaaaaa  = Device name in ACSII characters (fill with zero '\0' to get to LLL - no zero termination necessary)
                      (Default if not received - empty)

          dddddddd  = Binary data (LSB first)

          mmm       = Mode
                      000   = Mode 0
                      001   = Mode 1
                      010   = Mode 2
                      011   = Mode 3
                      100   = Mode 4
                      101   = Mode 5
                      110   = Mode 6
                      111   = Mode 7

          FFFF      = Total number of figures shown in view and datalog [0..15] (inclusive decimal point and decimals)
                      (Default if not received - 4)

          DDDD      = Number of decimals shown in view and datalog [0..15]
                      (Default if not received - 0)


      Example with info coming from a device with two modes
      =====================================================


          TYPE      01000000  tttttttt  cccccccc                                                type

          MODES     01001001  00000001  00000001  cccccccc                                      Mode 0 and 1, both shown in view

          SPEED     01010010  00000000  11100001  00000000  00000000  cccccccc                  57600 bits/Second


          NAME      10011001  00000000  'L' 'i' 'g' 'h' 't' '\0' '\0' '\0'  cccccccc            "Light"

          RAW       10011001  00000001  0.0 1023.0  cccccccc                                    RAW 0-1023

          SI        10011001  00000011  0.0 1023.0  cccccccc                                    SI 0-1023

          SYMBOL    10011001  00000100  'l' 'x' '\0' '\0' '\0' '\0' '\0' '\0' cccccccc          "lx"

          FORMAT    10010001  10000000  00000001  00000001  00000100  00000000  cccccccc        1 * DATA16, 4 figures, 0 decimals


          NAME      10011000  00000000  'C' 'o' 'l' 'o' 'r' '\0' '\0' '\0'  cccccccc            "Color"

          RAW       10011000  00000001  0.0    6.0  cccccccc                                    RAW 0-6

          SI        10011000  00000011  0.0    6.0  cccccccc                                    SI 0-6

          FORMAT    10010000  10000000  00000001  00000001  00000001  00000000  cccccccc        1 * DATA16, 1 figure, 0 decimals


          ACK


      Error handling
      ==============

      Before ACK:

      IF ANYTHING GOES WRONG - HOST WILL NOT ACK AND DEVICE MUST RESET !


      After ACK

      IF MORE THAN 5 ERRORS - HOST WILL NOT SERVICE WATCHDOG

     */
    struct UartProtocol {
        enum SysMessageType {
            BYTE_SYNC = 0x00,          // Synchronisation byte
            BYTE_ACK = 0x04,           // Acknowledge byte
            BYTE_NACK = 0x02           // Not acknowledge byte
        };

        //Two higher bits of the command
        enum MessageType {
            MESSAGE_SYS = 0x00,        // System message
            MESSAGE_CMD = 0x40,        // Command message
            MESSAGE_INFO = 0x80,       // Info message
            MESSAGE_DATA = 0xC0        // Data message
        };

        //Three least bits of the command
        enum CommandType {
            CMD_TYPE = 0x00,           // CMD command - TYPE     (device type for VM reference)
            CMD_MODES = 0x01,          // CMD command - MODES    (number of supported modes 0=1)
            CMD_SPEED = 0x02,          // CMD command - SPEED    (maximun communication speed)
            CMD_SELECT = 0x03,         // CMD command - SELECT   (select mode)
            CMD_WRITE = 0x04           // CMD command - WRITE    (write to device)
        };

        //The second byte of INFO command
        struct InfoByte {
            static const uint8_t NAME = 0;      // name of mode
            static const uint8_t RAW = 1;       // range of raw sensor readings
            static const uint8_t PCT = 2;       // range of readings in percent
            static const uint8_t SI = 3;        // range of readings in SI units
            static const uint8_t SYMBOL = 4;    // name of the SI unit
            static const uint8_t FORMAT = 0x80; // format of the sensor data in mode
        };

        static const int MAX_MODES = 8;
        static const int UART_DATA_LENGTH = 32;

        /**
         * All messages have following format:
         * <command> <payload> <checksum>
         * INFO commnand consists of two bytes. The second byte contains info type (NAME, RAW etc.)
         * The first command byte format:
         * CCLLLXXX where
         *     CC  - message type
         *     LLL - payload length in format 2**(LLLb)
         *     XXX - command specific content
         */
        static uint8_t getMessageType(uint8_t byte) { return byte & 0xC0; }
        static uint8_t getCommand(uint8_t byte) { return byte & 0x07; }
        static uint8_t getMessageLength(uint8_t byte) { return 1 << ((byte >> 3) & 0x07); }


        //Composes the message. Length must be a power of 2.
        static uint8_t makeMessage(MessageType message, uint8_t length, uint8_t content) {
            return message | (content & 0x7) | ((length & 0x7) << 3);
        }

        //Here length is log2(size).
        static uint8_t makeCommandMessage(CommandType command, uint8_t length) { return makeMessage(MESSAGE_CMD, length, (uint8_t)command); }
        static uint8_t makeInfo(uint8_t mode, uint8_t length) { return makeMessage(MESSAGE_INFO, length, mode); }
        static uint8_t makeData(uint8_t mode, uint8_t length) { return makeMessage(MESSAGE_DATA, length, mode); }

        //Utility constants
        static const uint8_t CMD_WRITE_MASK = (uint8_t)MESSAGE_CMD | (uint8_t)CMD_WRITE;
    };

    //Metafunctions and constants for EV3 commands
    struct EV3Command {
    private:
        template <UartProtocol::MessageType message, uint8_t length, uint8_t content>
        struct ev3_message {
            static_assert(mpl::is_pow2<length>::value, "length must be a power of 2");

            static const uint8_t value = message | (content & 0x7) | ((mpl::log2<length>::value & 0x7) << 3);
        };
    public:
        //Metafunctions to compose the first byte of messages
        template <UartProtocol::CommandType cmd, uint8_t length>
        struct command : ev3_message<UartProtocol::MESSAGE_CMD, length, (uint8_t)cmd> {};

        template <uint8_t mode, uint8_t length>
        struct info : ev3_message<UartProtocol::MESSAGE_INFO, length, mode> {};

        template <uint8_t mode, uint8_t length>
        struct data : ev3_message<UartProtocol::MESSAGE_DATA, length, mode> {};

        //Commands with fixed-lenth payload.
        //They have been precalculated to simplicity
        enum Commands {
            CMD_TYPE = command<UartProtocol::CMD_TYPE, 1>::value,     //0x40
            CMD_SELECT = command<UartProtocol::CMD_SELECT, 1>::value, //0x43
            CMD_MODES = command<UartProtocol::CMD_MODES, 2>::value,   //0x49
            CMD_SPEED = command<UartProtocol::CMD_SPEED, sizeof(uint32_t)>::value   //0x52
        };
    };
}

#endif //__EV3_UART_H