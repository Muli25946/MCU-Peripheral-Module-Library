/**
 * @file ads1220_def.h
 * @author Muli25946 (2687025869@qq.com)
 * @note 该文件由AI辅助生成，注意甄别
 * @version 0.1
 * @date 2026-04-11
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef ADS1220_DEF_H
#define ADS1220_DEF_H

/* 命令字节 */
#define ADS1220_CMD_RESET     0x06 /**< 复位 */
#define ADS1220_CMD_START     0x08 /**< 启动/同步转换 */
#define ADS1220_CMD_POWERDOWN 0x02 /**< 掉电 */
#define ADS1220_CMD_RDATA     0x10 /**< 读取转换结果 */
#define ADS1220_CMD_RREG      0x20 /**< 读寄存器基址 */
#define ADS1220_CMD_WREG      0x40 /**< 写寄存器基址 */

/* 寄存器地址 */
#define ADS1220_REG0 0x00 /**< 配置寄存器0：输入MUX和增益 */
#define ADS1220_REG1 0x01 /**< 配置寄存器1：数据速率和工作模式 */
#define ADS1220_REG2 0x02 /**< 配置寄存器2：基准电压和IDAC */
#define ADS1220_REG3 0x03 /**< 配置寄存器3：IDAC路由 */

/* 配置寄存器0 — 输入多路复用器(MUX[3:0]) */
#define ADS1220_MUX_AIN0_AIN1   0x00 /**< 差分 AINp=AIN0, AINn=AIN1（默认） */
#define ADS1220_MUX_AIN0_AIN2   0x10 /**< 差分 AINp=AIN0, AINn=AIN2 */
#define ADS1220_MUX_AIN0_AIN3   0x20 /**< 差分 AINp=AIN0, AINn=AIN3 */
#define ADS1220_MUX_AIN1_AIN2   0x30 /**< 差分 AINp=AIN1, AINn=AIN2 */
#define ADS1220_MUX_AIN1_AIN3   0x40 /**< 差分 AINp=AIN1, AINn=AIN3 */
#define ADS1220_MUX_AIN2_AIN3   0x50 /**< 差分 AINp=AIN2, AINn=AIN3 */
#define ADS1220_MUX_AIN1_AIN0   0x60 /**< 差分 AINp=AIN1, AINn=AIN0（反向） */
#define ADS1220_MUX_AIN3_AIN2   0x70 /**< 差分 AINp=AIN3, AINn=AIN2（反向） */
#define ADS1220_MUX_AIN0_AVSS   0x80 /**< 单端 AINp=AIN0, AINn=AVSS */
#define ADS1220_MUX_AIN1_AVSS   0x90 /**< 单端 AINp=AIN1, AINn=AVSS */
#define ADS1220_MUX_AIN2_AVSS   0xA0 /**< 单端 AINp=AIN2, AINn=AVSS */
#define ADS1220_MUX_AIN3_AVSS   0xB0 /**< 单端 AINp=AIN3, AINn=AVSS */
#define ADS1220_MUX_REFP_REFN   0xC0 /**< (REFPx-REFNx)/4 监测 */
#define ADS1220_MUX_AVDD_AVSS   0xD0 /**< (AVDD-AVSS)/4 监测 */
#define ADS1220_MUX_SHORTED     0xE0 /**< 输入短接（噪声测试） */

/* 配置寄存器0 — PGA增益(GAIN[2:0]) */
#define ADS1220_GAIN_1   0x00 /**< 增益=1（默认） */
#define ADS1220_GAIN_2   0x02 /**< 增益=2 */
#define ADS1220_GAIN_4   0x04 /**< 增益=4 */
#define ADS1220_GAIN_8   0x06 /**< 增益=8 */
#define ADS1220_GAIN_16  0x08 /**< 增益=16 */
#define ADS1220_GAIN_32  0x0A /**< 增益=32 */
#define ADS1220_GAIN_64  0x0C /**< 增益=64 */
#define ADS1220_GAIN_128 0x0E /**< 增益=128 */

/* 配置寄存器0 — PGA使能(PGA_BYPASS) */
#define ADS1220_PGA_ENABLED  0x00 /**< PGA使能（默认） */
#define ADS1220_PGA_DISABLED 0x01 /**< PGA旁路 */

/* 配置寄存器1 — 数据速率(DR[2:0]) */
#define ADS1220_DR_20SPS   0x00 /**< 20 SPS（默认） */
#define ADS1220_DR_45SPS   0x20 /**< 45 SPS */
#define ADS1220_DR_90SPS   0x40 /**< 90 SPS */
#define ADS1220_DR_175SPS  0x60 /**< 175 SPS */
#define ADS1220_DR_330SPS  0x80 /**< 330 SPS */
#define ADS1220_DR_600SPS  0xA0 /**< 600 SPS */
#define ADS1220_DR_1000SPS 0xC0 /**< 1000 SPS */

/* 配置寄存器1 — 工作模式(MODE[1:0]) */
#define ADS1220_MODE_NORMAL 0x00 /**< 正常模式（默认） */
#define ADS1220_MODE_DUTY   0x08 /**< 占空比模式（省电） */
#define ADS1220_MODE_TURBO  0x10 /**< Turbo模式（高速） */
#define ADS1220_MODE_DCT    0x18 /**< DCT滤波器测试模式 */

/* 配置寄存器1 — 转换模式(CM) */
#define ADS1220_CM_SINGLE     0x00 /**< 单次转换（默认） */
#define ADS1220_CM_CONTINUOUS 0x04 /**< 连续转换 */

/* 配置寄存器1 — 温度传感器(TS) */
#define ADS1220_TS_DISABLED 0x00 /**< 关闭内部温度传感器（默认） */
#define ADS1220_TS_ENABLED  0x02 /**< 使能内部温度传感器 */

/* 配置寄存器1 — 烧毁电流源(BCS) */
#define ADS1220_BCS_OFF 0x00 /**< 关闭（默认） */
#define ADS1220_BCS_ON  0x01 /**< 打开（10μA） */

/* 配置寄存器2 — 基准电压源(VREF[1:0]) */
#define ADS1220_VREF_INT      0x00 /**< 内部2.048V（默认） */
#define ADS1220_VREF_EXT_REF0 0x40 /**< 外部REFP0-REFN0 */
#define ADS1220_VREF_EXT_AIN  0x80 /**< AIN0/REFP1作为基准 */
#define ADS1220_VREF_AVDD     0xC0 /**< AVDD-AVSS作为基准 */

/* 配置寄存器2 — FIR滤波器(FIR[1:0]) */
#define ADS1220_FIR_NONE      0x00 /**< 无滤波（默认） */
#define ADS1220_FIR_50HZ_60HZ 0x10 /**< 同时抑制50Hz和60Hz */
#define ADS1220_FIR_50HZ      0x20 /**< 抑制50Hz */
#define ADS1220_FIR_60HZ      0x30 /**< 抑制60Hz */

/* 配置寄存器2 — 低边电源开关(PSW) */
#define ADS1220_PSW_OPEN 0x00 /**< 始终断开（默认） */
#define ADS1220_PSW_AUTO 0x08 /**< 自动控制 */

/* 配置寄存器2 — IDAC电流(IDAC[2:0]) */
#define ADS1220_IDAC_OFF    0x00 /**< 关闭（默认） */
#define ADS1220_IDAC_10UA   0x01 /**< 10 μA */
#define ADS1220_IDAC_50UA   0x02 /**< 50 μA */
#define ADS1220_IDAC_100UA  0x03 /**< 100 μA */
#define ADS1220_IDAC_250UA  0x04 /**< 250 μA */
#define ADS1220_IDAC_500UA  0x05 /**< 500 μA */
#define ADS1220_IDAC_1000UA 0x06 /**< 1000 μA */
#define ADS1220_IDAC_1500UA 0x07 /**< 1500 μA */

/* 配置寄存器3 — IDAC1路由(I1MUX[2:0]) */
#define ADS1220_I1MUX_DISABLED 0x00 /**< 禁用（默认） */
#define ADS1220_I1MUX_AIN0     0x20 /**< 连接到AIN0 */
#define ADS1220_I1MUX_AIN1     0x40 /**< 连接到AIN1 */
#define ADS1220_I1MUX_AIN2     0x60 /**< 连接到AIN2 */
#define ADS1220_I1MUX_AIN3     0x80 /**< 连接到AIN3 */
#define ADS1220_I1MUX_REFP0    0xA0 /**< 连接到REFP0 */
#define ADS1220_I1MUX_REFN0    0xC0 /**< 连接到REFN0 */

/* 配置寄存器3 — IDAC2路由(I2MUX[2:0]) */
#define ADS1220_I2MUX_DISABLED 0x00 /**< 禁用（默认） */
#define ADS1220_I2MUX_AIN0     0x04 /**< 连接到AIN0 */
#define ADS1220_I2MUX_AIN1     0x08 /**< 连接到AIN1 */
#define ADS1220_I2MUX_AIN2     0x0C /**< 连接到AIN2 */
#define ADS1220_I2MUX_AIN3     0x10 /**< 连接到AIN3 */
#define ADS1220_I2MUX_REFP0    0x14 /**< 连接到REFP0 */
#define ADS1220_I2MUX_REFN0    0x18 /**< 连接到REFN0 */

/* 配置寄存器3 — DRDY模式(DRDYM) */
#define ADS1220_DRDYM_DRDY_ONLY 0x00 /**< DRDY引脚仅作为数据就绪输出（默认） */
#define ADS1220_DRDYM_DOUT      0x02 /**< DRDY引脚复用为DOUT */

/* 24位ADC满量程 2^23 */
#define ADS1220_FULL_SCALE 8388608

#endif /* ADS1220_DEF_H */
