


#ifndef _MYUART__H
#define _MYUART__H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/uart_select.h"
#include "driver/gpio.h"
 
 
/* 引脚和串口定义 
	T37 R36  	
*/
#define USART_TX_GPIO_PIN   19			
#define USART_RX_GPIO_PIN   20
 
/* 串口接收相关定义 */
#define RX_BUF_SIZE         128    /* 环形缓冲区大小,64不行 */
 
 
 
/* 函数声明 */
void usart_init(int uart_num,uint32_t baudrate); /* 初始化串口 */

int uart_send_data(int uart_num, char *data,int len);

void uart_task_setup_for_recv(void);

int uart_cmd_push2Stm32(long addr);



#ifdef __cplusplus
}
#endif


#endif
