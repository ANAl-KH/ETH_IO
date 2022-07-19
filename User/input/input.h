#ifndef _INPUT_H
#define	_INPUT_H

#include "stm32f4xx.h"
//���Ŷ���
/*******************************************************/                             
#define __INPUT1_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()
#define INPUT1_GPIO_PORT        GPIOA
#define INPUT1_PIN              GPIO_PIN_0

#define __INPUT2_CLK_ENABLE()   __HAL_RCC_GPIOC_CLK_ENABLE()
#define INPUT2_GPIO_PORT        GPIOC
#define INPUT2_PIN              GPIO_PIN_13      

                     
/*******************************************************/

//����Ϊ��������������Ϊ�ߵ�ƽ���������� INPUT_HIGH=1�� INPUT_LOW=0

//#define INPUT_HIGH	1
//#define INPUT_LOW	0

void Input_GPIO_Config(void);
uint8_t Input_Scan(void);

#endif 
