// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _OV2640_SETTINGS_H_
#define _OV2640_SETTINGS_H_

#include <stdint.h>
#include <stdbool.h>
#include "esp_attr.h"
#include "ov2640_regs.h"

typedef enum {
    OV2640_MODE_UXGA, OV2640_MODE_SVGA, OV2640_MODE_CIF, OV2640_MODE_MAX
} ov2640_sensor_mode_t;

typedef struct {
        union {
                struct {
                        uint8_t pclk_div:7;
                        uint8_t pclk_auto:1;
                };
                uint8_t pclk;
        };
        union {
                struct {
                        uint8_t clk_div:6;
                        uint8_t reserved:1;
                        uint8_t clk_2x:1;
                };
                uint8_t clk;
        };
} ov2640_clk_t;

typedef struct {
        uint16_t offset_x;
        uint16_t offset_y;
        uint16_t max_x;
        uint16_t max_y;
} ov2640_ratio_settings_t;

static const DRAM_ATTR ov2640_ratio_settings_t ratio_table[] = {
    // ox,  oy,   mx,   my
    {   0,   0, 1600, 1200 }, //4x3
    {   8,  72, 1584, 1056 }, //3x2
    {   0, 100, 1600, 1000 }, //16x10
    {   0, 120, 1600,  960 }, //5x3
    {   0, 150, 1600,  900 }, //16x9
    {   2, 258, 1596,  684 }, //21x9
    {  50,   0, 1500, 1200 }, //5x4
    { 200,   0, 1200, 1200 }, //1x1
    { 462,   0,  676, 1200 }  //9x16
};

// 30fps@24MHz
const DRAM_ATTR uint8_t ov2640_settings_cif[][2] = {
    {BANK_SEL, BANK_DSP},
    {0x2c, 0xff},
    {0x2e, 0xdf},
    {BANK_SEL, BANK_SENSOR},
    {0x3c, 0x50},
    {CLKRC, 0x01},
    {COM2, COM2_OUT_DRIVE_3x},
    {REG04, REG04_DEFAULT},
    {COM8, COM8_DEFAULT | COM8_BNDF_EN | COM8_AGC_EN | COM8_AEC_EN},
    {COM9, COM9_AGC_SET(COM9_AGC_GAIN_8x)},
    {0x2c, 0x0c},
    {0x33, 0x78},
    {0x3a, 0x33},
    {0x3b, 0xfB},
    {0x3e, 0x00},
    {0x43, 0x11},
    {0x16, 0x10},
    {0x39, 0x92},
    {0x35, 0xda},
    {0x22, 0x1a},
    {0x37, 0xc3},
    {0x23, 0x00},
    {ARCOM2, 0xc0},
    {0x06, 0x88},
    {0x07, 0xc0},
    {COM4, 0x87},
    {0x0e, 0x41},
    {0x4c, 0x00},
    {0x4a, 0x81},
    {0x21, 0x99},
    {AEW, 0x40},
    {AEB, 0x38},
    {VV, VV_AGC_TH_SET(8,2)},
    {0x5c, 0x00},
    {0x63, 0x00},
    {HISTO_LOW, 0x70},
    {HISTO_HIGH, 0x80},
    {0x7c, 0x05},
    {0x20, 0x80},
    {0x28, 0x30},
    {0x6c, 0x00},
    {0x6d, 0x80},
    {0x6e, 0x00},
    {0x70, 0x02},
    {0x71, 0x94},
    {0x73, 0xc1},
    {0x3d, 0x34},
    {0x5a, 0x57},
    {BD50, 0xbb},
    {BD60, 0x9c},
    {COM7, COM7_RES_CIF},
    {HSTART, 0x11},
    {HSTOP, 0x43},
    {VSTART, 0x00},
    {VSTOP, 0x25},
    {REG32, 0x89},
    {0x37, 0xc0},
    {BD50, 0xca},
    {BD60, 0xa8},
    {0x6d, 0x00},
    {0x3d, 0x38},
    {BANK_SEL, BANK_DSP},
    {0xe5, 0x7f},
    {MC_BIST, MC_BIST_RESET | MC_BIST_BOOT_ROM_SEL},
    {0x41, 0x24},
    {RESET, RESET_JPEG | RESET_DVP},
    {0x76, 0xff},
    {0x33, 0xa0},
    {0x42, 0x20},
    {0x43, 0x18},
    {0x4c, 0x00},
    {CTRL3, CTRL3_WPC_EN | 0x10 },
    {0x88, 0x3f},
    {0xd7, 0x03},
    {0xd9, 0x10},
    {R_DVP_SP, R_DVP_SP_AUTO_MODE | 0x02},
    {0xc8, 0x08},
    {0xc9, 0x80},
    {BPADDR, 0x00},
    {BPDATA, 0x00},
    {BPADDR, 0x03},
    {BPDATA, 0x48},
    {BPDATA, 0x48},
    {BPADDR, 0x08},
    {BPDATA, 0x20},
    {BPDATA, 0x10},
    {BPDATA, 0x0e},
    {0x90, 0x00},
    {0x91, 0x0e},
    {0x91, 0x1a},
    {0x91, 0x31},
    {0x91, 0x5a},
    {0x91, 0x69},
    {0x91, 0x75},
    {0x91, 0x7e},
    {0x91, 0x88},
    {0x91, 0x8f},
    {0x91, 0x96},
    {0x91, 0xa3},
    {0x91, 0xaf},
    {0x91, 0xc4},
    {0x91, 0xd7},
    {0x91, 0xe8},
    {0x91, 0x20},
    {0x92, 0x00},
    {0x93, 0x06},
    {0x93, 0xe3},
    {0x93, 0x05},
    {0x93, 0x05},
    {0x93, 0x00},
    {0x93, 0x04},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x96, 0x00},
    {0x97, 0x08},
    {0x97, 0x19},
    {0x97, 0x02},
    {0x97, 0x0c},
    {0x97, 0x24},
    {0x97, 0x30},
    {0x97, 0x28},
    {0x97, 0x26},
    {0x97, 0x02},
    {0x97, 0x98},
    {0x97, 0x80},
    {0x97, 0x00},
    {0x97, 0x00},
    {0xa4, 0x00},
    {0xa8, 0x00},
    {0xc5, 0x11},
    {0xc6, 0x51},
    {0xbf, 0x80},
    {0xc7, 0x10},
    {0xb6, 0x66},
    {0xb8, 0xA5},
    {0xb7, 0x64},
    {0xb9, 0x7C},
    {0xb3, 0xaf},
    {0xb4, 0x97},
    {0xb5, 0xFF},
    {0xb0, 0xC5},
    {0xb1, 0x94},
    {0xb2, 0x0f},
    {0xc4, 0x5c},
    {CTRL1, 0xfd},
    {0x7f, 0x00},
    {0xe5, 0x1f},
    {0xe1, 0x67},
    {0xdd, 0x7f},
    {IMAGE_MODE, 0x00},
    {RESET, 0x00},
    {R_BYPASS, R_BYPASS_DSP_EN},
    {0, 0}
};

const DRAM_ATTR uint8_t ov2640_settings_to_cif[][2] = {
    {BANK_SEL, BANK_SENSOR},
    {COM7, COM7_RES_CIF},

    //Set the sensor output window
    {COM1, 0x0A},
    {REG32, REG32_CIF},
    {HSTART, 0x11},
    {HSTOP, 0x43},
    {VSTART, 0x00},
    {VSTOP, 0x25},

    //{CLKRC, 0x00},
    {BD50, 0xca},
    {BD60, 0xa8},
    {0x5a, 0x23},
    {0x6d, 0x00},
    {0x3d, 0x38},
    {0x39, 0x92},
    {0x35, 0xda},
    {0x22, 0x1a},
    {0x37, 0xc3},
    {0x23, 0x00},
    {ARCOM2, 0xc0},
    {0x06, 0x88},
    {0x07, 0xc0},
    {COM4, 0x87},
    {0x0e, 0x41},
    {0x4c, 0x00},
    {BANK_SEL, BANK_DSP},
    {RESET, RESET_DVP},

    //Set the sensor resolution (UXGA, SVGA, CIF)
    {HSIZE8, 0x32},
    {VSIZE8, 0x25},
    {SIZEL, 0x00},

    //Set the image window size >= output size
    {HSIZE, 0x64},
    {VSIZE, 0x4a},
    {XOFFL, 0x00},
    {YOFFL, 0x00},
    {VHYX, 0x00},
    {TEST, 0x00},

    {CTRL2, CTRL2_DCW_EN | 0x1D},
    {CTRLI, CTRLI_LP_DP | 0x00},
    //{R_DVP_SP, 0x08},
    {0, 0}
};

const DRAM_ATTR uint8_t ov2640_settings_to_svga[][2] = {
    {BANK_SEL, BANK_SENSOR},
    {COM7, COM7_RES_SVGA},

    //Set the sensor output window
    {COM1, 0x0A},
    {REG32, REG32_SVGA},
    {HSTART, 0x11},
    {HSTOP, 0x43},
    {VSTART, 0x00},
    {VSTOP, 0x4b},

    //{CLKRC, 0x00},
    {0x37, 0xc0},
    {BD50, 0xca},
    {BD60, 0xa8},
    {0x5a, 0x23},
    {0x6d, 0x00},
    {0x3d, 0x38},
    {0x39, 0x92},
    {0x35, 0xda},
    {0x22, 0x1a},
    {0x37, 0xc3},
    {0x23, 0x00},
    {ARCOM2, 0xc0},
    {0x06, 0x88},
    {0x07, 0xc0},
    {COM4, 0x87},
    {0x0e, 0x41},
    {0x42, 0x03},
    {0x4c, 0x00},
    {BANK_SEL, BANK_DSP},
    {RESET, RESET_DVP},

    //Set the sensor resolution (UXGA, SVGA, CIF)
    {HSIZE8, 0x64},
    {VSIZE8, 0x4B},
    {SIZEL, 0x00},

    //Set the image window size >= output size
    {HSIZE, 0xC8},
    {VSIZE, 0x96},
    {XOFFL, 0x00},
    {YOFFL, 0x00},
    {VHYX, 0x00},
    {TEST, 0x00},

    {CTRL2, CTRL2_DCW_EN | 0x1D},
    {CTRLI, CTRLI_LP_DP | 0x00},
    //{R_DVP_SP, 0x08},
    {0, 0}
};

const DRAM_ATTR uint8_t ov2640_settings_to_uxga[][2] = {
    {BANK_SEL, BANK_SENSOR},
    {COM7, COM7_RES_UXGA},

    //Set the sensor output window
    {COM1, 0x0F},
    {REG32, REG32_UXGA},
    {HSTART, 0x11},
    {HSTOP, 0x75},
    {VSTART, 0x01},
    {VSTOP, 0x97},

    //{CLKRC, 0x00},
    {0x3d, 0x34},
    {BD50, 0xbb},
    {BD60, 0x9c},
    {0x5a, 0x57},
    {0x6d, 0x80},
    {0x39, 0x82},
    {0x23, 0x00},
    {0x07, 0xc0},
    {0x4c, 0x00},
    {0x35, 0x88},
    {0x22, 0x0a},
    {0x37, 0x40},
    {ARCOM2, 0xa0},
    {0x06, 0x02},
    {COM4, 0xb7},
    {0x0e, 0x01},
    {0x42, 0x83},
    {BANK_SEL, BANK_DSP},
    {RESET, RESET_DVP},

    //Set the sensor resolution (UXGA, SVGA, CIF)
    {HSIZE8, 0xc8},
    {VSIZE8, 0x96},
    {SIZEL, 0x00},

    //Set the image window size >= output size
    {HSIZE, 0x90},
    {VSIZE, 0x2c},
    {XOFFL, 0x00},
    {YOFFL, 0x00},
    {VHYX, 0x88},
    {TEST, 0x00},

    {CTRL2, CTRL2_DCW_EN | 0x1d},
    {CTRLI, 0x00},
    //{R_DVP_SP, 0x06},
    {0, 0}
};

const DRAM_ATTR uint8_t ov2640_settings_jpeg3[][2] = {
    {BANK_SEL, BANK_DSP},
    {RESET, RESET_JPEG | RESET_DVP},
    {IMAGE_MODE, IMAGE_MODE_JPEG_EN | IMAGE_MODE_HREF_VSYNC},
    {0xD7, 0x03},
    {0xE1, 0x77},
    {0xE5, 0x1F},
    {0xD9, 0x10},
    {0xDF, 0x80},
    {0x33, 0x80},
    {0x3C, 0x10},
    {0xEB, 0x30},
    {0xDD, 0x7F},
    {RESET, 0x00},
    {0, 0}
};

static const uint8_t ov2640_settings_yuv422[][2] = {
    {BANK_SEL, BANK_DSP},
    {RESET, RESET_DVP},
    {IMAGE_MODE, IMAGE_MODE_YUV422},
    {0xD7, 0x01},
    {0xE1, 0x67},
    {RESET, 0x00},
    {0, 0},
};

static const uint8_t ov2640_settings_rgb565[][2] = {
    {BANK_SEL, BANK_DSP},
    {RESET, RESET_DVP},
    {IMAGE_MODE, IMAGE_MODE_RGB565},
    {0xD7, 0x03},
    {0xE1, 0x77},
    {RESET, 0x00},
    {0, 0},
};

#define NUM_BRIGHTNESS_LEVELS (5)
static const uint8_t brightness_regs[NUM_BRIGHTNESS_LEVELS + 1][5] = {
    {BPADDR, BPDATA, BPADDR, BPDATA, BPDATA },
    {0x00, 0x04, 0x09, 0x00, 0x00 }, /* -2 */
    {0x00, 0x04, 0x09, 0x10, 0x00 }, /* -1 */
    {0x00, 0x04, 0x09, 0x20, 0x00 }, /*  0 */
    {0x00, 0x04, 0x09, 0x30, 0x00 }, /* +1 */
    {0x00, 0x04, 0x09, 0x40, 0x00 }, /* +2 */
};

#define NUM_CONTRAST_LEVELS (5)
static const uint8_t contrast_regs[NUM_CONTRAST_LEVELS + 1][7] = {
    {BPADDR, BPDATA, BPADDR, BPDATA, BPDATA, BPDATA, BPDATA },
    {0x00, 0x04, 0x07, 0x20, 0x18, 0x34, 0x06 }, /* -2 */
    {0x00, 0x04, 0x07, 0x20, 0x1c, 0x2a, 0x06 }, /* -1 */
    {0x00, 0x04, 0x07, 0x20, 0x20, 0x20, 0x06 }, /*  0 */
    {0x00, 0x04, 0x07, 0x20, 0x24, 0x16, 0x06 }, /* +1 */
    {0x00, 0x04, 0x07, 0x20, 0x28, 0x0c, 0x06 }, /* +2 */
};

#define NUM_SATURATION_LEVELS (5)
static const uint8_t saturation_regs[NUM_SATURATION_LEVELS + 1][5] = {
    {BPADDR, BPDATA, BPADDR, BPDATA, BPDATA },
    {0x00, 0x02, 0x03, 0x28, 0x28 }, /* -2 */
    {0x00, 0x02, 0x03, 0x38, 0x38 }, /* -1 */
    {0x00, 0x02, 0x03, 0x48, 0x48 }, /*  0 */
    {0x00, 0x02, 0x03, 0x58, 0x58 }, /* +1 */
    {0x00, 0x02, 0x03, 0x68, 0x68 }, /* +2 */
};

#define NUM_SPECIAL_EFFECTS (7)
static const uint8_t special_effects_regs[NUM_SPECIAL_EFFECTS + 1][5] = {
    {BPADDR, BPDATA, BPADDR, BPDATA, BPDATA },
    {0x00, 0X00, 0x05, 0X80, 0X80 }, /* no effect */
    {0x00, 0X40, 0x05, 0X80, 0X80 }, /* negative */
    {0x00, 0X18, 0x05, 0X80, 0X80 }, /* black and white */
    {0x00, 0X18, 0x05, 0X40, 0XC0 }, /* reddish */
    {0x00, 0X18, 0x05, 0X40, 0X40 }, /* greenish */
    {0x00, 0X18, 0x05, 0XA0, 0X40 }, /* blue */
    {0x00, 0X18, 0x05, 0X40, 0XA6 }, /* retro */
};

#define NUM_WB_MODES (4)
static const uint8_t wb_modes_regs[NUM_WB_MODES + 1][3] = {
    {0XCC, 0XCD, 0XCE },
    {0x5E, 0X41, 0x54 }, /* sunny */
    {0x65, 0X41, 0x4F }, /* cloudy */
    {0x52, 0X41, 0x66 }, /* office */
    {0x42, 0X3F, 0x71 }, /* home */
};

#define NUM_AE_LEVELS (5)
static const uint8_t ae_levels_regs[NUM_AE_LEVELS + 1][3] = {
    { AEW,  AEB,  VV  },
    {0x20, 0X18, 0x60 },
    {0x34, 0X1C, 0x00 },
    {0x3E, 0X38, 0x81 },
    {0x48, 0X40, 0x81 },
    {0x58, 0X50, 0x92 },
};

const uint8_t agc_gain_tbl[31] = {
    0x00, 0x10, 0x18, 0x30, 0x34, 0x38, 0x3C, 0x70, 0x72, 0x74, 0x76, 0x78, 0x7A, 0x7C, 0x7E, 0xF0,
    0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

#endif /* _OV2640_SETTINGS_H_ */
