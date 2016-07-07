#ifndef __STM8S_SAFE_H
#define __STM8S_SAFE_H

//Unsafe marco definitions that can intersect with HAL class names

/**
  * @}
  */

/******************************************************************************/
/*                          Peripherals declarations                          */
/******************************************************************************/

#if defined(STM8S105) || defined(STM8S005) || defined(STM8S103) || defined(STM8S003) || \
    defined(STM8S903) || defined(STM8AF626x) || defined(STM8AF622x)
	inline ADC1_TypeDef* ADC1() { return (ADC1_TypeDef *) ADC1_BaseAddress; }
#endif /* (STM8S105) ||(STM8S103) || (STM8S005) ||(STM8S003) || (STM8S903) || (STM8AF626x) || (STM8AF622x)*/

#if defined(STM8S208) || defined(STM8S207) || defined (STM8S007) || defined (STM8AF52Ax) || \
    defined (STM8AF62Ax)
inline ADC2_TypeDef* ADC2() { return (ADC2_TypeDef*) ADC2_BaseAddress; }
#endif /* (STM8S208) ||(STM8S207) || (STM8S007) || (STM8AF52Ax) || (STM8AF62Ax) */

inline AWU_TypeDef* AWU() { return (AWU_TypeDef*) AWU_BaseAddress; }

inline BEEP_TypeDef* BEEP() { return (BEEP_TypeDef*) BEEP_BaseAddress; }

#if defined (STM8S208) || defined (STM8AF52Ax)
 inline CAN_TypeDef* CAN() { return (CAN_TypeDef*) CAN_BaseAddress; }
#endif /* (STM8S208) || (STM8AF52Ax) */

inline CLK_TypeDef* CLK() { return (CLK_TypeDef*) CLK_BaseAddress; }

inline EXTI_TypeDef* EXTI() { return (EXTI_TypeDef*) EXTI_BaseAddress; }

inline FLASH_TypeDef* FLASH() { return (FLASH_TypeDef*) FLASH_BaseAddress; }

inline OPT_TypeDef* OPT() { return (OPT_TypeDef*) OPT_BaseAddress; }

inline GPIO_TypeDef* GPIOA() { return (GPIO_TypeDef*) GPIOA_BaseAddress; }

inline GPIO_TypeDef* GPIOB() { return (GPIO_TypeDef*) GPIOB_BaseAddress; }

inline GPIO_TypeDef* GPIOC() { return (GPIO_TypeDef*) GPIOC_BaseAddress; }

inline GPIO_TypeDef* GPIOD() { return (GPIO_TypeDef*) GPIOD_BaseAddress; }

inline GPIO_TypeDef* GPIOE() { return (GPIO_TypeDef*) GPIOE_BaseAddress; }

inline GPIO_TypeDef* GPIOF() { return (GPIO_TypeDef*) GPIOF_BaseAddress; }

#if defined(STM8S207) || defined (STM8S007) || defined(STM8S208) || defined(STM8S105) || \
    defined(STM8S005) || defined (STM8AF52Ax) || defined (STM8AF62Ax) || defined (STM8AF626x)
 inline GPIO_TypeDef* GPIOG() { return (GPIO_TypeDef*) GPIOG_BaseAddress; }
#endif /* (STM8S208) ||(STM8S207)  || (STM8S105) || (STM8AF52Ax) || (STM8AF62Ax) || (STM8AF626x) */

#if defined(STM8S207) || defined (STM8S007) || defined(STM8S208) || defined (STM8AF52Ax) || \
    defined (STM8AF62Ax)
 inline GPIO_TypeDef* GPIOH() { return (GPIO_TypeDef*) GPIOH_BaseAddress; }
 inline GPIO_TypeDef* GPIOI() { return (GPIO_TypeDef*) GPIOI_BaseAddress; }
#endif /* (STM8S208) ||(STM8S207) || (STM8AF62Ax) || (STM8AF52Ax) */

inline RST_TypeDef* RST() { return (RST_TypeDef*) RST_BaseAddress; }

inline WWDG_TypeDef* WWDG() { return (WWDG_TypeDef*) WWDG_BaseAddress; }
inline IWDG_TypeDef* IWDG() { return (IWDG_TypeDef*) IWDG_BaseAddress; }

inline SPI_TypeDef* SPI() { return (SPI_TypeDef*) SPI_BaseAddress; }
inline I2C_TypeDef* I2C() { return (I2C_TypeDef*) I2C_BaseAddress; }

#if defined(STM8S208) ||defined(STM8S207) || defined (STM8S007) || defined(STM8S103) || \
    defined(STM8S003) ||defined(STM8S903) || defined (STM8AF52Ax) || defined (STM8AF62Ax)
 inline UART1_TypeDef* UART1() { return (UART1_TypeDef*) UART1_BaseAddress; }
#endif /* (STM8S208) ||(STM8S207)  || (STM8S103) || (STM8S903) || (STM8AF52Ax) || (STM8AF62Ax) */

#if defined (STM8S105) || defined (STM8S005) || defined (STM8AF626x)
 inline UART2_TypeDef* UART2() { return (UART2_TypeDef*) UART2_BaseAddress; }
#endif /* STM8S105 || STM8S005 || STM8AF626x */

#if defined(STM8S208) ||defined(STM8S207) || defined (STM8S007) || defined (STM8AF52Ax) || \
    defined (STM8AF62Ax)
 inline UART3_TypeDef* UART3() { return (UART3_TypeDef*) UART3_BaseAddress; }
#endif /* (STM8S208) ||(STM8S207) || (STM8AF62Ax) || (STM8AF52Ax) */

#if defined(STM8AF622x)
 inline UART4_TypeDef* UART4() { return (UART4_TypeDef*) UART4_BaseAddress; }
#endif /* (STM8AF622x) */

inline TIM1_TypeDef* TIM1() { return (TIM1_TypeDef*) TIM1_BaseAddress; }

#if defined(STM8S208) || defined(STM8S207) || defined (STM8S007) || defined(STM8S103) || \
    defined(STM8S003) || defined(STM8S105) || defined(STM8S005) || defined (STM8AF52Ax) || \
    defined (STM8AF62Ax) || defined (STM8AF626x)
 inline TIM2_TypeDef* TIM2() { return (TIM2_TypeDef*) TIM2_BaseAddress; }
#endif /* (STM8S208) ||(STM8S207)  || (STM8S103) || (STM8S105) || (STM8AF52Ax) || (STM8AF62Ax) || (STM8AF626x)*/

#if defined(STM8S208) || defined(STM8S207) || defined (STM8S007) || defined(STM8S105) || \
    defined(STM8S005) || defined (STM8AF52Ax) || defined (STM8AF62Ax) || defined (STM8AF626x)
 inline TIM3_TypeDef* TIM3() { return (TIM3_TypeDef*) TIM3_BaseAddress; }
#endif /* (STM8S208) ||(STM8S207)  || (STM8S105) || (STM8AF62Ax) || (STM8AF52Ax) || (STM8AF626x)*/

#if defined(STM8S208) ||defined(STM8S207) || defined (STM8S007) || defined(STM8S103) || \
    defined(STM8S003) || defined(STM8S105) || defined(STM8S005) || defined (STM8AF52Ax) || \
    defined (STM8AF62Ax) || defined (STM8AF626x)
 inline TIM4_TypeDef* TIM4() { return (TIM4_TypeDef*) TIM4_BaseAddress; }
#endif /* (STM8S208) ||(STM8S207)  || (STM8S103) || (STM8S105) || (STM8AF52Ax) || (STM8AF62Ax) || (STM8AF626x)*/

#if defined (STM8S903) || defined (STM8AF622x)
 inline TIM5_TypeDef* TIM5() { return (TIM5_TypeDef*) TIM5_BaseAddress; }
 inline TIM6_TypeDef* TIM6() { return (TIM6_TypeDef*) TIM6_BaseAddress; }
#endif /* (STM8S903) || (STM8AF622x) */

inline ITC_TypeDef* ITC() { return (ITC_TypeDef*) ITC_BaseAddress; }

inline CFG_TypeDef* CFG() { return (CFG_TypeDef*) CFG_BaseAddress; }

//inline DM_TypeDef* DM() { return (DM_TypeDef*) DM_BaseAddress; }

#endif //__STM8S_SAFE_H