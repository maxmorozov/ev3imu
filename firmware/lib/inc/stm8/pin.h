#ifndef __STM8_PIN_H
#define __STM8_PIN_H

#include <stm8_target.h>
#include <stm8/config.h>
#include <stm8/port_defs.h>
#include <stm8/port.h>
#include <stm8/ext_int.h>

namespace stm8 {

    namespace details {
        //Converts interupt sensitivity mode to enabled or disable iterrupts flag
        template <InterruptMode mode>
        struct interrupt_traits {
            static const uint8_t xCR2 = 1;
        };

        template <>
        struct interrupt_traits<InterruptsDisabled> {
            static const uint8_t xCR2 = 0;
        };
    }

    template <InputMode mode, InterruptMode interruptMode>
    struct input_traits {
        static const Direction direction = Input;

        //We cannot extract all information from the trait here, because it
        //requires port name, so we will handle it in port_configurer
        static const InterruptMode extIntMode = interruptMode;

        static const uint8_t xDDR = (uint8_t)direction;
        static const uint8_t xCR1 = (uint8_t)mode;
        static const uint8_t xCR2 = details::interrupt_traits<interruptMode>::xCR2;
    };

    template <OutputMode mode, OptputSpeedMode speedMode, Level initialLevel = Low>
    struct output_traits {
        static const Direction direction = Output;

        static const uint8_t xDDR = (uint8_t)direction;
        static const uint8_t xCR1 = (uint8_t)mode;
        static const uint8_t xCR2 = (uint8_t)speedMode;
        static const uint8_t xODR = (uint8_t)initialLevel;
    };


    template <Port port, int pin_no, typename PortTraits, Level activestate = High> struct Pin;

    template <Port port, int pin_no, typename PortTraits, Level activestate>
    struct Pin : public PortBase<port>
    {
        //IAR needs this
        using PortBase<port>::GPIOx;

        typedef PortTraits config;

        static const int pin = pin_no;
        static const uint8_t mask = 1 << pin_no;

        INLINE static void on()
        {
            if (activestate == Low)
                GPIOx->ODR &= (uint8_t)~mask;
            else
                GPIOx->ODR |= mask;
        }
        INLINE static void off()
        {
            if (activestate == Low)
                GPIOx->ODR |= mask;
            else
                GPIOx->ODR &= (uint8_t)~mask;
        }
        INLINE static void cpl()
        {
            //Clear volatile spec
            __BCPL(const_cast<uint8_t*>(&GPIOx->ODR), pin_no);
            //GPIOx->ODR ^= mask;
        }

        INLINE static uint8_t latched()
        {
            uint8_t ret = GPIOx->ODR & mask;
            return activestate == Low ? !ret : ret;
        }

        INLINE static uint8_t signalled()
        {
            uint8_t ret = GPIOx->IDR & mask;
            return activestate == Low ? !ret : ret;
        }

        //Configures one bit of the IO port
        //Does not configure external interrupts
        INLINE static void configure()
        {
            //Reset corresponding bit to GPIO_Pin in CR2 register
            GPIOx->CR2 &= (uint8_t)~mask;

            //Input/Output mode selection
            if (config::direction == Output) {
                //Output mode
                if (config::xODR != 0) { //High level
                    GPIOx->ODR |= mask;
                } else { //Low level
                    GPIOx->ODR &= (uint8_t)~mask;
                }
                // Set Output mode
                GPIOx->DDR |= mask;
            } else { //Input mode
                // Set Input mode
                GPIOx->DDR &= (uint8_t)~mask;
            }

            // Pull-Up/Float (Input) or Push-Pull/Open-Drain (Output) modes selection

            if (config::xCR1 != 0) { // Pull-Up or Push-Pull
                GPIOx->CR1 |= mask;
            } else { // Float or Open-Drain
                GPIOx->CR1 &= (uint8_t)~mask;
            }

            // Interrupt (Input) or Slope (Output) modes selection
            if (config::xCR2 != 0) { // Interrupt or Slow slope
                GPIOx->CR2 |= mask;
            } else { // No external interrupt or No slope control
                GPIOx->CR2 &= (uint8_t)~mask;
            }
        }
    };
}

#endif // __STM8_PIN_H
