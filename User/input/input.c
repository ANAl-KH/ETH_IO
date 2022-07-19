//I/Oģ������벿��

#include "input.h"

//���������õ���io��
void Input_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*��������GPIO�ڵ�ʱ��*/
	__INPUT1_CLK_ENABLE();
	__INPUT2_CLK_ENABLE();
	
  /*ѡ�����������*/
	GPIO_InitStructure.Pin = INPUT1_PIN; 
  
  /*��������Ϊ����ģʽ*/
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT; 
  
  /*�������Ų�����Ҳ������*/
  GPIO_InitStructure.Pull = GPIO_NOPULL;
	
	//����
	GPIO_InitStructure.Speed=GPIO_SPEED_HIGH;
	
  /*ʹ������Ľṹ���ʼ������*/
	HAL_GPIO_Init(INPUT1_GPIO_PORT, &GPIO_InitStructure);  
  
  /*ѡ�����������*/
	GPIO_InitStructure.Pin = INPUT2_PIN; 
  
  /*ʹ������Ľṹ���ʼ������*/
	HAL_GPIO_Init(INPUT2_GPIO_PORT, &GPIO_InitStructure);  
}


uint8_t Input_Scan(void)
{ 
  if(HAL_GPIO_ReadPin(INPUT1_GPIO_PORT,INPUT1_PIN))
	{if(HAL_GPIO_ReadPin(INPUT2_GPIO_PORT,INPUT2_PIN))
		{return 11;}else{return 10;}
	}else if(HAL_GPIO_ReadPin(INPUT2_GPIO_PORT,INPUT2_PIN))
	{return 01;}else{return 00;}
}
