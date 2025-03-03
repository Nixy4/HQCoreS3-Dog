/*
 * @Descripttion :
 * @version      :
 * @Author       : Kevincoooool
 * @Date         : 2021-05-25 16:03:28
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-07-28 12:22:19
 * @FilePath: \SP_V2_DEMO\8.new_speech_rec\main\app_speech_wakeup.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "xtensa/core-macros.h"
#include "esp_partition.h"
#include "app_speech_srcif.h"
#include "sdkconfig.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "dl_lib_coefgetter_if.h"
//#include "app_main.h"
#include "app_led.h"			  
#include "esp_mn_iface.h"
#include "esp_mn_models.h"
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"

//#include "page_wakeup.h"
#include "wav_player.h"

#include "dl_lib_coefgetter_if.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "esp_afe_sr_models.h"
#include "esp_mn_iface.h"
#include "esp_mn_models.h"
#include "model_path.h"

#include "page_wakeup.h"

#include "uart-rt.h"		//uart 2 stm32


#include "app_speech_comm.h"




#define TAG "SPEECH"
static const esp_wn_iface_t *wakenet = &WAKENET_MODEL;
static const model_coeff_getter_t *model_coeff_getter = &WAKENET_COEFF;
// MultiNet
static const esp_mn_iface_t *multinet = &MULTINET_MODEL;
model_iface_data_t *model_data_mn = NULL;

static src_cfg_t srcif;
static model_iface_data_t *model_data;

QueueHandle_t sndQueue;

extern lv_obj_t *label_speech;

esp_afe_sr_iface_t *afe_handle = NULL;

//						 0    1        2				4					 6
char srCmdName[][16] = { " ","  ", "Front", "Back","TurnLeft","TurnRight","Stop",
	//  7 				 8					9					11			
	"ShakeHeadLeft", "ShakeHeadRight", "FrontDown","BackDown","StandUp",
	//  12 		  13					15						 18
	"FrontUp" ,	"BackUp", "ShakeHand", "Pacing", "Fun1", "Fun2","Fun3",
	//	19	 20		 21
	"Fun4", "Cute","Down"
};



/*
 * 语音识别处理函数
 */
static esp_err_t asr_multinet_control(int commit_id)
{
	static char buf2stm32[64] = {0};	//发给stm32,需要一直保留

    if (commit_id >= 0 && commit_id < ID_MAX)
    {
		snprintf(buf2stm32,sizeof(buf2stm32), 
			"toSTM32:action=move,arg=%s;",srCmdName[commit_id] );

		char tempbuf[64] = {"      "};
		strcat(tempbuf, srCmdName[commit_id] );
		
		lv_label_set_text(label_speech, tempbuf );
		//ESP_LOGI(TAG, srCmdName[commit_id]);  不能这么用,为啥 wqf

		uart_cmd_push2Stm32((long)buf2stm32  );

		struct queue_play_msg msg;
		sprintf(msg.filepath, "%s/%s", "/spiffs/wav", "haode.wav");
		queue_play_push(&msg);
	
        return ESP_OK;
    }
    return ESP_FAIL;
}
void tips_rec(void)
{
    static uint8_t i = 0;
    i++;
    if (i > 200)
    {
        i = 0;
        printf("recing...\r\n");
    }
}
void tips_wake(void)
{
    static uint8_t i = 0;
    i++;
    if (i > 200)
    {
        i = 0;
        printf("waiting for waking...\r\n");
    }
}
void nnTask(void *arg)
{

    esp_afe_sr_data_t *afe_data = arg;
    int afe_chunksize = afe_handle->get_fetch_chunksize(afe_data);
    int nch = afe_handle->get_channel_num(afe_data);
    //int16_t *buff = (int16_t *)malloc(afe_chunksize * sizeof(int16_t));
	int16_t *buff = heap_caps_malloc(afe_chunksize * sizeof(int16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
	
    assert(buff);

    static const esp_mn_iface_t *multinet = &MULTINET_MODEL;
    model_iface_data_t *model_data = multinet->create((const model_coeff_getter_t *)&MULTINET_COEFF, 5760);
    int mu_chunksize = multinet->get_samp_chunksize(model_data);
    int chunk_num = multinet->get_samp_chunknum(model_data);

	//wqf set the prob of property
	multinet->set_det_threshold(model_data,0.6);
	for(int i=0;i< ID_MAX;i++){
		multinet->set_command_det_threshold(model_data, i, 0.6);
	}

	
    assert(mu_chunksize == afe_chunksize);

    printf("------------------detect start------------\n");
    lv_label_set_text(label_speech, "I'm listening......");
	
    ESP_LOGI(TAG, "Ready");

    int chunks = 0;
    int mn_chunks = 0;
    bool detect_flag = 0;

    while (1)
    {
        int res = afe_handle->fetch(afe_data, buff);
        if (res == AFE_FETCH_WWE_DETECTED) {
            playing = true;		//播放的时候不要采集,会干扰识别
            vTaskDelay(100 );	//让 mic task 有机会检测到这里
            afe_handle->disable_wakenet(afe_data);		//禁止网络,应该是防止spk干扰
            ESP_LOGI(TAG, "      >>> Say your command <<<");
            //page_wakeup_start();		只要执行一次就够了
            detect_flag = true;
            lv_label_set_text(label_speech, "I am Listening say it.");
            printf("      -----------------LISTENING-----------------\n\n");
			
			app_led_set_all(0, 1, 0);
			//播放,发送通知
			struct queue_play_msg msg;
			sprintf(msg.filepath, "%s/%s", "/spiffs/wav", "wozai.wav");
			queue_play_push(&msg);
			vTaskDelay(1.3*1000 );			//	等待播放完毕
			app_led_set_all(0, 15, 0);		//在灯亮度提升后在说话
        }
		
        if (detect_flag){
            // Detect comman
            int command = (command_word_t)multinet->detect(model_data, buff);
            if (command == COMMAND_NOT_DETECTED)
                continue;
            else if (command == COMMAND_TIMEOUT) {
                playing = true;
                vTaskDelay(100 );	//让 capture task 能够停止
                afe_handle->enable_wakenet(afe_data);
                ESP_LOGI(TAG, "      >>> Waiting to be waken up <<<");
                vTaskDelay(100 );
                lv_label_set_text(label_speech, "Say it again, please...");

				//播放,发送通知
				struct queue_play_msg msg;
				sprintf(msg.filepath, "%s/%s", "/spiffs/wav", "what.wav");
				queue_play_push(&msg);

				vTaskDelay(2.8*1000 );			//	等待播放完毕
				printf("\n      -----------awaits to be waken up-----------\n");
                //page_wakeup_end();
                detect_flag = false;
				playing = false;
            } else {
                playing = true;
                vTaskDelay(150    );	//应该是 让rectask 干完活等待!!!
                asr_multinet_control(command);
                ESP_LOGI(TAG, "Command: %d", command);
                //lv_label_set_text(label_speech, "OK.");
                printf("\n      -----------awaits to be waken up-----------\n");
                vTaskDelay(0.5*1000 );			//	等待播放完毕
                //page_wakeup_end();
#ifndef CONFIG_SR_MN_CN_MULTINET3_CONTINUOUS_RECOGNITION
                afe_handle->enable_wakenet(afe_data);
                detect_flag = false;
                ESP_LOGI(TAG, "      >>> Waiting to be waken up <<<");
#endif
                command = COMMAND_TIMEOUT;
                playing = false;
            }

			app_led_set_all(0, 0, 0);	//wqf sr end,close led
        }

		
    }

    // free(buffer);
    vTaskDelete(NULL);
}

#include "esp_spiffs.h"
static void spiffs_init(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/srmodel",
        .partition_label = "model",
        .max_files = 5,
        .format_if_mount_failed = true};
    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }
    size_t total = 0, used = 0;
    ret = esp_spiffs_info("model", &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
}
#include "esp_vfs.h"
#include "esp_spiffs.h"
/*显示spiffs的所有文件名*/
static void SPIFFS_Directory(char *path)
{
    DIR *dir = opendir(path);
    assert(dir != NULL);
    while (true)
    {
        struct dirent *pe = readdir(dir);
        if (!pe)
            break;
        ESP_LOGI(__FUNCTION__, "d_name=%s d_ino=%d d_type=%x", pe->d_name, pe->d_ino, pe->d_type);
    }
    closedir(dir);
}
/*
    初始化语音识别任务
*/
void app_speech_wakeup_init()
{
    spiffs_init();

	printf("%s-%d \n",__func__,__LINE__);
    SPIFFS_Directory("/srmodel/");
    afe_config_t afe_config = {
        .aec_init = true,
        .se_init = true,
        .vad_init = true,
        .wakenet_init = true,
        .vad_mode = 3,
        .wakenet_model = &WAKENET_MODEL,
        .wakenet_coeff = (const model_coeff_getter_t *)&WAKENET_COEFF,
        .wakenet_mode = DET_MODE_2CH_90,
        .afe_mode = AFE_PSRAM_MEDIA_COST  ,  //SR_MODE_LOW_COST,
        .afe_perferred_core = 0,
        .afe_perferred_priority = 5,
        .afe_ringbuf_size = 50,
        .alloc_from_psram = AFE_PSRAM_MEDIA_COST  ,//AFE_PSRAM_LOW_COST ,//,没影响
        .agc_mode = 2,
    };
    afe_handle = &esp_afe_sr_1mic;

    afe_config.aec_init = false;
    afe_config.se_init = false;
    afe_config.vad_init = false;
    afe_config.afe_ringbuf_size = 10;
    esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(&afe_config);


	queue_play_init( );
    xTaskCreatePinnedToCore(&recsrcTask, "recsrcTask", 5 * 1024, (void *)afe_data,13, NULL, 0); // 创建录音任务  13

    xTaskCreatePinnedToCore(&nnTask, "nnTask", 6* 1024 , (void *)afe_data, 14, NULL, 1); // 创建识别任务  14


    xTaskCreatePinnedToCore(&playWavTask, "playTask", 4* 1024 , (void *)afe_data, 14, NULL, 1); // 创建播放任务   



}
