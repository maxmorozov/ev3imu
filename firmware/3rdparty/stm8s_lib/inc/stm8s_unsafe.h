#ifndef __STM8S_UNSAFE_H
#define __STM8S_UNSAFE_H

//Unsafe marco definitions that can intersect with HAL class names

/**
  * @}
  */

/******************************************************************************/
/*                          Peripherals declarations                          */
/******************************************************************************/

#if defined(STM8S105) || defined(STM8S005) || defined(STM8S103) || defined(STM8S003) || \
    defined(STM8S903) || defined(STM8AF626x) || defined(STM8AF622x)
 #define ADC1 ((ADC1_TypeDef *) ADC1_BaseAddress)
#endif /* (STM8S105) ||(STM8S103) || (STM8S005) ||(STM8S003) || (STM8S903) || (STM8AF626x) || (STM8AF622x)*/

#if defined(STM8S208) || defined(STM8S207) || defined (STM8S007) || defined (STM8AF52Ax) || \
    defined (STM8AF62Ax)
#define ADC2 ((ADC2_TypeDef *) ADC2_BaseAddress)
#endif /* (STM8S208) ||(STM8S207) || (STM8S007) || (STM8AF52Ax) || (STM8AF62Ax) */

#define AWU ((AWU_TypeDef *) AWU_BaseAddress)

#define BEEP ((BEEP_TypeDef *) BEEP_BaseAddress)

#if defined (STM8S208) || defined (STM8AF52Ax)
 #define CAN ((CAN_TypeDef *) CAN_BaseAddress)
#endif /* (STM8S208) || (STM8AF52Ax) */

#define CLK ((CLK_TypeDef *) CLK_BaseAddress)

#define EXTI ((EXTI_TypeDef *) EXTI_BaseAddress)

#define FLASH ((FLASH_TypeDef *) FLASH_BaseAddress)

#define OPT ((OPT_TypeDef *) OPT_BaseAddress)

#define GPIOA ((GPIO_TypeDef *) GPIOA_BaseAddress)

#define GPIOB ((GPIO_TypeDef *) GPIOB_BaseAddress)

#define GPIOC ((GPIO_TypeDef *) GPIOC_BaseAddress)

#define GPIOD ((GPIO_TypeDef *) GPIOD_BaseAddress)

#define GPIOE ((GPIO_TypeDef *) GPIOE_BaseAddress)

#define GPIOF ((GPIO_TypeDef *) GPIOF_BaseAddress)

#if defined(STM8S207) || defined (STM8S007) || defined(STM8S208) || defined(STM8S105) || \
    defined(STM8S005) || defined (STM8AF52Ax) || defined (STM8AF62Ax) || defined (STM8AF626x)
 #define GPIOG ((GPIO_TypeDef *) GPIOG_BaseAddress)
#endif /* (STM8S208) ||(STM8S207)  || (STM8S105) || (STM8AF52Ax) || (STM8AF62Ax) || (STM8AF626x) */

#if defined(STM8S207) || defined (STM8S007) || defined(STM8S208) || defined (STM8AF52Ax) || \
    defined (STM8AF62Ax)
 #define GPIOH ((GPIO_TypeDef *) GPIOH_BaseAddress)
 #define GPIOI ((GPIO_TypeDef *) GPIOI_BaseAddress)
#endif /* (STM8S208) ||(STM8S207) || (STM8AF62Ax) || (STM8AF52Ax) */

#define RST ((RST_TypeDef *) RST_BaseAddress)

#define WWDG ((WWDG_TypeDef *) WWDG_BaseAddress)
#define IWDG ((IWDG_TypeDef *) IWDG_BaseAddress)

#define SPI ((SPI_TypeDef *) SPI_BaseAddress)
#define I2C ((I2C_TypeDef *) I2C_BaseAddress)

#if defined(STM8S208) ||defined(STM8S207) || defined (STM8S007) || defined(STM8S103) || \
    defined(STM8S003) ||defined(STM8S903) || defined (STM8AF52Ax) || defined (STM8AF62Ax)
 #define UART1 ((UART1_TypeDef *) UART1_BaseAddress)
#endif /* (STM8S208) ||(STM8S207)  || (STM8S103) || (STM8S903) || (STM8AF52Ax) || (STM8AF62Ax) */

#if defined (STM8S105) || defined (STM8S005) || defined (STM8AF626x)
 #define UART2 ((UART2_TypeDef *) UART2_BaseAddress)
#endif /* STM8S105 || STM8S005 || STM8AF626x */

#if defined(STM8S208) ||defined(STM8S207) || defined (STM8S007) || defined (STM8AF52Ax) || \
    defined (STM8AF62Ax)
 #define UART3 ((UART3_TypeDef *) UART3_BaseAddress)
#endif /* (STM8S208) ||(STM8S207) || (STM8AF62Ax) || (STM8AF52Ax) */

#if defined(STM8AF622x)
 #define UART4 ((UART4_TypeDef *) UART4_BaseAddress)
#endif /* (STM8AF622x) */

#define TIM1 ((TIM1_TypeDef *) TIM1_BaseAddress)

#if defined(STM8S208) || defined(STM8S207) || defined (STM8S007) || defined(STM8S103) || \
    defined(STM8S003) || defined(STM8S105) || defined(STM8S005) || defined (STM8AF52Ax) || \
    defined (STM8AF62Ax) || defined (STM8AF626x)
 #define TIM2 ((TIM2_TypeDef *) TIM2_BaseAddress)
#endif /* (STM8S208) ||(STM8S207)  || (STM8S103) || (STM8S105) || (STM8AF52Ax) || (STM8AF62Ax) || (STM8AF626x)*/

#if defined(STM8S208) || defined(STM8S207) || defined (STM8S007) || defined(STM8S105) || \
    defined(STM8S005) || defined (STM8AF52Ax) || defined (STM8AF62Ax) || defined (STM8AF626x)
 #define TIM3 ((TIM3_TypeDef *) TIM3_BaseAddress)
#endif /* (STM8S208) ||(STM8S207)  || (STM8S105) || (STM8AF62Ax) || (STM8AF52Ax) || (STM8AF626x)*/

#if defined(STM8S208) ||defined(STM8S207) || defined (STM8S007) || defined(STM8S103) || \
    defined(STM8S003) || defined(STM8S105) || defined(STM8S005) || defined (STM8AF52Ax) || \
    defined (STM8AF62Ax) || defined (STM8AF626x)
 #define TIM4 ((TIM4_TypeDef *) TIM4_BaseAddress)
#endif /* (STM8S208) ||(STM8S207)  || (STM8S103) || (STM8S105) || (STM8AF52Ax) || (STM8AF62Ax) || (STM8AF626x)*/

#if defined (STM8S903) || defined (STM8AF622x)
 #define TIM5 ((TIM5_TypeDef *) TIM5_BaseAddress)
 #define TIM6 ((TIM6_TypeDef *) TIM6_BaseAddress)
#endif /* (STM8S903) || (STM8AF622x) */ 

#define ITC ((ITC_TypeDef *) ITC_BaseAddress)

#define CFG ((CFG_TypeDef *) CFG_BaseAddress)

#define DM ((DM_TypeDef *) DM_BaseAddress)

#endif //__STM8S_UNSAFE_H