/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   FreeRTOS V9.0.0 + STM32 LwIP
  *********************************************************************
  * @attention
  *
  * 实验平台:野火  STM32全系列开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/ 
#include "main.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "client.h"
#include "timers.h"

/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* 创建任务句柄 */
TaskHandle_t I2ceeprom_Task_Handle = NULL;/* I2ceeprom任务句柄 */
//static TaskHandle_t Test2_Task_Handle = NULL;/* KEY任务句柄 */
static TimerHandle_t Swtmr1_Handle  =  NULL;//定时器任务
extern TaskHandle_t Ethsend_Task_Handle;//ETH发送任务

//static TaskHandle_t Uart_Task_Handle = NULL;//串口

/********************************** 内核对象句柄 *********************************/
/*
 * 信号量，消息队列，事件标志组，软件定时器这些都属于内核的对象，要想使用这些内核
 * 对象，必须先创建，创建成功之后会返回一个相应的句柄。实际上就是一个指针，后续我
 * 们就可以通过这个句柄操作这些内核对象。
 *
 * 内核对象说白了就是一种全局的数据结构，通过这些数据结构我们可以实现任务间的通信，
 * 任务间的事件同步等各种功能。至于这些功能的实现我们是通过调用这些内核对象的函数
 * 来完成的
 * 
 */
 
// QueueHandle_t I2ceeprom_Queue =NULL;

/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */


/******************************* 宏定义 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些宏定义。
 */

//#define  QUEUE_LEN    1   /* 队列的长度，最大可包含多少个消息 */
//#define  QUEUE_SIZE   20   /* 队列中每个消息大小（字节） */

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void AppTaskCreate(void);/* 用于创建任务 */

static void I2ceeprom_Task(void* pvParameters);/* i2cee_Task任务实现 */
//static void Test2_Task(void* pvParameters);/* Test2_Task任务实现 */
static void Swtmr1_Callback(void* parameter);//定时器任务实现
//static void Uart_Task(void* pvParameters);//串口

extern void TCPIP_Init(void);

/*****************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
            第二步：创建APP应用任务
            第三步：启动FreeRTOS，开始多任务调度
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  /* 开发板硬件初始化 */
  BSP_Init();

  /* 创建AppTaskCreate任务 */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
                        (const char*    )"AppTaskCreate",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )1, /* 任务的优先级 */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* 任务控制块指针 */ 
  /* 启动任务调度 */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* 启动任务，开启调度 */
  else
    return -1;  
  
  while(1);   /* 正常不会执行到这里 */    
}


/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
 
  TCPIP_Init();

  client_init();
	
	clientrecv_init();
  
//  printf("本例程演示开发板发送数据到服务器\n\n");
  
  printf("网络连接模型：\t 电脑<--网线-->路由<--网线-->I/O模块\n");
  
  printf("使用TCP协议传输数据，电脑作为TCP Server，I/O模块作为TCP Client\n");
  
//  printf("本例程的IP地址均在User/arch/sys_arch.h文件中修改\n\n");
//    
//  printf("本例程参考<<LwIP应用实战开发指南>>第15章 使用 NETCONN 接口编程\n\n");
  
  taskENTER_CRITICAL();           //进入临界区
	
	
//	/* 创建I2ceeprom_Queue */
//  I2ceeprom_Queue = xQueueCreate((UBaseType_t ) QUEUE_LEN,/* 消息队列的长度 */
//                            (UBaseType_t ) QUEUE_SIZE);/* 消息的大小 */
//  if(NULL != I2ceeprom_Queue)
//    printf("创建I2ceeprom_Queue消息队列成功!\r\n");

  /* 创建I2ceeprom_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )I2ceeprom_Task, /* 任务入口函数 */
                        (const char*    )"I2ceeprom_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )1,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&I2ceeprom_Task_Handle);/* 任务控制块指针 */
  if(pdPASS == xReturn)
//    printf("Create I2ceeprom_Task sucess...\r\n");
  
	
//  /* 创建Test2_Task任务 */
//  xReturn = xTaskCreate((TaskFunction_t )Test2_Task,  /* 任务入口函数 */
//                        (const char*    )"Test2_Task",/* 任务名字 */
//                        (uint16_t       )512,  /* 任务栈大小 */
//                        (void*          )NULL,/* 任务入口函数参数 */
//                        (UBaseType_t    )2, /* 任务的优先级 */
//                        (TaskHandle_t*  )&Test2_Task_Handle);/* 任务控制块指针 */ 
//  if(pdPASS == xReturn)
//    printf("Create Test2_Task sucess...\n\n");
	
	
//	 /* 创建Uart_Task任务 */
//  xReturn = xTaskCreate((TaskFunction_t )Uart_Task,  /* 任务入口函数 */
//                        (const char*    )"Uart_Task",/* 任务名字 */
//                        (uint16_t       )512,  /* 任务栈大小 */
//                        (void*          )NULL,/* 任务入口函数参数 */
//                        (UBaseType_t    )3, /* 任务的优先级 */
//                        (TaskHandle_t*  )&Uart_Task_Handle);/* 任务控制块指针 */ 
//  if(pdPASS == xReturn)
//    printf("创建 Uart_Task 任务成功!\n\n");
	
	
	//创建定时器任务
  Swtmr1_Handle=xTimerCreate((const char*)"AutoReloadTimer",
 (TickType_t)1000,/* 定时器周期 1000tick/1秒 */
 (UBaseType_t)pdTRUE,/* 周期模式 */
 (void* )1,/* 为每个计时器分配一个索引的唯一 ID */
 (TimerCallbackFunction_t)Swtmr1_Callback); /* 回调函数 */ 
 if (Swtmr1_Handle != NULL)
 { xTimerStart(Swtmr1_Handle,0);//开启周期定时器
//	 printf("创建计时器任务成功!\n\n");
 }
	
	
  printf("I/O模块启动完成!正在运行！\n");
  printf("本I/O模块具有2个输入3个输出\n");
  printf("输入模块支持事件触发自动向上位机发送状态\n");
  printf("输入模块支持每秒自动向上位机发送一次状态\n");
  printf("上位机可以通过网络发送命令：“query”来查询一次输入状态\n");
  printf("可通过网络发送命令：“LED1”、“LED2”、“LED3”来控制输出状态\n");
  printf("可通过串口配置上位机的IP端口\n");
  printf("示例192.168.2.187:5000\n");
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}



/**********************************************************************
  * @ 函数名  ： I2ceeprom_Task
  * @ 功能说明： I2ceeprom_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void I2ceeprom_Task(void* parameter)
{	
	uint16_t NotifyValue;
	uint16_t Checkip0,Checkip1,Checkip2,Checkip3,Checkport;
  while (1)
  {
		NotifyValue=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);	//获取任务通知
		if(NotifyValue!=0)	//清零之前的任务通知值不为0，说明任务通知有效
		{
//			printf("I2ceeprom_Task收到了%s\n",Usart_Rx_Buf);
			int xx = sscanf((char *)Usart_Rx_Buf, "%d.%d.%d.%d:%d", &Checkip0,&Checkip0,&Checkip0,&Checkip0,&Checkport);
			if (sscanf((char *)Usart_Rx_Buf, "%d.%d.%d.%d:%d", &Checkip0,&Checkip0,&Checkip0,&Checkip0,&Checkport) == 5)
				{
//					printf("准备调用I2C_Write_EE_IP\n");
					I2C_Write_EE_IP(&Usart_Rx_Buf);
				}else{
					printf("您输入的IP端口配置信息格式错误，请重新输入\n");
					printf("示例格式：192.168.2.187:5000\n");
				}
			memset(Usart_Rx_Buf,0,USART_RBUFF_SIZE);/* 清零 */
		}
  }
}

/**********************************************************************
  * @ 函数名  ： Test2_Task
  * @ 功能说明： Test2_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
//static void Test2_Task(void* parameter)
//{	 
//  while (1)
//  {
//    LED2_TOGGLE;
//    PRINT_DEBUG("LED2_TOGGLE\n");
//    vTaskDelay(2000);/* 延时2000个tick */
//  }
//}


/**********************************************************************
  * @ 函数名  ： 周期定时器
  * @ 功能说明： 定时通知client函数向上位机发送I/O状态
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
 static void Swtmr1_Callback(void* parameter)
 {
	 xTaskNotifyGive( Ethsend_Task_Handle );
 }


 /**********************************************************************
  * @ 函数名  ： 串口
  * @ 功能说明： 
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
// static void Uart_Task(void* parameter)
//{	
//	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
//  while (1)
//  {
//    //获取二值信号量 xSemaphore,没获取到则一直等待
//		xReturn = xSemaphoreTake(BinarySem_Handle,/* 二值信号量句柄 */
//                              portMAX_DELAY); /* 等待时间 */
//    if(pdPASS == xReturn)
//    {
//      LED2_TOGGLE;
//      printf("收到数据:%s\n",Usart_Rx_Buf);
//      memset(Usart_Rx_Buf,0,USART_RBUFF_SIZE);/* 清零 */
//    }
//  }
//}
/********************************END OF FILE****************************/
