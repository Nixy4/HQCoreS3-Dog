

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#include "uart-rt.h"
 

int guart_rt_num = UART_NUM_2;

unsigned char data[RX_BUF_SIZE] = {0};
 

/*	
	对外提供接口, 外界调用它
	原理, 定义一个队列,存储 char* 
	缺点: task间传递任务,如果 对方迅速连续更新 ,可能读到 被破坏的数据.
	
*/

static QueueHandle_t queueCmd2UartSTM32 = NULL;	//camera capture to it



/*
	uart_num:	UART_NUM_2
*/
void usart_init(int uart_num,uint32_t baudrate)
{
    uart_config_t uart_config;                          /* 串口配置句柄 */
 
    uart_config.baud_rate = baudrate;                   /* 波特率 */
    uart_config.data_bits = UART_DATA_8_BITS;           /* 字长为8位数据格式 */
    uart_config.parity = UART_PARITY_DISABLE;           /* 无奇偶校验位 */
    uart_config.stop_bits = UART_STOP_BITS_1;           /* 一个停止位 */
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;   /* 无硬件控制流 */
    uart_config.source_clk = UART_SCLK_APB;             /* 配置时钟源 */
    uart_config.rx_flow_ctrl_thresh = 122;              /* 硬件控制流阈值 */
    uart_param_config(uart_num, &uart_config);          /* 配置uart端口 */

	guart_rt_num = uart_num;
    /* 配置uart引脚 */
    uart_set_pin(uart_num, USART_TX_GPIO_PIN, USART_RX_GPIO_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    /* 安装串口驱动 */
    uart_driver_install(uart_num, RX_BUF_SIZE*2, RX_BUF_SIZE*2, 20, NULL, 0);


	// 队列 queue init, allow other task push data in
	queueCmd2UartSTM32 = xQueueCreate(5, sizeof(long));
}


int uart_cmd_push2Stm32(long addr)
{
	printf("%s-%d %ld\n",__func__,__LINE__,addr);

	xQueueSend(queueCmd2UartSTM32, &addr, portMAX_DELAY);

	return 0;
}


int uart_send_data(int uart_num, char *data,int len)
{
	printf("%s-%d %s\n",__func__,__LINE__,data);

	int ret = uart_write_bytes(uart_num, (const char*)data, strlen((const char*)data));
	if(ret <0){
		printf("uart_send_data err %d\n",ret);
		return ret;
	}
	return ret;
}


static void uart_recv_task(void *arg)
{
	uint8_t len = 0;

	long addr = 0;
	char *recvbuf;

	printf("\n uart start recv task\n");

	while(1 ){
	    uart_get_buffered_data_len(guart_rt_num, (size_t*) &len); 
	    if(len>0){
	        memset(data, 0, RX_BUF_SIZE); 
	        uart_read_bytes(guart_rt_num, data, len, 100);
			printf("uart2 recv data len:%d %s\n",len, data);
			uart_send_data(guart_rt_num,(const char*)data,strlen((const char*)data));
			//vTaskDelay(20);
		}

		//转发消息给stm32
		if ( xQueueReceive(queueCmd2UartSTM32, &addr, 50) ){
			recvbuf = (char *)addr;
			uart_send_data(guart_rt_num,(const char*)recvbuf,strlen((const char*)recvbuf));
			printf("%s-%d  %s\n",__func__,__LINE__,recvbuf);
		}
	}
}

void uart_task_setup_for_recv(void)
{

	xTaskCreatePinnedToCore(&uart_recv_task, "Uart recv task", 1024 * 4 , NULL, 5, NULL, 0);
}






