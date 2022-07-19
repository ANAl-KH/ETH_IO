/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#include "client.h"
#include "input.h"
#include "bsp_led.h"
#include <string.h>

#include "lwip/opt.h"

#include "lwip/sys.h"
#include "lwip/api.h"


TaskHandle_t Ethsend_Task_Handle;
TaskHandle_t Ethrecv_Task_Handle;

uint16_t ip_addr0 , ip_addr1 ,ip_addr2 ,ip_addr3 ,ip_port;

static  struct netconn *conn;

static void client(void *thread_param)
{
  int ret;
  ip4_addr_t ipaddr;
	uint16_t NotifyValue;
	

	uint8_t send_buf1[]= "Input1 is high,Input2 is high.\n";
	uint8_t send_buf2[]= "Input1 is high,Input2 is low.\n";
	uint8_t send_buf3[]= "Input1 is low,Input2 is high.\n";
	uint8_t send_buf4[]= "Input1 is low,Input2 is low.\n";
	

	  printf("目地IP地址:%d.%d.%d.%d \t 端口号:%d\n",      \
          ip_addr0,ip_addr1,ip_addr2,ip_addr3,ip_port);
  
  while(1)
  {
		//与上位机建立TCP连接
    conn = netconn_new(NETCONN_TCP);
    if (conn == NULL)
    {
      PRINT_DEBUG("create conn failed!\n");
      vTaskDelay(10);
      continue;
    }
    
    IP4_ADDR(&ipaddr,ip_addr0,ip_addr1,ip_addr2,ip_addr3);

    ret = netconn_connect(conn,&ipaddr,ip_port);
    if (ret == -1)
    {
        PRINT_DEBUG("Connect failed!\n");
        netconn_close(conn);
        vTaskDelay(10);
        continue;
    }

    PRINT_DEBUG("Connect to server successful!\n");
     
    while (1)
    {
			//上位机发送命令查询输入状态时，clientrecv会发送任务通知到本函数
			//输入事件触发功能，输入引脚的电平发生变化时GPIO中断会发送任务通知到本函数
			//定时器任务每秒发送一次任务通知到本函数
			//收到任务通知后本函数会向上位机发送一次输入引脚状态信息
			NotifyValue=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);	
		  if(NotifyValue!=0)	//清零之前的任务通知值不为0，说明任务通知有效
		  {		switch(Input_Scan()){
					case 11  :
					ret = netconn_write(conn,send_buf1,sizeof(send_buf1),0);
					break;
					case 10  :
					ret = netconn_write(conn,send_buf2,sizeof(send_buf2),0);
					break;
					case 01  :
					ret = netconn_write(conn,send_buf3,sizeof(send_buf3),0);
					break;
					case 00  :
					ret = netconn_write(conn,send_buf4,sizeof(send_buf4),0);
					break;
					}
			}
    }
  }

}

//接收并解析上位机的命令
static void clientrecv(void *thread_param)
{
	struct netbuf *recv_buf;
	err_t err;
  char msg[100];
	//控制输出的命令
	char *LED1recv = "LED1";
	char *LED2recv = "LED2";
	char *LED3recv = "LED3";
	//查询输入引脚状态的命令
	char *queryrecv = "query";
	
			while ((err = netconn_recv(conn, &recv_buf)) == 0) {
        do {
					   strncpy (msg, recv_buf->p->payload, recv_buf->p->len);
					   if(strncmp(msg,LED1recv,4) == 0)
						 {
							 LED1_TOGGLE;
					 	 }else if (strncmp(msg,LED2recv,4) == 0) {
							 LED2_TOGGLE;
						 }else if (strncmp(msg,LED3recv,4) == 0){
							 LED3_TOGGLE;
						 }else if (strncmp(msg,queryrecv,5) == 0){
							 xTaskNotifyGive( Ethsend_Task_Handle );
						 }
						 memset (msg, '\0', 100);
        } while (netbuf_next(recv_buf) >= 0);
        netbuf_delete(recv_buf);
      }
}


void
client_init(void)
{
  Ethsend_Task_Handle = sys_thread_new("client", client, NULL, 512, 4);
}

void
clientrecv_init(void)
{
  Ethrecv_Task_Handle = sys_thread_new("clientrecv", clientrecv, NULL, 512, 4);
}

