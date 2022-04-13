/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2015. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
* AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*/

#define LOG_TAG "LCM"

#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif

#include "lcm_drv.h"

#ifdef BUILD_LK
#include <platform/upmu_common.h>
#include <platform/mt_gpio.h>
#include <platform/mt_i2c.h>
#include <platform/mt_pmic.h>
#include <string.h>
#ifndef MACH_FPGA
#include <lcm_pmic.h>
#endif
#endif

#ifdef BUILD_LK
#define LCM_LOGI(string, args...)  dprintf(ALWAYS, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(INFO, "[LK/"LOG_TAG"]"string, ##args)
#else
#define LCM_LOGI(fmt, args...)  pr_notice("[KERNEL/"LOG_TAG"]"fmt, ##args)
#define LCM_LOGD(fmt, args...)  pr_debug("[KERNEL/"LOG_TAG"]"fmt, ##args)
#endif


static struct LCM_UTIL_FUNCS lcm_util;
#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))
#ifdef BUILD_LK

#else
#define SET_LCM_ENP(v)	(lcm_util.set_gpio_lcd_enp_bias((v)))
#define SET_LCM_ENN(v)	(lcm_util.set_gpio_lcd_enn_bias((v)))
#endif
#define MDELAY(n)       (lcm_util.mdelay(n))
#define UDELAY(n)       (lcm_util.udelay(n))
/* --------------------------------------------------------------------------- */
/* Local Functions */
/* --------------------------------------------------------------------------- */

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) \
    lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) \
        lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd) lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) \
        lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd) \
      lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) \
        lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#ifndef BUILD_LK
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#endif
/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */
#define FRAME_WIDTH                                     (720)
#define FRAME_HEIGHT                                    (1600)
#define PHYSICAL_WIDTH                                  (6793/100)
#define PHYSICAL_HEIGHT                                 (15096/100)
#define PHYSICAL_WIDTH_UM                                  (67930)
#define PHYSICAL_HEIGHT_UM                                 (150960)
#define REGFLAG_DELAY       0xFFFC
#define REGFLAG_UDELAY  0xFFFB
#define REGFLAG_END_OF_TABLE    0xFFFD

struct LCM_setting_table {
	unsigned int cmd;
	unsigned char count;
	unsigned char para_list[64];
};

static struct LCM_setting_table lcm_suspend_setting[] = {
	{0x28, 0, {} },
	{REGFLAG_DELAY, 20, {} },
	{0x10, 0, {} },
	{REGFLAG_DELAY, 120, {} }
};

extern int tpd_load_status_nvt;
static int nt36525b_is_sc_flag = 0;
 
#ifdef BUILD_LK
static unsigned int GPIO_LCD_ENN = 66;
static unsigned int GPIO_LCD_ENP = 65;
#define sgm3804_SLAVE_ADDR_WRITE  0x7C
static struct mt_i2c_t sgm3804_i2c;

static int sgm3804_write_byte(kal_uint8 addr, kal_uint8 value)
{
	kal_uint32 ret_code = I2C_OK;
	kal_uint8 write_data[2];
	kal_uint16 len;

	write_data[0] = addr;
	write_data[1] = value;

	sgm3804_i2c.id = 3; /* I2C1; */
	/* Since i2c will left shift 1 bit, we need to set FAN5405 I2C address to >>1 */
	sgm3804_i2c.addr = (sgm3804_SLAVE_ADDR_WRITE >> 1);
	sgm3804_i2c.mode = ST_MODE;
	sgm3804_i2c.speed = 100;
	len = 2;

	ret_code = i2c_write(&sgm3804_i2c, write_data, len);
	/* printf("%s: i2c_write: ret_code: %d\n", __func__, ret_code); */

	return ret_code;
}
#endif

#ifdef BUILD_LK
static void tp_reset_ctl(int onoff)
{
	static unsigned int GPIO_TP_RESET = 174;
	mt_set_gpio_mode(GPIO_TP_RESET, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_TP_RESET, GPIO_DIR_OUT);
	
	if(onoff)
		mt_set_gpio_out(GPIO_TP_RESET, GPIO_OUT_ONE);
	else
		mt_set_gpio_out(GPIO_TP_RESET, GPIO_OUT_ZERO);
}
#else
extern void nvt_reset_pin_ctl(int onoff);

static void tp_reset_ctl(int onoff)
{
	nvt_reset_pin_ctl(onoff);
}
#endif

static void lcm_power_onoff(int onoff)
{
	if(onoff)
	{
		SET_RESET_PIN(0);
		tp_reset_ctl(0);
		MDELAY(10);
		SET_LCM_ENP(1);
		MDELAY(3);
		SET_LCM_ENN(1);
		MDELAY(20);
		tp_reset_ctl(1);
		MDELAY(10);
		SET_RESET_PIN(1);
		MDELAY(50);
	}
	else
	{
		SET_LCM_ENN(0);
		MDELAY(10);	
		SET_LCM_ENP(0);
		MDELAY(10);
	}
}

void nt36525b_sc_shutdown_power(void)
{
	if(tpd_load_status_nvt)
	{
		printk("wxs nt36525b_shutdown_power\n");
		tp_reset_ctl(0);
		MDELAY(5);
		SET_RESET_PIN(0);
		MDELAY(15);
		SET_LCM_ENN(0);
		MDELAY(10);
		SET_LCM_ENP(0);
		MDELAY(10);
	}
}

static struct LCM_setting_table init_setting[] = {
	{0xFF, 1, {0x20}},
	{0xFB, 1, {0x01}},
	{0x00, 1, {0x01}},
	{0x01, 1, {0x55}},
	{0x03, 1, {0x55}},
	{0x05, 1, {0xA9}},
	{0x07, 1, {0x73}},
	{0x08, 1, {0xC1}},
	{0x0E, 1, {0x91}},
	{0x0F, 1, {0x5F}},
	{0x1F, 1, {0x00}},
	{0x69, 1, {0xA9}},
	{0x94, 1, {0x00}},
	{0x95, 1, {0xEB}},
	{0x96, 1, {0xEB}},
	{0xFF, 1, {0x24}},
	{0xFB, 1, {0x01}},
	{0x00, 1, {0x1E}},
	{0x04, 1, {0x21}},
	{0x06, 1, {0x22}},
	{0x07, 1, {0x20}},
	{0x08, 1, {0x1D}},
	{0x0A, 1, {0x0C}},
	{0x0B, 1, {0x0D}},
	{0x0C, 1, {0x0E}},
	{0x0D, 1, {0x0F}},
	{0x0F, 1, {0x04}},
	{0x10, 1, {0x05}},
	{0x12, 1, {0x1E}},
	{0x13, 1, {0x1E}},
	{0x14, 1, {0x1E}},
	{0x16, 1, {0x1E}},
	{0x1A, 1, {0x21}},
	{0x1C, 1, {0x22}},
	{0x1D, 1, {0x20}},
	{0x1E, 1, {0x1D}},
	{0x20, 1, {0x0C}},
	{0x21, 1, {0x0D}},
	{0x22, 1, {0x0E}},
	{0x23, 1, {0x0F}},
	{0x25, 1, {0x04}},
	{0x26, 1, {0x05}},
	{0x28, 1, {0x1E}},
	{0x29, 1, {0x1E}},
	{0x2A, 1, {0x1E}},
	{0x2F, 1, {0x0C}},
	{0x30, 1, {0x0A}},
	{0x33, 1, {0x0A}},
	{0x34, 1, {0x0C}},
	{0x37, 1, {0x66}},
	{0x39, 1, {0x00}},
	{0x3A, 1, {0x10}},
	{0x3B, 1, {0x90}},
	{0x3D, 1, {0x92}},
	{0x4D, 1, {0x12}},
	{0x4E, 1, {0x34}},
	{0x51, 1, {0x43}},
	{0x52, 1, {0x21}},
	{0x55, 1, {0x87}},
	{0x56, 1, {0x44}},
	{0x5A, 1, {0x9F}},
	{0x5B, 1, {0x90}},
	{0x5C, 1, {0x00}},
	{0x5D, 1, {0x00}},
	{0x5E, 1, {0x04}},
	{0x5F, 1, {0x00}},
	{0x60, 1, {0x80}},
	{0x61, 1, {0x90}},
	{0x64, 1, {0x11}},
	{0x82, 1, {0x0D}},
	{0x83, 1, {0x05}},
	{0x85, 1, {0x00}},
	{0x86, 1, {0x51}},
	{0x92, 1, {0xAD}},
	{0x93, 1, {0x08}},
	{0x94, 1, {0x0E}},
	{0xAB, 1, {0x00}},
	{0xAC, 1, {0x00}},
	{0xAD, 1, {0x00}},
	{0xAF, 1, {0x04}},
	{0xB0, 1, {0x05}},
	{0xB1, 1, {0xA8}},
	{0xC2, 1, {0x86}},
	{0xFF, 1, {0x25}},
	{0xFB, 1, {0x01}},
	{0x0A, 1, {0x82}},
	{0x0B, 1, {0x26}},
	{0x0C, 1, {0x01}},
	{0x17, 1, {0x82}},
	{0x18, 1, {0x06}},
	{0x19, 1, {0x0F}},
	{0x58, 1, {0x00}},
	{0x59, 1, {0x00}},
	{0x5A, 1, {0x40}},
	{0x5B, 1, {0x80}},
	{0x5C, 1, {0x00}},
	{0x5D, 1, {0x9F}},
	{0x5E, 1, {0x90}},
	{0x5F, 1, {0x9F}},
	{0x60, 1, {0x90}},
	{0x61, 1, {0x9F}},
	{0x62, 1, {0x90}},
	{0x65, 1, {0x05}},
	{0x66, 1, {0xA8}},
	{0xC0, 1, {0x03}},
	{0xCA, 1, {0x1C}},
	{0xCC, 1, {0x1C}},
	{0xD3, 1, {0x11}},
	{0xD4, 1, {0xC8}},
	{0xD5, 1, {0x11}},
	{0xD6, 1, {0x1C}},
	{0xD7, 1, {0x11}},
	{0xFF, 1, {0x26}},
	{0xFB, 1, {0x01}},
	{0x00, 1, {0xA0}},
	{0xFF, 1, {0x27}},
	{0xFB, 1, {0x01}},
	{0x13, 1, {0x00}},
	{0x15, 1, {0xB4}},
	{0x1F, 1, {0x55}},
	{0x26, 1, {0x0F}},
	{0xC0, 1, {0x18}},
	{0xC1, 1, {0xF0}},
	{0xC2, 1, {0x00}},
	{0xC3, 1, {0x00}},
	{0xC4, 1, {0xF0}},
	{0xC5, 1, {0x00}},
	{0xC6, 1, {0x00}},
	{0xC7, 1, {0x03}},
	{0xFF, 1, {0x23}},
	{0xFB, 1, {0x01}},
	{0x07, 1, {0x20}},
	{0x08, 1, {0x0F}},
	{0x12, 1, {0xB4}},
	{0x15, 1, {0xE9}},
	{0x16, 1, {0x0B}},
	{0xFF, 1, {0x20}},
	{0xFB, 1, {0x01}},
	{0xB0, 1, {0x00, 0x08, 0x00, 0x18, 0x00, 0x33, 0x00, 0x4B, 0x00, 0x5F, 0x00, 0x72, 0x00, 0x83, 0x00, 0x94}},
	{0xB1, 1, {0x00, 0xA2, 0x00, 0xD7, 0x00, 0xFF, 0x01, 0x41, 0x01, 0x6F, 0x01, 0xBD, 0x01, 0xF8, 0x01, 0xFA}},
	{0xB2, 1, {0x02, 0x36, 0x02, 0x71, 0x02, 0x9C, 0x02, 0xD0, 0x02, 0xF5, 0x03, 0x20, 0x03, 0x30, 0x03, 0x3E}},
	{0xB3, 1, {0x03, 0x4F, 0x03, 0x63, 0x03, 0x7B, 0x03, 0x95, 0x03, 0xA6, 0x03, 0xA7}},
	{0xB4, 1, {0x00, 0x08, 0x00, 0x18, 0x00, 0x33, 0x00, 0x4B, 0x00, 0x5F, 0x00, 0x72, 0x00, 0x83, 0x00, 0x94}},
	{0xB5, 1, {0x00, 0xA2, 0x00, 0xD7, 0x00, 0xFF, 0x01, 0x41, 0x01, 0x6F, 0x01, 0xBD, 0x01, 0xF8, 0x01, 0xFA}},
	{0xB6, 1, {0x02, 0x36, 0x02, 0x71, 0x02, 0x9C, 0x02, 0xD0, 0x02, 0xF5, 0x03, 0x20, 0x03, 0x30, 0x03, 0x3E}},
	{0xB7, 1, {0x03, 0x4F, 0x03, 0x63, 0x03, 0x7B, 0x03, 0x95, 0x03, 0xA6, 0x03, 0xA7}},
	{0xB8, 1, {0x00, 0x08, 0x00, 0x18, 0x00, 0x33, 0x00, 0x4B, 0x00, 0x5F, 0x00, 0x72, 0x00, 0x83, 0x00, 0x94}},
	{0xB9, 1, {0x00, 0xA2, 0x00, 0xD7, 0x00, 0xFF, 0x01, 0x41, 0x01, 0x6F, 0x01, 0xBD, 0x01, 0xF8, 0x01, 0xFA}},
	{0xBA, 1, {0x02, 0x36, 0x02, 0x71, 0x02, 0x9C, 0x02, 0xD0, 0x02, 0xF5, 0x03, 0x20, 0x03, 0x30, 0x03, 0x3E}},
	{0xBB, 1, {0x03, 0x4F, 0x03, 0x63, 0x03, 0x7B, 0x03, 0x95, 0x03, 0xA6, 0x03, 0xA7}},
	{0xFF, 1, {0x21}},
	{0xFB, 1, {0x01}},
	{0xB0, 1, {0x00, 0x00, 0x00, 0x10, 0x00, 0x2B, 0x00, 0x43, 0x00, 0x57, 0x00, 0x6A, 0x00, 0x7B, 0x00, 0x8C}},
	{0xB1, 1, {0x00, 0x9A, 0x00, 0xCF, 0x00, 0xF7, 0x01, 0x39, 0x01, 0x67, 0x01, 0xB5, 0x01, 0xF0, 0x01, 0xF2}},
	{0xB2, 1, {0x02, 0x2E, 0x02, 0x7B, 0x02, 0xAE, 0x02, 0xEA, 0x03, 0x13, 0x03, 0x42, 0x03, 0x52, 0x03, 0x62}},
	{0xB3, 1, {0x03, 0x73, 0x03, 0x89, 0x03, 0xA1, 0x03, 0xBD, 0x03, 0xCE, 0x03, 0xD9}},
	{0xB4, 1, {0x00, 0x00, 0x00, 0x10, 0x00, 0x2B, 0x00, 0x43, 0x00, 0x57, 0x00, 0x6A, 0x00, 0x7B, 0x00, 0x8C}},
	{0xB5, 1, {0x00, 0x9A, 0x00, 0xCF, 0x00, 0xF7, 0x01, 0x39, 0x01, 0x67, 0x01, 0xB5, 0x01, 0xF0, 0x01, 0xF2}},
	{0xB6, 1, {0x02, 0x2E, 0x02, 0x7B, 0x02, 0xAE, 0x02, 0xEA, 0x03, 0x13, 0x03, 0x42, 0x03, 0x52, 0x03, 0x62}},
	{0xB7, 1, {0x03, 0x73, 0x03, 0x89, 0x03, 0xA1, 0x03, 0xBD, 0x03, 0xCE, 0x03, 0xD9}},
	{0xB8, 1, {0x00, 0x00, 0x00, 0x10, 0x00, 0x2B, 0x00, 0x43, 0x00, 0x57, 0x00, 0x6A, 0x00, 0x7B, 0x00, 0x8C}},
	{0xB9, 1, {0x00, 0x9A, 0x00, 0xCF, 0x00, 0xF7, 0x01, 0x39, 0x01, 0x67, 0x01, 0xB5, 0x01, 0xF0, 0x01, 0xF2}},
	{0xBA, 1, {0x02, 0x2E, 0x02, 0x7B, 0x02, 0xAE, 0x02, 0xEA, 0x03, 0x13, 0x03, 0x42, 0x03, 0x52, 0x03, 0x62}},
	{0xBB, 1, {0x03, 0x73, 0x03, 0x89, 0x03, 0xA1, 0x03, 0xBD, 0x03, 0xCE, 0x03, 0xD9}},
	{0xFF, 1, {0x10}},
	{0xFB, 1, {0x01}},
	{0x53, 1, {0x24}},
	{0x51, 1, {0x88}},
	{0x35, 1, {0x00}},

	{0x11, 0, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	{0x29, 0, {0x00}},
	{REGFLAG_DELAY, 20, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

	for (i = 0; i < count; i++) {
		unsigned cmd;
		cmd = table[i].cmd;

		switch (cmd) {

			case REGFLAG_DELAY:
				if (table[i].count <= 10)
					MDELAY(table[i].count);
				else
					MDELAY(table[i].count);
				break;

			case REGFLAG_UDELAY:
				UDELAY(table[i].count);
				break;

			case REGFLAG_END_OF_TABLE:
				break;

			default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
		}
	}
}

/* --------------------------------------------------------------------------- */
/* LCM Driver Implementations */
/* --------------------------------------------------------------------------- */

static void lcm_set_util_funcs(const struct LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(struct LCM_UTIL_FUNCS));
}

static void lcm_get_params(struct LCM_PARAMS *params)
{
	memset(params, 0, sizeof(struct LCM_PARAMS));

	params->type = LCM_TYPE_DSI;

	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;
	params->physical_width = PHYSICAL_WIDTH;
	params->physical_height = PHYSICAL_HEIGHT;
	params->physical_width_um = PHYSICAL_WIDTH_UM;
	params->physical_height_um = PHYSICAL_HEIGHT_UM;
	params->dsi.switch_mode_enable = 0;
	params->dsi.mode = SYNC_PULSE_VDO_MODE;
	params->dsi.switch_mode = CMD_MODE;

	/* DSI */
	/* Command mode setting */
	params->dsi.LANE_NUM = LCM_FOUR_LANE;
	/* The following defined the fomat for data coming from LCD engine. */
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

	/* Highly depends on LCD driver capability. */
	params->dsi.packet_size = 256;
	/* video mode timing */

	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;
	params->dsi.vertical_sync_active = 2;
	params->dsi.vertical_backporch = 252;
	params->dsi.vertical_frontporch = 8;
	params->dsi.vertical_active_line = FRAME_HEIGHT;
	params->dsi.horizontal_sync_active = 30;
	params->dsi.horizontal_backporch = 78;
	params->dsi.horizontal_frontporch = 74;
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;
	params->dsi.ssc_disable = 1;
	params->dsi.PLL_CLOCK = 331; /* this value must be in MTK suggested table */
	params->dsi.clk_lp_per_line_enable = 0;
	params->dsi.esd_check_enable = 0;
	params->dsi.customization_esd_check_enable = 0;
	params->dsi.lcm_esd_check_table[0].cmd = 0x0A;
	params->dsi.lcm_esd_check_table[0].count = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;
	nt36525b_is_sc_flag = 1;
}

int nt36525b_is_sc_module(void)
{
	return nt36525b_is_sc_flag;
}

static void lcm_init(void)
{
	lcm_power_onoff(1);
	push_table(init_setting, sizeof(init_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
	if(tpd_load_status_nvt)
	{
		push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);
		MDELAY(50);
		lcm_power_onoff(0);
	}
	else
	{
		lcm_power_onoff(0);
	}
}

static void lcm_resume(void)
{
	lcm_init();
}

static unsigned int lcm_compare_id(void)
{
	unsigned int version_id = 0;
	unsigned char buffer[2];
	unsigned int array[16];

	array[0] = 0x00013700;	/* read id return two byte,version and id */
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0xda, buffer, 1);
	version_id = buffer[0]; 	/* we only need ID */

	LCM_LOGI("%s,nt36525b 0xda version_id=0x%x\n", __func__, version_id);

	if(version_id==0x12)
		return 1;
	else
		return 0;
}

static unsigned int lcm_ata_check(unsigned char *buffer)
{
	unsigned int version_id = 0;
	unsigned char ata_check_buffer[2];
	unsigned int array[16];

	array[0] = 0x00013700;  /* read id return two byte,version and id */
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0xda, ata_check_buffer, 1);
	version_id = ata_check_buffer[0];       /* we only need ID */

	LCM_LOGI("%s,nt36525b sc 0xda version_id=0x%x\n", __func__, version_id);

	if (version_id == 0x13)
		return 1;
	else
		return 0;
}


struct LCM_DRIVER nt36525b_720x1600_sc_incell_drv = {
	.name = "nt36525b_720x1600_sc_incell",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
	.compare_id = lcm_compare_id,
	.ata_check = lcm_ata_check,
};
