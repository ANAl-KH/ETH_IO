#include "input.h"
#include "input_exti.h"


extern TaskHandle_t Ethsend_Task_Handle;

//�ⲿ�жϳ�ʼ��
void INPUT_EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    GPIO_Initure.Pin=GPIO_PIN_0;                //PA0
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING_FALLING;      //�����½��ض�����
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_13; 	//PC13
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    //�ж���0-PA0
    HAL_NVIC_SetPriority(INPUT1_INT_EXTI_IRQ,10,0);       //��ռ���ȼ�Ϊ2�������ȼ�Ϊ0
    HAL_NVIC_EnableIRQ(INPUT1_INT_EXTI_IRQ);             //ʹ���ж���0
    
    //�ж���13-PC13
    HAL_NVIC_SetPriority(INPUT2_INT_EXTI_IRQ,10,0);       //��ռ���ȼ�Ϊ2�������ȼ�Ϊ0
    HAL_NVIC_EnableIRQ(INPUT2_INT_EXTI_IRQ);             //ʹ���ж���13
}


	//�жϷ����������Ҫ��������
  //��HAL�������е��ⲿ�жϷ�����������ô˺���
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	BaseType_t xHigherPriorityTaskWoken;
    switch(GPIO_Pin)
    {
        case INPUT1_PIN:
        vTaskNotifyGiveFromISR(Ethsend_Task_Handle,&xHigherPriorityTaskWoken);//��������֪ͨ
		    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//�����Ҫ�Ļ�����һ�������л�
            break;
        case INPUT2_PIN:
        vTaskNotifyGiveFromISR(Ethsend_Task_Handle,&xHigherPriorityTaskWoken);//��������֪ͨ
		    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//�����Ҫ�Ļ�����һ�������л�
            break;
    }
}
