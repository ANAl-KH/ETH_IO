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
  * ʵ��ƽ̨:Ұ��  STM32ȫϵ�п����� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
#include "main.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "client.h"
#include "timers.h"

/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */
TaskHandle_t I2ceeprom_Task_Handle = NULL;/* I2ceeprom������ */
//static TaskHandle_t Test2_Task_Handle = NULL;/* KEY������ */
static TimerHandle_t Swtmr1_Handle  =  NULL;//��ʱ������
extern TaskHandle_t Ethsend_Task_Handle;//ETH��������

//static TaskHandle_t Uart_Task_Handle = NULL;//����

/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */
 
// QueueHandle_t I2ceeprom_Queue =NULL;

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */


/******************************* �궨�� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩ�궨�塣
 */

//#define  QUEUE_LEN    1   /* ���еĳ��ȣ����ɰ������ٸ���Ϣ */
//#define  QUEUE_SIZE   20   /* ������ÿ����Ϣ��С���ֽڣ� */

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void);/* ���ڴ������� */

static void I2ceeprom_Task(void* pvParameters);/* i2cee_Task����ʵ�� */
//static void Test2_Task(void* pvParameters);/* Test2_Task����ʵ�� */
static void Swtmr1_Callback(void* parameter);//��ʱ������ʵ��
//static void Uart_Task(void* pvParameters);//����

extern void TCPIP_Init(void);

/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  /* ������Ӳ����ʼ�� */
  BSP_Init();

  /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
  /* ����������� */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1;  
  
  while(1);   /* ��������ִ�е����� */    
}


/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
 
  TCPIP_Init();

  client_init();
	
	clientrecv_init();
  
//  printf("��������ʾ�����巢�����ݵ�������\n\n");
  
  printf("��������ģ�ͣ�\t ����<--����-->·��<--����-->I/Oģ��\n");
  
  printf("ʹ��TCPЭ�鴫�����ݣ�������ΪTCP Server��I/Oģ����ΪTCP Client\n");
  
//  printf("�����̵�IP��ַ����User/arch/sys_arch.h�ļ����޸�\n\n");
//    
//  printf("�����̲ο�<<LwIPӦ��ʵս����ָ��>>��15�� ʹ�� NETCONN �ӿڱ��\n\n");
  
  taskENTER_CRITICAL();           //�����ٽ���
	
	
//	/* ����I2ceeprom_Queue */
//  I2ceeprom_Queue = xQueueCreate((UBaseType_t ) QUEUE_LEN,/* ��Ϣ���еĳ��� */
//                            (UBaseType_t ) QUEUE_SIZE);/* ��Ϣ�Ĵ�С */
//  if(NULL != I2ceeprom_Queue)
//    printf("����I2ceeprom_Queue��Ϣ���гɹ�!\r\n");

  /* ����I2ceeprom_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )I2ceeprom_Task, /* ������ں��� */
                        (const char*    )"I2ceeprom_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )1,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&I2ceeprom_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
//    printf("Create I2ceeprom_Task sucess...\r\n");
  
	
//  /* ����Test2_Task���� */
//  xReturn = xTaskCreate((TaskFunction_t )Test2_Task,  /* ������ں��� */
//                        (const char*    )"Test2_Task",/* �������� */
//                        (uint16_t       )512,  /* ����ջ��С */
//                        (void*          )NULL,/* ������ں������� */
//                        (UBaseType_t    )2, /* ��������ȼ� */
//                        (TaskHandle_t*  )&Test2_Task_Handle);/* ������ƿ�ָ�� */ 
//  if(pdPASS == xReturn)
//    printf("Create Test2_Task sucess...\n\n");
	
	
//	 /* ����Uart_Task���� */
//  xReturn = xTaskCreate((TaskFunction_t )Uart_Task,  /* ������ں��� */
//                        (const char*    )"Uart_Task",/* �������� */
//                        (uint16_t       )512,  /* ����ջ��С */
//                        (void*          )NULL,/* ������ں������� */
//                        (UBaseType_t    )3, /* ��������ȼ� */
//                        (TaskHandle_t*  )&Uart_Task_Handle);/* ������ƿ�ָ�� */ 
//  if(pdPASS == xReturn)
//    printf("���� Uart_Task ����ɹ�!\n\n");
	
	
	//������ʱ������
  Swtmr1_Handle=xTimerCreate((const char*)"AutoReloadTimer",
 (TickType_t)1000,/* ��ʱ������ 1000tick/1�� */
 (UBaseType_t)pdTRUE,/* ����ģʽ */
 (void* )1,/* Ϊÿ����ʱ������һ��������Ψһ ID */
 (TimerCallbackFunction_t)Swtmr1_Callback); /* �ص����� */ 
 if (Swtmr1_Handle != NULL)
 { xTimerStart(Swtmr1_Handle,0);//�������ڶ�ʱ��
//	 printf("������ʱ������ɹ�!\n\n");
 }
	
	
  printf("I/Oģ���������!�������У�\n");
  printf("��I/Oģ�����2������3�����\n");
  printf("����ģ��֧���¼������Զ�����λ������״̬\n");
  printf("����ģ��֧��ÿ���Զ�����λ������һ��״̬\n");
  printf("��λ������ͨ�����緢�������query������ѯһ������״̬\n");
  printf("��ͨ�����緢�������LED1������LED2������LED3�����������״̬\n");
  printf("��ͨ������������λ����IP�˿�\n");
  printf("ʾ��192.168.2.187:5000\n");
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}



/**********************************************************************
  * @ ������  �� I2ceeprom_Task
  * @ ����˵���� I2ceeprom_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void I2ceeprom_Task(void* parameter)
{	
	uint16_t NotifyValue;
	uint16_t Checkip0,Checkip1,Checkip2,Checkip3,Checkport;
  while (1)
  {
		NotifyValue=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);	//��ȡ����֪ͨ
		if(NotifyValue!=0)	//����֮ǰ������ֵ֪ͨ��Ϊ0��˵������֪ͨ��Ч
		{
//			printf("I2ceeprom_Task�յ���%s\n",Usart_Rx_Buf);
			int xx = sscanf((char *)Usart_Rx_Buf, "%d.%d.%d.%d:%d", &Checkip0,&Checkip0,&Checkip0,&Checkip0,&Checkport);
			if (sscanf((char *)Usart_Rx_Buf, "%d.%d.%d.%d:%d", &Checkip0,&Checkip0,&Checkip0,&Checkip0,&Checkport) == 5)
				{
//					printf("׼������I2C_Write_EE_IP\n");
					I2C_Write_EE_IP(&Usart_Rx_Buf);
				}else{
					printf("�������IP�˿�������Ϣ��ʽ��������������\n");
					printf("ʾ����ʽ��192.168.2.187:5000\n");
				}
			memset(Usart_Rx_Buf,0,USART_RBUFF_SIZE);/* ���� */
		}
  }
}

/**********************************************************************
  * @ ������  �� Test2_Task
  * @ ����˵���� Test2_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
//static void Test2_Task(void* parameter)
//{	 
//  while (1)
//  {
//    LED2_TOGGLE;
//    PRINT_DEBUG("LED2_TOGGLE\n");
//    vTaskDelay(2000);/* ��ʱ2000��tick */
//  }
//}


/**********************************************************************
  * @ ������  �� ���ڶ�ʱ��
  * @ ����˵���� ��ʱ֪ͨclient��������λ������I/O״̬
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
 static void Swtmr1_Callback(void* parameter)
 {
	 xTaskNotifyGive( Ethsend_Task_Handle );
 }


 /**********************************************************************
  * @ ������  �� ����
  * @ ����˵���� 
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
// static void Uart_Task(void* parameter)
//{	
//	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
//  while (1)
//  {
//    //��ȡ��ֵ�ź��� xSemaphore,û��ȡ����һֱ�ȴ�
//		xReturn = xSemaphoreTake(BinarySem_Handle,/* ��ֵ�ź������ */
//                              portMAX_DELAY); /* �ȴ�ʱ�� */
//    if(pdPASS == xReturn)
//    {
//      LED2_TOGGLE;
//      printf("�յ�����:%s\n",Usart_Rx_Buf);
//      memset(Usart_Rx_Buf,0,USART_RBUFF_SIZE);/* ���� */
//    }
//  }
//}
/********************************END OF FILE****************************/
