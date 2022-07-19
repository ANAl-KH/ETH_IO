#include "input.h"
#include "input_exti.h"


extern TaskHandle_t Ethsend_Task_Handle;

//外部中断初始化
void INPUT_EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    GPIO_Initure.Pin=GPIO_PIN_0;                //PA0
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING_FALLING;      //上升下降沿都触发
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_13; 	//PC13
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    //中断线0-PA0
    HAL_NVIC_SetPriority(INPUT1_INT_EXTI_IRQ,10,0);       //抢占优先级为2，子优先级为0
    HAL_NVIC_EnableIRQ(INPUT1_INT_EXTI_IRQ);             //使能中断线0
    
    //中断线13-PC13
    HAL_NVIC_SetPriority(INPUT2_INT_EXTI_IRQ,10,0);       //抢占优先级为2，子优先级为0
    HAL_NVIC_EnableIRQ(INPUT2_INT_EXTI_IRQ);             //使能中断线13
}


	//中断服务程序中需要做的事情
  //在HAL库中所有的外部中断服务函数都会调用此函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	BaseType_t xHigherPriorityTaskWoken;
    switch(GPIO_Pin)
    {
        case INPUT1_PIN:
        vTaskNotifyGiveFromISR(Ethsend_Task_Handle,&xHigherPriorityTaskWoken);//发送任务通知
		    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//如果需要的话进行一次任务切换
            break;
        case INPUT2_PIN:
        vTaskNotifyGiveFromISR(Ethsend_Task_Handle,&xHigherPriorityTaskWoken);//发送任务通知
		    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//如果需要的话进行一次任务切换
            break;
    }
}
