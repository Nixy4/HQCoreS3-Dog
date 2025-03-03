#include <stdio.h>


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "nvs_flash.h"


#include "esp_err.h"
#include "esp_log.h"

#include "page_wakeup.h"

#include "lvgl_helpers.h"


#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/rmt.h"

#include "app_speech_srcif.h"
#include "app_led.h"		 

#include "esp_task_wdt.h"



#include "who_camera.h"
#include "who_lcd.h"
#include "who_adc_button.h"
#include "who_button.h"
#include "who_color_detection.hpp"
#include "who_human_face_detection.hpp"
#include "who_human_face_recognition.hpp"
#include "who_cat_face_detection.hpp"
#include "who_motion_detection.hpp"
#include "app_wifi.h"
#include "app_httpd.hpp"
#include "app_mdns.h"
#include "event_logic.hpp"

#include "uart-rt.h"


#define TAG "esp32-dog"


static QueueHandle_t queueCamera = NULL;	//camera capture to it


/*用定时器给LVGL提供时钟*/
static void lv_tick_task(void *arg)
{
    (void)arg;
    lv_tick_inc(10);
	//printf("%s-%d\n",__func__,__LINE__);
}

SemaphoreHandle_t xGuiSemaphore;

static void gui_task(void *arg)
{
    xGuiSemaphore = xSemaphoreCreateMutex();
    lv_init(); // lvgl内核初始化

    lvgl_driver_init(); // lvgl显示接口初始化
    // 申请两个buffer来给lvgl刷屏用
    /*外部PSRAM方式*/
     lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
     lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    /*内部DMA方式*/
    //lv_color_t *buf1   = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    //lv_color_t *buf2   = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);

    //lv_color_t *buf1   = (lv_color_t *)heap_caps_malloc(240*5*2, MALLOC_CAP_DMA);
    //lv_color_t *buf2   = (lv_color_t *)heap_caps_malloc(240*5*2, MALLOC_CAP_DMA);

	printf("lvgl alloc buf  %d\n",DISP_BUF_SIZE * sizeof(lv_color_t));
	if(!buf1 || !buf2){
		printf("%s-%d lvgl malloc buf err\n",__func__,__LINE__);
		while(1){; }
	}
	

    // static lv_color_t buf1[DISP_BUF_SIZE];
    // static lv_color_t buf2[DISP_BUF_SIZE];
    static lv_disp_buf_t disp_buf;
    uint32_t size_in_px = DISP_BUF_SIZE;
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    // lv_indev_drv_t indev_drv;
    // lv_indev_drv_init(&indev_drv);
    // indev_drv.read_cb = touch_driver_read;
    // indev_drv.type = LV_INDEV_TYPE_POINTER;
    // lv_indev_drv_register(&indev_drv);

    // esp_register_freertos_tick_hook(lv_tick_task);
    /* 创建一个定时器中断来进入 lv_tick_inc 给lvgl运行提供心跳 这里是10ms一次 主要是动画运行要用到 */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 10 * 1000));
    page_wakeup_start();
    while (1)
    {

#if  0	
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));
        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
            //  page.Running();
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
#else 
		vTaskDelay(pdMS_TO_TICKS(10));
		lv_task_handler();
#endif
    }
}


/*显示spiffs的所有文件名*/
static void SPIFFS_Directory(char *path)
{
    DIR *dir = opendir(path);
    assert(dir != NULL);
    while (true)  {
        struct dirent *pe = readdir(dir);
        if (!pe)
            break;
        ESP_LOGI(__FUNCTION__, "d_name=%s d_ino=%d d_type=%x", pe->d_name, pe->d_ino, pe->d_type);
    }
    closedir(dir);
}

extern "C" void app_main(void)
{
	esp_err_t ret;

	/*uart2 init*/
	usart_init(UART_NUM_2,115200);
	uart_task_setup_for_recv();

	/* ws led init   and blink 3 times*/
	ret = app_led_init(GPIO_RMT_LED);
	ESP_ERROR_CHECK(ret);

	for(int i=0;i<5;i++){
		ret = app_led_set_all(0, 10, 0);
		vTaskDelay(250);
		ret = app_led_set_all(0, 0, 0);
		vTaskDelay(250);
	}
	
	printf("%s-%d start wifi init\n",__func__,__LINE__);
	/*camera --> http */

	// 初始化nvs用于存放wifi或者其他需要掉电保存的东西
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	app_wifi_main();	//wifi esp init
	app_mdns_main();	// dns init
	
	queueCamera = xQueueCreate(2, sizeof(camera_fb_t *));
	register_camera(PIXFORMAT_JPEG, FRAMESIZE_HVGA, 2, queueCamera);
	register_httpd(queueCamera, NULL, true);

	//##########		lgvl task
	
    /*初始化spiffs用于存放字体文件或者图片文件或者网页文件或者音频文件*/
    esp_vfs_spiffs_conf_t conf = {
    	.base_path = "/spiffs",
    	.partition_label = "storage",
    	.max_files = 20,
    	.format_if_mount_failed = true //false
    };

    ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK){
    	if (ret == ESP_FAIL)
    		ESP_LOGE(TAG, "Failed to mount or format filesystem");
    	else if (ret == ESP_ERR_NOT_FOUND)
    		ESP_LOGE(TAG, "Failed to find SPIFFS partition");
    	else
    		ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
    	return;
    }

    /*显示spiffs里的文件列表*/	
    SPIFFS_Directory("/spiffs/");


    /*初始化语音唤醒识别任务*/
	xTaskCreatePinnedToCore(&gui_task, "gui task", 1024 * 5, NULL, 5, NULL, 1);
	vTaskDelay(100 / portTICK_PERIOD_MS);

	app_speech_wakeup_init();
}
