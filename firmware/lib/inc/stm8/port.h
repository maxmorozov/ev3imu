#ifndef __STM8_PORT_H
#define __STM8_PORT_H

#include "stm8_target.h"
#include <stm8/config.h>

namespace stm8 {
    enum Port {
        Port_A,
        Port_B,
        Port_C,
        Port_D,
        Port_E,
        Port_F,
        Port_G,
        Port_H,
        Port_I
    };

    template <Port port> struct port_gpio_t;

    template <> struct port_gpio_t<Port_A>
    {
        enum { GPIOx_BASE = GPIOA_BaseAddress };
    };

    template <> struct port_gpio_t<Port_B>
    {
        enum { GPIOx_BASE = GPIOB_BaseAddress };
    };

    template <> struct port_gpio_t<Port_C>
    {
        enum { GPIOx_BASE = GPIOC_BaseAddress };
    };

    template <> struct port_gpio_t<Port_D>
    {
        enum { GPIOx_BASE = GPIOD_BaseAddress };
    };

    template <> struct port_gpio_t<Port_E>
    {
        enum { GPIOx_BASE = GPIOE_BaseAddress };
    };

    template <> struct port_gpio_t<Port_F>
    {
        enum { GPIOx_BASE = GPIOF_BaseAddress };
    };

    template<> struct port_gpio_t<Port_G>
    {
        enum { GPIOx_BASE = GPIOG_BaseAddress };
    };

    template <> struct port_gpio_t<Port_H>
    {
        enum { GPIOx_BASE = GPIOH_BaseAddress };
    };

    template <> struct port_gpio_t<Port_I>
    {
        enum { GPIOx_BASE = GPIOI_BaseAddress };
    };

    template <Port port>
    struct PortBase {
        enum { GPIOx_BASE = port_gpio_t<port>::GPIOx_BASE };

        struct GPIO
        {
            GPIO_TypeDef* operator-> () { return (GPIO_TypeDef*)GPIOx_BASE; }
		};

		static GPIO GPIOx;

        static const Port port_name = port;
    };
	template <Port port>
	typename PortBase<port>::GPIO PortBase<port>::GPIOx;

}

#endif //__STM8_PORT_H