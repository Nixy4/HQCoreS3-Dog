/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-05-25 09:20:06
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2022-03-09 14:08:21
 * @FilePath: \S3_DEMO\8.speech_rec\main\wav_player.c
 */
#include "wav_player.h"
#include "file_manager.h"
#include "driver/i2s.h"
#include "esp_log.h"
#include "esp_spiffs.h"

#include "app_speech_srcif.h"

#include "app_speech_comm.h"


#define TAG "WAV_PLAYER"

char path_buf[256] = {0};
char **g_file_list = NULL;
uint16_t g_file_num = 0;
volatile  bool playing = false;
typedef struct
{
	// The "RIFF" chunk descriptor
	uint8_t ChunkID[4];
	int32_t ChunkSize;
	uint8_t Format[4];
	// The "fmt" sub-chunk
	uint8_t Subchunk1ID[4];
	int32_t Subchunk1Size;
	int16_t AudioFormat;
	int16_t NumChannels;
	int32_t SampleRate;
	int32_t ByteRate;
	int16_t BlockAlign;
	int16_t BitsPerSample;
	// The "data" sub-chunk
	uint8_t Subchunk2ID[4];
	int32_t Subchunk2Size;
} wav_header_t;

/*
	王庆法增加:			语音识别功能和播放功能相互干扰.
		所以设计为 新的,播放正常运行,但是 可以随时被采集 识别终止;

		cap task 和 play task同时运行; 
		但是一旦 captask 识别到 wakeup唤醒词, 就终止播放,全心监听




*/

volatile int playIsOn = 0;		//正在播放吗 0-no 1-yes
volatile int playStopNow = 0;	// 1-立即结束	0-nothing


int play_is_on(void)
{
	return playIsOn;
}

int play_stop(void)
{
	playStopNow = 1;
	return playStopNow;
}


esp_err_t play_wav(const char *filepath)
{
	static int init_ok = 0;

	if(!init_ok ){
		play_i2s_init();//初始化播放模式的i2s
		init_ok = 1;
	}

	FILE *fd = NULL;
	struct stat file_stat;

	if (stat(filepath, &file_stat) == -1){		//先找找这个文件是否存在
		ESP_LOGE(TAG, "Failed to stat file : %s", filepath);
		//all_i2s_deinit();
		//record_i2s_init();
		return ESP_FAIL;//如果不存在就继续录音
	}

	ESP_LOGI(TAG, "file stat info: %s (%ld bytes)...", filepath, file_stat.st_size);
	fd = fopen(filepath, "r");

	if (NULL == fd){
		ESP_LOGE(TAG, "Failed to read existing file : %s", filepath);
		//all_i2s_deinit();
		//record_i2s_init();
		return ESP_FAIL;
	}
	const size_t chunk_size = 4096;
	uint8_t *buffer = malloc(chunk_size);

	if (NULL == buffer){
		ESP_LOGE(TAG, "audio data buffer malloc failed");
		//all_i2s_deinit();
		//record_i2s_init();
		fclose(fd);
		return ESP_FAIL;
	}

	/**
	 * read head of WAV file
	 */
	wav_header_t wav_head;
	int len = fread(&wav_head, 1, sizeof(wav_header_t), fd);//读取wav文件的文件头
	if (len <= 0){
		ESP_LOGE(TAG, "Read wav header failed");
		//all_i2s_deinit();
		//record_i2s_init();
		fclose(fd);
		return ESP_FAIL;
	}
	if (NULL == strstr((char *)wav_head.Subchunk1ID, "fmt") &&
			NULL == strstr((char *)wav_head.Subchunk2ID, "data")){
		ESP_LOGE(TAG, "Header of wav format error");
		//all_i2s_deinit();
		//record_i2s_init();
		fclose(fd);
		return ESP_FAIL;
	}

	playIsOn = 1;//	设置标志位,表示 正在播放

	ESP_LOGI(TAG, "frame_rate=%d, ch=%d, width=%d", wav_head.SampleRate, wav_head.NumChannels, wav_head.BitsPerSample);
	/**
	 * read wave data of WAV file
	 */
	size_t write_num = 0;
	size_t cnt;
	ESP_LOGI(TAG, "set clock");
	i2s_set_clk( I2S_NUM_SPK, wav_head.SampleRate, wav_head.BitsPerSample, 1);//根据该wav文件的各种参数来配置一下i2S的clk 采样率等等
	ESP_LOGI(TAG, "write data");
	do{
		/* Read file in chunks into the scratch buffer */
		len = fread(buffer, 1, chunk_size, fd);
		if (len <= 0){
			break;
		}
		i2s_write(I2S_NUM_SPK, buffer, len, &cnt, 1000 / portTICK_PERIOD_MS);//输出数据到I2S  就实现了播放
		write_num += len;

		if(playStopNow == 1){
			break;		//有人要求停止,那就结束吧
		}
	} while (1);
	fclose(fd);
	free(buffer);
	ESP_LOGI(TAG, "File reading complete, total: %d bytes", write_num);
	playIsOn = 0;		//没有播放
	playStopNow = 0;	//复位

	
	return ESP_OK;
}
void play_sdfile_name(char *file_name)
{
	playing = true;

	fm_sdcard_init();
	fm_print_dir("/sdcard", 2);
	fm_file_table_create(&g_file_list, &g_file_num, ".WAV");
	for (size_t i = 0; i < g_file_num; i++)
	{
		ESP_LOGI(TAG, "have file [%d:%s]", i, g_file_list[i]);
	}
	if (0 == g_file_num)
	{
		ESP_LOGW(TAG, "Can't found any wav file in sdcard!");
		all_i2s_deinit();
		record_i2s_init();
		sd_unmount();
		playing = false;
		return;
	}
	sprintf(path_buf, "%s/%s", "/sdcard", file_name);
	play_wav(path_buf);
	sd_unmount();
	fm_file_table_free(&g_file_list, g_file_num);
    record_i2s_init();
    //vTaskDelay(10);//wqf for down delay
	playing = false;
}
void play_spiffs_name(char *file_name)
{
#if  	1		//wqf for not disturb record 
	playing = true;

	sprintf(path_buf, "%s/%s", "/spiffs/wav", file_name);
	play_wav(path_buf);		//wqf add del play ,for not disturb record i2s
    playing = false;

#endif

}

void playWavTask(void *arg)
{
	play_i2s_init();//初始化播放模式的i2s
	while(1){
		/*等待 通知,获取播放内容文件 filepath */
		struct queue_play_msg  msg;
		printf(" playWavTask wait for play wav...\n ");
		queue_play_pop(&msg);
		
		char *filepath = msg.filepath;
	
		struct stat file_stat;
		if (stat(filepath, &file_stat) == -1){	//先找找这个文件是否存在
			ESP_LOGE(TAG, "Failed to stat file : %s", filepath);
			continue;//如果不存在就继续next
		}

		ESP_LOGI(TAG, "file stat info: %s (%ld bytes)...", filepath, file_stat.st_size);
		FILE *fd = fopen(filepath, "r");
		if (NULL == fd){
			ESP_LOGE(TAG, "Failed to read existing file : %s", filepath);
			continue;
		}
		const size_t chunk_size = 4096;
		uint8_t *buffer = malloc(chunk_size);

		if (NULL == buffer){
			ESP_LOGE(TAG, "audio data buffer malloc failed");
			fclose(fd);
			continue;
		}

		/**
		 * read head of WAV file
		 */
		wav_header_t wav_head;
		int len = fread(&wav_head, 1, sizeof(wav_header_t), fd);//读取wav文件的文件头
		if (len <= 0){
			ESP_LOGE(TAG, "Read wav header failed");
			fclose(fd);
			continue;
		}
		if (NULL == strstr((char *)wav_head.Subchunk1ID, "fmt") &&
				NULL == strstr((char *)wav_head.Subchunk2ID, "data")){
			ESP_LOGE(TAG, "Header of wav format error");
			fclose(fd);
			continue;
		}

		ESP_LOGI(TAG, "frame_rate=%d, ch=%d, width=%d", wav_head.SampleRate, wav_head.NumChannels, wav_head.BitsPerSample);
		/**
		 * read wave data of WAV file
		 */
		size_t write_num = 0;
		size_t cnt;
		ESP_LOGI(TAG, "set clock");
		i2s_set_clk( I2S_NUM_SPK, wav_head.SampleRate, wav_head.BitsPerSample, 1);//根据该wav文件的各种参数来配置一下i2S的clk 采样率等等
		ESP_LOGI(TAG, "write data");
 		do{   /* Read file in chunks into the scratch buffer */
			len = fread(buffer, 1, chunk_size, fd);
			if (len <= 0){
				break;
			}
			i2s_write(I2S_NUM_SPK, buffer, len, &cnt, 1000 / portTICK_PERIOD_MS);//输出数据到I2S  就实现了播放
			write_num += len;
		} while (1);
		fclose(fd);
		ESP_LOGI(TAG, "File reading complete, total: %d bytes", write_num);
	}

	return NULL;
}
