#ifndef __STM8_PORT_CONFIG_H
#define __STM8_PORT_CONFIG_H

#include <mpl/type_list.h>
#include <mpl/sort.h>

#include <stm8/pin.h>
#include <stm8/details/port_mpl.h>
#include <stm8/ext_int.h>

namespace stm8 {

    namespace details {

        //Combines pin types and initializes the port
        //TypeList should contain pins of the same port
        template <Port port, typename PinsTypeList>
        struct port_configurer : public PortBase<port>
        {
            //IAR needs this
            using PortBase<port>::GPIOx;

            //Filter pis for the same port
			//It works as a guard, because we expect that PinsTypeList is already contains pins of the same port
            typedef typename mpl::filter<PinsTypeList, details::port_filter<port>::template predicate>::type pins;

            static const uint8_t maskDDR = mpl::fold<pins, details::tag<0>, details::gen_mask<extract_DDR>::template apply>::type::value;
            static const uint8_t maskCR1 = mpl::fold<pins, details::tag<0>, details::gen_mask<extract_CR1>::template apply>::type::value;
            static const uint8_t maskCR2 = mpl::fold<pins, details::tag<0>, details::gen_mask<extract_CR2>::template apply>::type::value;
            static const uint8_t mask = mpl::fold<pins, details::tag<0>, details::pin2mask>::type::value;

            typedef typename mpl::filter<pins, details::is_output>::type output_pins;
            static const uint8_t maskODR = mpl::fold<output_pins, details::tag<0>, details::gen_mask<extract_ODR>::template apply>::type::value;
            static const uint8_t mask_output = mpl::fold<output_pins, details::tag<0>, details::pin2mask>::type::value;

            //Configures all bits in the port
            INLINE static void configure() {
                // Reset corresponding bits in CR2 register
                GPIOx->CR2 = 0;

                //Set initial state of output registers
                if (mask_output != 0) {
                    GPIOx->ODR = maskODR;
                }
                //Input/Output mode selection
                GPIOx->DDR = maskDDR;

                //Pull-Up/Float (Input) or Push-Pull/Open-Drain (Output) modes selection
                GPIOx->CR1 = maskCR1;

                //Interrupt (Input) or Slope (Output) modes selection
                GPIOx->CR2 = maskCR2;
            }
        };

        template <typename TypeList> struct port_configurer_gen;

        template <>
        struct port_configurer_gen<mpl::null_type>
        {
            struct type {
                INLINE static void configure() {
                }
            };
        };

        template <typename Head, typename Tail>
        struct port_configurer_gen<mpl::type_list<Head, Tail> >
        {
            static const stm8::Port port = details::get_port_name<Head>::value;

            typedef port_configurer<port, mpl::type_list<Head, Tail> > type;
        };

    }

    //Combines pin types and initializes the port
    template <typename PinsTypeList>
    struct PortConfigurer
    {
    private:
        //grouping all pins by port. The resulting type list consists of lists of pin types
        typedef typename mpl::sort<PinsTypeList, details::port_comparator>::type sorted_pins;
        typedef typename mpl::group_by<sorted_pins, details::port_comparator>::type grouped_pins;

        typedef typename mpl::map<grouped_pins, details::port_configurer_gen>::type port_configurers;

        //Recursion terminator
        INLINE static void configure(mpl::null_type) {
        }

        #pragma inline=forced
        template <typename Head, typename Tail>
        static void configure(mpl::type_list<Head, Tail>) {
            //Configure port
            Head::configure();
            //Call next port configurers
            configure(Tail());
        }

    public:

        //Configures all bits in the port
        INLINE static void configure() {
            //Configure GPIO pins
            configure(port_configurers());
            //Configure external interrupts
            ExtIntConfigurer<PinsTypeList>::configure();
        }
    };

}

#endif //__STM8_PORT_CONFIG_H