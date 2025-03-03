

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"



#include "app_speech_comm.h"


static QueueHandle_t queueForPlay = NULL;


int queue_play_init(void)
{
	queueForPlay = xQueueCreate(3, sizeof( struct queue_play_msg ));
	if(!queueForPlay){
		printf("xQueueCreate queueForPlay err\n");
		return -23;
	}
	return 0;
}


int queue_play_push( struct queue_play_msg  *pmsg)
{
	xQueueSend(queueForPlay, pmsg, portMAX_DELAY);
	return 0;
}

int queue_play_pop(struct queue_play_msg  *pmsg)
{
	if (xQueueReceive(queueForPlay,pmsg, portMAX_DELAY)) {
		return 0;
	}
	printf("xQueueReceive for play error\n");
	return -1;
}

