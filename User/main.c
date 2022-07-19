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

static TaskHandle_t AppTaskCreate_Handle = NULL;/* 创建任务句柄 */
TaskHandle_t I2ceeprom_Task_Handle = NULL;/* I2ceeprom任务句柄 */
//static TaskHandle_t Test2_Task_Handle = NULL;/* KEY任务句柄 */
static TimerHandle_t Swtmr1_Handle  =  NULL;//定时器任务
extern TaskHandle_t Ethsend_Task_Handle;//ETH发送任务


/********************************** 内核对象句柄 *********************************/

/******************************* 全局变量声明 ************************************/

/******************************* 宏定义 ************************************/

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void AppTaskCreate(void);/* 用于创建任务 */

static void I2ceeprom_Task(void* pvParameters);/* i2cee_Task任务实现 */
static void Swtmr1_Callback(void* parameter);//定时器任务实现

extern void TCPIP_Init(void);

/*****************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：硬件初始化 
            第二步：创建APP应用任务
            第三步：启动FreeRTOS，开始多任务调度
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  /* 硬件初始化 */
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
  
  while(1);    
}

/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;
 
  TCPIP_Init();//LwIP初始化

  client_init();//通过TCP协议连接上位机server
	
	clientrecv_init();//接收上位机传来的命令并解析并控制IO输出
  
  
  printf("网络连接模型：\t 电脑<--网线-->路由<--网线-->I/O模块\n");
  
  printf("使用TCP协议传输数据，电脑作为TCP Server，I/O模块作为TCP Client\n");
  
  taskENTER_CRITICAL();           //进入临界区
	

  /* 创建I2ceeprom_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )I2ceeprom_Task, /* 任务入口函数 */
                        (const char*    )"I2ceeprom_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )1,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&I2ceeprom_Task_Handle);/* 任务控制块指针 */
  if(pdPASS == xReturn)

	
	//创建定时器任务
  Swtmr1_Handle=xTimerCreate((const char*)"AutoReloadTimer",
 (TickType_t)1000,/* 定时器周期 1000tick/1秒 */
 (UBaseType_t)pdTRUE,/* 周期模式 */
 (void* )1,/* 为每个计时器分配一个索引的唯一 ID */
 (TimerCallbackFunction_t)Swtmr1_Callback); /* 回调函数 */ 
 if (Swtmr1_Handle != NULL)
 { xTimerStart(Swtmr1_Handle,0);//开启周期定时器
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
  * @ 功能说明： 接收串口空闲中断传来的任务通知，将串口接收到的配置信息保存在EEPROM中
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void I2ceeprom_Task(void* parameter)
{	
	uint16_t NotifyValue;
	uint16_t Checkip0,Checkip1,Checkip2,Checkip3,Checkport;
  while (1)
  {
		NotifyValue=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);	//获取串口空闲中断发送过来的任务通知
		if(NotifyValue!=0)	//清零之前的任务通知值不为0，说明任务通知有效
		{
			//验证配置信息的格式是否正确
			int xx = sscanf((char *)Usart_Rx_Buf, "%d.%d.%d.%d:%d", &Checkip0,&Checkip0,&Checkip0,&Checkip0,&Checkport);
			if (sscanf((char *)Usart_Rx_Buf, "%d.%d.%d.%d:%d", &Checkip0,&Checkip0,&Checkip0,&Checkip0,&Checkport) == 5)
				{
					I2C_Write_EE_IP(&Usart_Rx_Buf);//写入EEPROM
				}else{
					printf("您输入的IP端口配置信息格式错误，请重新输入\n");
					printf("示例格式：192.168.2.187:5000\n");
				}
			memset(Usart_Rx_Buf,0,USART_RBUFF_SIZE);//清空串口
		}
  }
}

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
 
/********************************END OF FILE****************************/
