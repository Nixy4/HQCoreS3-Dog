
#ifndef APP_SPEECH_COMM_H
#define	APP_SPEECH_COMM_H


#define I2S_NUM_SPK 0

#define I2S_NUM_MIC 1




struct queue_play_msg {
	char filepath[32];	//play file path
};

int queue_play_init(void);
int queue_play_push( struct queue_play_msg  *pmsg);
int queue_play_pop(struct queue_play_msg  *pmsg);


void playWavTask(void *arg);



#endif
