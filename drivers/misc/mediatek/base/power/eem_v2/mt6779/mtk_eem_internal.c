// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

/**
 * @file	mtk_eem_internal.c
 * @brief   Driver for EEM
 *
 */
#define __MTK_EEM_INTERNAL_C__

#include "mtk_eem_config.h"
#include "mtk_eem.h"
#include "mtk_eem_internal_ap.h"
#include "mtk_eem_internal.h"

/**
 * EEM controllers
 */
struct eem_ctrl eem_ctrls[NR_EEM_CTRL] = {
	[EEM_CTRL_L] = {
		.name = __stringify(EEM_CTRL_L),
		.det_id = EEM_DET_L,
	},

	[EEM_CTRL_B] = {
		.name = __stringify(EEM_CTRL_B),
		.det_id = EEM_DET_B,
	},

	[EEM_CTRL_CCI] = {
		.name = __stringify(EEM_CTRL_CCI),
		.det_id = EEM_DET_CCI,
	},
	[EEM_CTRL_GPU] = {
		.name = __stringify(EEM_CTRL_GPU),
		.det_id = EEM_DET_GPU,
	},
#if ENABLE_MDLA
	[EEM_CTRL_MDLA] = {
		.name = __stringify(EEM_CTRL_MDLA),
		.det_id = EEM_DET_MDLA,
	},
#endif
#if ENABLE_VPU
	[EEM_CTRL_VPU] = {
		.name = __stringify(EEM_CTRL_VPU),
		.det_id = EEM_DET_VPU,
	},
#endif
#if ENABLE_LOO
	[EEM_CTRL_GPU_HI] = {
		.name = __stringify(EEM_CTRL_GPU_HI),
		.det_id = EEM_DET_GPU_HI,
	},
#endif

};

#define BASE_OP(fn)	.fn = base_ops_ ## fn
struct eem_det_ops eem_det_base_ops = {
	BASE_OP(enable),
	BASE_OP(disable),
	BASE_OP(disable_locked),
	BASE_OP(switch_bank),

	BASE_OP(init01),
	BASE_OP(init02),
	BASE_OP(mon_mode),

	BASE_OP(get_status),
	BASE_OP(dump_status),

	BASE_OP(set_phase),

	BASE_OP(get_temp),

	BASE_OP(get_volt),
	BASE_OP(set_volt),
	BASE_OP(restore_default_volt),
	BASE_OP(get_freq_table),
	BASE_OP(get_orig_volt_table),

	/* platform independent code */
	BASE_OP(volt_2_pmic),
	BASE_OP(volt_2_eem),
	BASE_OP(pmic_2_volt),
	BASE_OP(eem_2_pmic),
};

struct eem_det eem_detectors[NR_EEM_DET] = {
	[EEM_DET_L] = {
		.name		= __stringify(EEM_DET_L),
		.ops		= &cpu_det_ops,
#ifdef EEM_OFFSET_PROC_SHOW
		.volt_offset	= 0,
#endif
		.ctrl_id	= EEM_CTRL_L,
		.features	= FEA_INIT01 | FEA_INIT02,
		.max_freq_khz	= 2000000,
		.VBOOT		= VBOOT_VAL, /* 10uV */
		.eem_v_base	= EEM_V_BASE,
		.eem_step	= EEM_STEP,
		.pmic_base	= CPU_PMIC_BASE_6359,
		.pmic_step	= CPU_PMIC_STEP,
		.DETWINDOW	= DETWINDOW_VAL,
		.DTHI		= DTHI_VAL,
		.DTLO		= DTLO_VAL,
		.DETMAX		= DETMAX_VAL,
		.AGECONFIG	= AGECONFIG_VAL,
		.AGEM		= AGEM_VAL,
		.VMAX		= VMAX_VAL,
		.VMIN		= VMIN_VAL,
		.VCO		= VCO_VAL,
		.DVTFIXED	= DVTFIXED_VAL,
		.DCCONFIG	= DCCONFIG_VAL,
		.EEMCTL0	= EEM_CTL0_L,
		.low_temp_off	= LOW_TEMP_OFF_DEFAULT,
	},

	[EEM_DET_B] = {
		.name		= __stringify(EEM_DET_B),
		.ops		= &cpu_det_ops,
#ifdef EEM_OFFSET_PROC_SHOW
		.volt_offset	= 0,
#endif
		.ctrl_id	= EEM_CTRL_B,
		.features	= FEA_INIT01 | FEA_INIT02,
		.max_freq_khz	= 2200000,
		.VBOOT		= VBOOT_VAL, /* 10uV */
		.eem_v_base	= EEM_V_BASE,
		.eem_step	= EEM_STEP,
		.pmic_base	= CPU_PMIC_BASE_6359,
		.pmic_step	= CPU_PMIC_STEP,
		.DETWINDOW      = DETWINDOW_VAL,
		.DTHI		= DTHI_VAL,
		.DTLO		= DTLO_VAL,
		.DETMAX		= DETMAX_VAL,
		.AGECONFIG	= AGECONFIG_VAL,
		.AGEM		= AGEM_VAL,
		.VMAX		= VMAX_VAL_B,
		.VMIN		= VMIN_VAL_B,
		.VCO		= VCO_VAL_B,
		.DVTFIXED	= DVTFIXED_VAL_B,
		.DCCONFIG	= DCCONFIG_VAL,
		.EEMCTL0	= EEM_CTL0_B,
		.low_temp_off	= LOW_TEMP_OFF_DEFAULT,
	},

	[EEM_DET_CCI] = {
		.name		= __stringify(EEM_DET_CCI),
		.ops		= &cci_det_ops,
#ifdef EEM_OFFSET_PROC_SHOW
		.volt_offset = 0,
#endif
		.ctrl_id	= EEM_CTRL_CCI,
		.features	= FEA_INIT01 | FEA_INIT02,
		.max_freq_khz	= 1400000,
		.VBOOT		= VBOOT_VAL, /* 10uV */
		.VMAX		= VMAX_VAL_CCI,
		.VMIN		= VMIN_VAL_CCI,
		.eem_v_base	= EEM_V_BASE,
		.eem_step	= EEM_STEP,
		.pmic_base	= CPU_PMIC_BASE_6359,
		.pmic_step	= CPU_PMIC_STEP,
		.DETWINDOW      = DETWINDOW_VAL,
		.DTHI		= DTHI_VAL,
		.DTLO		= DTLO_VAL,
		.DETMAX		= DETMAX_VAL,
		.AGECONFIG	= AGECONFIG_VAL,
		.AGEM		= AGEM_VAL,
		.DVTFIXED	= DVTFIXED_VAL_CCI,
		.VCO		= VCO_VAL_CCI,
		.DCCONFIG	= DCCONFIG_VAL,
		.EEMCTL0	= EEM_CTL0_CCI,
		.low_temp_off	= LOW_TEMP_OFF_DEFAULT,
	},

	[EEM_DET_GPU] = {
		.name		= __stringify(EEM_DET_GPU),
		.ops		= &gpu_det_ops,
#ifdef EEM_OFFSET_PROC_SHOW
		.volt_offset	= 0,
#endif
		.ctrl_id	= EEM_CTRL_GPU,
		.features	= FEA_INIT01 | FEA_INIT02,
#if ENABLE_LOO
		.max_freq_khz	= 640000,/* MHz */
		.VMAX		= VMAX_VAL_GL,
#else
		.max_freq_khz   = 970000,
		.VMAX		= VMAX_VAL_GPU,
#endif
		.VBOOT		= VBOOT_VAL, /* 10uV */
		.VMIN		= VMIN_VAL_GPU,
		.eem_v_base	= EEM_V_BASE,
		.eem_step	= EEM_STEP,
		.pmic_base	= GPU_PMIC_BASE,
		.pmic_step	= GPU_PMIC_STEP,
		.DETWINDOW	= DETWINDOW_VAL,
		.DTHI		= DTHI_VAL,
		.DTLO		= DTLO_VAL,
		.DETMAX		= DETMAX_VAL,
		.AGECONFIG	= AGECONFIG_VAL,
		.AGEM		= AGEM_VAL,
#if ENABLE_LOO
		.loo_role       = LOW_BANK,
		.loo_couple     = EEM_CTRL_GPU_HI,
		.loo_mutex      = &gpu_mutex,
		.DVTFIXED	= DVTFIXED_VAL_GL,
#else
		.DVTFIXED	= DVTFIXED_VAL_GPU,
#endif
		.VCO		= VCO_VAL_GL,
		.DCCONFIG	= DCCONFIG_VAL,
		.EEMCTL0	= EEM_CTL0_GPU,
		.low_temp_off	= LOW_TEMP_OFF_DEFAULT,
	},

#if ENABLE_MDLA
	[EEM_DET_MDLA] = {
		.name		= __stringify(EEM_DET_MDLA),
		.ops		= &mdla_det_ops,
#ifdef EEM_OFFSET_PROC_SHOW
		.volt_offset	= 0,
#endif
		.ctrl_id	= EEM_CTRL_MDLA,
		.features	= FEA_INIT01 | FEA_CORN,
		.max_freq_khz	= 880000,/* MHz */
		.VBOOT		= VBOOT_VAL_VPU, /* 10uV */
		.VMAX		= VMAX_VAL_VPU,
		.VMIN		= VMIN_VAL_VPU,
		.eem_v_base	= EEM_V_BASE,
		.eem_step	= EEM_STEP,
		.pmic_base	= CPU_PMIC_BASE_6359,
		.pmic_step	= CPU_PMIC_STEP,
		.DETWINDOW	= DETWINDOW_VAL,
		.DTHI		= DTHI_VAL,
		.DTLO		= DTLO_VAL,
		.DETMAX		= DETMAX_VAL,
		.AGECONFIG	= AGECONFIG_VAL,
		.AGEM		= AGEM_VAL,
		.DVTFIXED	= DVTFIXED_VAL_VPU,
		.VCO		= VCO_VAL_VPU,
		.DCCONFIG	= DCCONFIG_VAL,
		.EEMCTL0	= EEM_CTL0_MDLA,
		.low_temp_off	= LOW_TEMP_OFF_DEFAULT,
	},
#endif

#if ENABLE_VPU
	[EEM_DET_VPU] = {
		.name		= __stringify(EEM_DET_VPU),
		.ops		= &vpu_det_ops,
#ifdef EEM_OFFSET_PROC_SHOW
		.volt_offset	= 0,
#endif
		.ctrl_id	= EEM_CTRL_VPU,
		.features	= FEA_INIT01 | FEA_CORN,
		.max_freq_khz	= 880000,/* MHz */
		.VBOOT		= VBOOT_VAL_VPU, /* 10uV */
		.VMAX		= VMAX_VAL_VPU,
		.VMIN		= VMIN_VAL_VPU,
		.eem_v_base	= EEM_V_BASE,
		.eem_step	= EEM_STEP,
		.pmic_base	= CPU_PMIC_BASE_6359,
		.pmic_step	= CPU_PMIC_STEP,
		.DETWINDOW	= DETWINDOW_VAL,
		.DTHI		= DTHI_VAL,
		.DTLO		= DTLO_VAL,
		.DETMAX		= DETMAX_VAL,
		.AGECONFIG	= AGECONFIG_VAL,
		.AGEM		= AGEM_VAL,
		.DVTFIXED	= DVTFIXED_VAL_VPU,
		.VCO		= VCO_VAL_VPU,
		.DCCONFIG	= DCCONFIG_VAL,
		.EEMCTL0	= EEM_CTL0_VPU,
		.low_temp_off	= LOW_TEMP_OFF_DEFAULT,
	},
#endif

#if ENABLE_LOO

	[EEM_DET_GPU_HI] = {
		.name		= __stringify(EEM_DET_GPU_HI),
		.ops		= &gpu_det_ops,
#ifdef EEM_OFFSET_PROC_SHOW
		.volt_offset	= 0,
#endif
		.ctrl_id	= EEM_CTRL_GPU_HI,
		.features	= FEA_INIT02,
		.max_freq_khz	= 970000,
		.VBOOT		= VBOOT_VAL, /* 10uV */
		.VMAX		= VMAX_VAL_GH,
		.VMIN		= VMIN_VAL_GH,
		.eem_v_base	= EEM_V_BASE,
		.eem_step	= EEM_STEP,
		.pmic_base	= CPU_PMIC_BASE_6359,
		.pmic_step	= CPU_PMIC_STEP,
		.DETWINDOW	= DETWINDOW_VAL,
		.DTHI		= DTHI_VAL,
		.DTLO		= DTLO_VAL,
		.DETMAX		= DETMAX_VAL,
		.AGECONFIG	= AGECONFIG_VAL,
		.AGEM		= AGEM_VAL,
		.DVTFIXED	= DVTFIXED_VAL_GPU,
		.loo_role       = HIGH_BANK,
		.loo_couple     = EEM_CTRL_GPU,
		.loo_mutex      = &gpu_mutex,
		.VCO		= VCO_VAL_GH,
		.DCCONFIG	= DCCONFIG_VAL,
		.EEMCTL0	= EEM_CTL0_GPU,
		.low_temp_off	= LOW_TEMP_OFF_DEFAULT,

	},
#endif

};

#if DUMP_DATA_TO_DE
const unsigned int reg_dump_addr_off[DUMP_LEN] = {
	0x0000,
	0x0004,
	0x0008,
	0x000C,
	0x0010,
	0x0014,
	0x0018,
	0x001c,
	0x0024,
	0x0028,
	0x002c,
	0x0030,
	0x0034,
	0x0038,
	0x003c,
	0x0040,
	0x0044,
	0x0048,
	0x004c,
	0x0050,
	0x0054,
	0x0058,
	0x005c,
	0x0060,
	0x0064,
	0x0068,
	0x006c,
	0x0070,
	0x0074,
	0x0078,
	0x007c,
	0x0080,
	0x0084,
	0x0088,
	0x008c,
	0x0090,
	0x0094,
	0x0098,
	0x00a0,
	0x00a4,
	0x00a8,
	0x00B0,
	0x00B4,
	0x00B8,
	0x00BC,
	0x00C0,
	0x00C4,
	0x00C8,
	0x00CC,
	0x00F0,
	0x00F4,
	0x00F8,
	0x00FC,
	0x0190, /* dump this for gpu thermal */
	0x0194, /* dump this for gpu thermal */
	0x0198, /* dump this for gpu thermal */
	0x01B8, /* dump this for gpu thermal */
	0x0C00,
	0x0C04,
	0x0C08,
	0x0C0C,
	0x0C10,
	0x0C14,
	0x0C18,
	0x0C1C,
	0x0C20,
	0x0C24,
	0x0C28,
	0x0C2C,
	0x0C30,
	0x0C34,
	0x0C38,
	0x0C3C,
	0x0C40,
	0x0C44,
	0x0C48,
	0x0C4C,
	0x0C50,
	0x0C54,
	0x0C58,
	0x0C5C,
	0x0C60,
	0x0C64,
	0x0C68,
	0x0C6C,
	0x0C70,
	0x0C74,
	0x0C78,
	0x0C7C,
	0x0C80,
	0x0C84,
	0x0C88, /* dump thermal sensor */
	0x0F00,
	0x0F04,
	0x0F08,
	0x0F0C,
	0x0F10,
	0x0F14,
	0x0F18,
	0x0F1C,
	0x0F20,
	0x0F24,
	0x0F28,
	0x0F2C,
	0x0F30,
#if DVT
	0x0800,
	0x0804,
	0x0808,
	0x080C,
	0x0810,
	0x0814,
	0x0818,
	0x081c,
	0x0824,
	0x0828,
	0x082c,
	0x0830,
	0x0834,
	0x0838,
	0x083c,
	0x0840,
	0x0844,
	0x0848,
	0x084c,
	0x0850,
	0x0854,
	0x0858,
	0x085c,
	0x0860,
	0x0864,
	0x0868,
	0x086c,
	0x0870,
	0x0874,
	0x0878,
	0x087c,
	0x0880,
	0x0884,
	0x0888,
	0x088c,
	0x0890,
	0x0894,
	0x0898,
	0x08a0,
	0x08a4,
	0x08a8,
	0x08B0,
	0x08B4,
	0x08B8,
	0x08BC,
	0x08C0,
	0x08C4,
	0x08C8,
	0x08CC,
	0x08D0,
	0x08D4,
	0x08D8,
	0x08DC,
	0x08E0,
	0x08E4,
	0x08E8,
	0x08EC,
	0x08F0,
	0x08F4,
	0x08F8,
	0x08FC,
	0x0900,
	0x0904,
	0x0908,
	0x090C,
	0x0910,
	0x0914,
	0x0918,
	0x091c,
	0x0924,
	0x0928,
	0x092c,
	0x0930,
	0x0934,
	0x0938,
	0x093c,
	0x0940,
	0x0944,
	0x0948,
	0x094c,
	0x0950,
	0x0954,
	0x0958,
	0x095c,
	0x0960,
	0x0964,
	0x0968,
	0x096c,
	0x0970,
	0x0974,
	0x0978,
	0x097c,
	0x0980,
	0x0984,
	0x0988,
	0x098c,
	0x0990,
	0x0994,
	0x0998,
	0x09a0,
	0x09a4,
	0x09a8,
	0x09B0,
	0x09B4,
	0x09B8,
	0x09BC,
	0x09C0,
	0x09C4,
	0x09C8,
	0x09CC,
	0x09D0,
	0x09D4,
	0x09D8,
	0x09DC,
	0x09E0,
	0x09E4,
	0x09E8,
	0x09EC,
	0x09F0,
	0x09F4,
	0x09F8,
	0x09FC,
	0x0a00,
	0x0a04,
	0x0a08,
	0x0a0C,
	0x0a10,
	0x0a14,
	0x0a18,
	0x0a1c,
	0x0a24,
	0x0a28,
	0x0a2c,
	0x0a30,
	0x0a34,
	0x0a38,
	0x0a3c,
	0x0a40,
	0x0a44,
	0x0a48,
	0x0a4c,
	0x0a50,
	0x0a54,
	0x0a58,
	0x0a5c,
	0x0a60,
	0x0a64,
	0x0a68,
	0x0a6c,
	0x0a70,
	0x0a74,
	0x0a78,
	0x0a7c,
	0x0a80,
	0x0a84,
	0x0a88,
	0x0a8c,
	0x0a90,
	0x0a94,
	0x0a98,
	0x0aa0,
	0x0aa4,
	0x0aa8,
	0x0aB0,
	0x0aB4,
	0x0aB8,
	0x0aBC,
	0x0aC0,
	0x0aC4,
	0x0aC8,
	0x0aCC,
	0x0aD0,
	0x0aD4,
	0x0aD8,
	0x0aDC,
	0x0aE0,
	0x0aE4,
	0x0aE8,
	0x0aEC,
	0x0aF0,
	0x0aF4,
	0x0aF8,
	0x0aFC,
	0x0100,
	0x0104,
	0x0108,
	0x010C,
	0x0110,
	0x0114,
	0x0118,
	0x011c,
	0x0124,
	0x0128,
	0x012c,
	0x0130,
	0x0134,
	0x0138,
	0x013c,
	0x0140,
	0x0144,
	0x0148,
	0x014c,
	0x0150,
	0x0154,
	0x0158,
	0x015c,
	0x0160,
	0x0164,
	0x0168,
	0x016c,
	0x0170,
	0x0174,
	0x0178,
	0x017c,
	0x0180,
	0x0184,
	0x0188,
	0x018c,
	0x0190,
	0x0194,
	0x0198,
	0x01a0,
	0x01a4,
	0x01a8,
	0x01B0,
	0x01B4,
	0x01B8,
	0x01BC,
	0x01C0,
	0x01C4,
	0x01C8,
	0x01CC,
	0x01D0,
	0x01D4,
	0x01D8,
	0x01DC,
	0x01E0,
	0x01E4,
	0x01E8,
	0x01EC,
	0x01F0,
	0x01F4,
	0x01F8,
	0x01FC,
	0x0200,
	0x0204,
	0x0208,
	0x020C,
	0x0210,
	0x0214,
	0x0218,
	0x021c,
	0x0224,
	0x0228,
	0x022c,
	0x0230,
	0x0234,
	0x0238,
	0x023c,
	0x0240,
	0x0244,
	0x0248,
	0x024c,
	0x0250,
	0x0254,
	0x0258,
	0x025c,
	0x0260,
	0x0264,
	0x0268,
	0x026c,
	0x0270,
	0x0274,
	0x0278,
	0x027c,
	0x0280,
	0x0284,
	0x0288,
	0x028c,
	0x0290,
	0x0294,
	0x0298,
	0x02a0,
	0x02a4,
	0x02a8,
	0x02B0,
	0x02B4,
	0x02B8,
	0x02BC,
	0x02C0,
	0x02C4,
	0x02C8,
	0x02CC,
	0x02D0,
	0x02D4,
	0x02D8,
	0x02DC,
	0x02E0,
	0x02E4,
	0x02E8,
	0x02EC,
	0x02F0,
	0x02F4,
	0x02F8,
	0x02FC,
#endif
};
#endif
#undef __MT_EEM_INTERNAL_C__
