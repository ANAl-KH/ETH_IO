//I/O模块的输入部分

#include "input.h"

//配置输入用到的io口
void Input_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*开启输入GPIO口的时钟*/
	__INPUT1_CLK_ENABLE();
	__INPUT2_CLK_ENABLE();
	
  /*选择输入的引脚*/
	GPIO_InitStructure.Pin = INPUT1_PIN; 
  
  /*设置引脚为输入模式*/
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT; 
  
  /*设置引脚不上拉也不下拉*/
  GPIO_InitStructure.Pull = GPIO_NOPULL;
	
	//高速
	GPIO_InitStructure.Speed=GPIO_SPEED_HIGH;
	
  /*使用上面的结构体初始化输入*/
	HAL_GPIO_Init(INPUT1_GPIO_PORT, &GPIO_InitStructure);  
  
  /*选择输入的引脚*/
	GPIO_InitStructure.Pin = INPUT2_PIN; 
  
  /*使用上面的结构体初始化输入*/
	HAL_GPIO_Init(INPUT2_GPIO_PORT, &GPIO_InitStructure);  
}


uint8_t Input_Scan(void)
{ 
////	uint8_t str1[] = "Input1 is high,Input2 is high.\n";
////	uint8_t str2[] = "Input1 is high,Input2 is low.\n";
////	uint8_t str3[] = "Input1 is low,Input2 is high.\n";
////	uint8_t str4[] = "Input1 is low,Input2 is low.\n";

  if(HAL_GPIO_ReadPin(INPUT1_GPIO_PORT,INPUT1_PIN))
	{if(HAL_GPIO_ReadPin(INPUT2_GPIO_PORT,INPUT2_PIN))
		{return 11;}else{return 10;}
	}else if(HAL_GPIO_ReadPin(INPUT2_GPIO_PORT,INPUT2_PIN))
	{return 01;}else{return 00;}
//	
//	char string1[]="",string2[]="";
//  sprintf(string1, "Input1 is %d.", HAL_GPIO_ReadPin(INPUT1_GPIO_PORT,INPUT1_PIN));
//	sprintf(string2, "Input2 is %d.", HAL_GPIO_ReadPin(INPUT2_GPIO_PORT,INPUT2_PIN));
//  strcat(string1, string2);
//	send_buf = 
//	return string1;
}

