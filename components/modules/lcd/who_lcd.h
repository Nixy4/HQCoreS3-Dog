/*
 * @Author: Kevincoooool
 * @Date: 2022-03-02 19:29:05
 * @Description: 
 * @version:  
 * @Filename: Do not Edit
 * @LastEditTime: 2023-07-17 11:29:18
 * @FilePath: \SP_V2_DEMO\25.face_reg\components\modules\lcd\who_lcd.h
 */
#pragma once

#include <stdint.h>
#include "esp_log.h"
#include "screen_driver.h"

#define BOARD_LCD_MOSI 47
#define BOARD_LCD_MISO -1
#define BOARD_LCD_SCK 21
#define BOARD_LCD_CS 14
#define BOARD_LCD_DC 45
#define BOARD_LCD_RST -1
#define BOARD_LCD_BL 48
#define BOARD_LCD_PIXEL_CLOCK_HZ (60 * 1000 * 1000)
#define BOARD_LCD_BK_LIGHT_ON_LEVEL 0
#define BOARD_LCD_BK_LIGHT_OFF_LEVEL !BOARD_LCD_BK_LIGHT_ON_LEVEL
#define BOARD_LCD_H_RES 240
#define BOARD_LCD_V_RES 240
#define BOARD_LCD_CMD_BITS 8
#define BOARD_LCD_PARAM_BITS 8
#define LCD_HOST SPI2_HOST

#ifdef __cplusplus
extern "C"
{
#endif

    esp_err_t register_lcd(const QueueHandle_t frame_i, const QueueHandle_t frame_o, const bool return_fb);

    void app_lcd_draw_wallpaper();
    void app_lcd_set_color(int color);

#ifdef __cplusplus
}
#endif
