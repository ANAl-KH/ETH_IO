#ifndef _INPUT_EXTI_H
#define _INPUT_EXTI_H
#include "sys.h"

#define INPUT1_INT_EXTI_IRQ     EXTI0_IRQn
#define INPUT1_IRQHandler       EXTI0_IRQHandler


#define INPUT2_INT_EXTI_IRQ     EXTI15_10_IRQn
#define INPUT2_IRQHandler       EXTI15_10_IRQHandler


void INPUT_EXTI_Init(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
#endif
