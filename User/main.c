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

static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */
TaskHandle_t I2ceeprom_Task_Handle = NULL;/* I2ceeprom������ */
//static TaskHandle_t Test2_Task_Handle = NULL;/* KEY������ */
static TimerHandle_t Swtmr1_Handle  =  NULL;//��ʱ������
extern TaskHandle_t Ethsend_Task_Handle;//ETH��������


/********************************** �ں˶����� *********************************/

/******************************* ȫ�ֱ������� ************************************/

/******************************* �궨�� ************************************/

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void);/* ���ڴ������� */

static void I2ceeprom_Task(void* pvParameters);/* i2cee_Task����ʵ�� */
static void Swtmr1_Callback(void* parameter);//��ʱ������ʵ��

extern void TCPIP_Init(void);

/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  /* Ӳ����ʼ�� */
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
  
  while(1);    
}

/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;
 
  TCPIP_Init();//LwIP��ʼ��

  client_init();//ͨ��TCPЭ��������λ��server
	
	clientrecv_init();//������λ���������������������IO���
  
  
  printf("��������ģ�ͣ�\t ����<--����-->·��<--����-->I/Oģ��\n");
  
  printf("ʹ��TCPЭ�鴫�����ݣ�������ΪTCP Server��I/Oģ����ΪTCP Client\n");
  
  taskENTER_CRITICAL();           //�����ٽ���
	

  /* ����I2ceeprom_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )I2ceeprom_Task, /* ������ں��� */
                        (const char*    )"I2ceeprom_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )1,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&I2ceeprom_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)

	
	//������ʱ������
  Swtmr1_Handle=xTimerCreate((const char*)"AutoReloadTimer",
 (TickType_t)1000,/* ��ʱ������ 1000tick/1�� */
 (UBaseType_t)pdTRUE,/* ����ģʽ */
 (void* )1,/* Ϊÿ����ʱ������һ��������Ψһ ID */
 (TimerCallbackFunction_t)Swtmr1_Callback); /* �ص����� */ 
 if (Swtmr1_Handle != NULL)
 { xTimerStart(Swtmr1_Handle,0);//�������ڶ�ʱ��
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
  * @ ����˵���� ���մ��ڿ����жϴ���������֪ͨ�������ڽ��յ���������Ϣ������EEPROM��
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void I2ceeprom_Task(void* parameter)
{	
	uint16_t NotifyValue;
	uint16_t Checkip0,Checkip1,Checkip2,Checkip3,Checkport;
  while (1)
  {
		NotifyValue=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);	//��ȡ���ڿ����жϷ��͹���������֪ͨ
		if(NotifyValue!=0)	//����֮ǰ������ֵ֪ͨ��Ϊ0��˵������֪ͨ��Ч
		{
			//��֤������Ϣ�ĸ�ʽ�Ƿ���ȷ
			int xx = sscanf((char *)Usart_Rx_Buf, "%d.%d.%d.%d:%d", &Checkip0,&Checkip0,&Checkip0,&Checkip0,&Checkport);
			if (sscanf((char *)Usart_Rx_Buf, "%d.%d.%d.%d:%d", &Checkip0,&Checkip0,&Checkip0,&Checkip0,&Checkport) == 5)
				{
					I2C_Write_EE_IP(&Usart_Rx_Buf);//д��EEPROM
				}else{
					printf("�������IP�˿�������Ϣ��ʽ��������������\n");
					printf("ʾ����ʽ��192.168.2.187:5000\n");
				}
			memset(Usart_Rx_Buf,0,USART_RBUFF_SIZE);//��մ���
		}
  }
}

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
 
/********************************END OF FILE****************************/
