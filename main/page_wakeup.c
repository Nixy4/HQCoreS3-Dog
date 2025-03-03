/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-04-26 10:21:13
 * @FilePath: \MASTER_V1.3\21.new_speech_rec\main\page\page_wakeup.c
 */
#include "page_wakeup.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include <esp_system.h>
#include "esp_log.h"
#include "lv_port_indev.h"


#include "sdkconfig.h"		//wqf


//LV_FONT_DECLARE(myFont);


lv_obj_t *label_speech;                                                                                 

void wakeup_init(void)
{
	label_speech = lv_label_create(lv_scr_act(), NULL);
	/*Modify the Label's text*/
	
	static lv_style_t style_label_speech;
	lv_style_init(&style_label_speech);

	//Write style state: LV_STATE_DEFAULT for style_label_speech
	lv_style_set_radius(&style_label_speech, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_label_speech, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_bg_grad_color(&style_label_speech, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_bg_grad_dir(&style_label_speech, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_label_speech, LV_STATE_DEFAULT, 255);
	lv_style_set_text_color(&style_label_speech, LV_STATE_DEFAULT, LV_COLOR_BLACK);

   // lv_obj_set_style_local_text_font(label_speech, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_style_set_text_letter_space(&style_label_speech, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_label_speech, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_label_speech, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_label_speech, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_label_speech, LV_STATE_DEFAULT, 0);

	lv_obj_add_style(label_speech, LV_LABEL_PART_MAIN, &style_label_speech);
    lv_label_set_long_mode(label_speech,LV_LABEL_LONG_BREAK);
	lv_obj_set_pos(label_speech, 0, LV_VER_RES-150);
	lv_obj_set_size(label_speech, LV_HOR_RES, 180);
    lv_label_set_recolor(label_speech, true);


	/*wqf display the ap hotpoint  name and passwd*/
	uint8_t mac[6]={0};
	char wifi_ap_name[32] = {0};
	esp_err_t err = esp_efuse_mac_get_default(mac);
	snprintf((char *)wifi_ap_name, 32, "%s-%x%x%x%x",CONFIG_ESP_WIFI_AP_SSID,mac[2],mac[3],mac[4],mac[5] );

	char displaybuf[128] = {0};
	snprintf(displaybuf,sizeof(displaybuf),"#0000ff hello# \nI'm DIY ESP32S3.\n  wifi:%s 12345678 \n  192.168.4.1",wifi_ap_name);
			
	lv_label_set_text(label_speech, displaybuf );
}

void page_wakeup_load()
{
    wakeup_init();

}
void page_wakeup_end()
{
	lv_obj_del(label_speech);
}
void page_wakeup_start()
{
	// //获取芯片可用内存
	// printf("     esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
	// //获取从未使用过的最小内存
	// printf("     esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());
	// page_wakeup_load();

	// static lv_style_t style;
	// lv_style_init(&style);
	// 设置边距
	// lv_style_set_pad_top(&style, LV_STATE_DEFAULT, 20);
	// lv_style_set_pad_bottom(&style, LV_STATE_DEFAULT, 20);
	// lv_style_set_pad_left(&style, LV_STATE_DEFAULT, 20);
	// lv_style_set_pad_right(&style, LV_STATE_DEFAULT, 20);

	lv_obj_t* scr = lv_scr_act();

	//拼接WiFi信息
	uint8_t mac[6]={0};
	char wifi_ap_name[32] = {0};
	ESP_ERROR_CHECK( esp_efuse_mac_get_default(mac) );
	snprintf((char *)wifi_ap_name, 32, "%s-%x%x%x%x",CONFIG_ESP_WIFI_AP_SSID,mac[2],mac[3],mac[4],mac[5] );

	lv_obj_t* label = lv_label_create(scr, NULL);
	lv_label_set_text(label, "Model: HQ Core S3");
	lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 40, 20);
	label= lv_label_create(scr, NULL);
	lv_label_set_text_fmt(label, "WiFi SSID: %s", wifi_ap_name);
	lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 40, 50);
	label= lv_label_create(scr, NULL);
	lv_label_set_text(label, "WiFi Password: 12345678");
	lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 40, 80);
	label= lv_label_create(scr, NULL);
	lv_label_set_text_fmt(label, "IP Address:%s", CONFIG_SERVER_IP);
	lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 40, 110);
	label_speech = lv_label_create(scr, NULL);
	lv_label_set_text(label_speech, "Hi I'n LeXin");
	lv_obj_align(label_speech, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 40, -40);
}
